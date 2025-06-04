#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "thread_queue.h"
#include "list.h"

thread_queue_t *thread_queue_create()
{
	thread_queue_t *tq = NULL;

	tq = calloc(1, sizeof(thread_queue_t));
	if (!tq) {
		return NULL;
	}

	tq->list = list_create();
	if (!tq->list) {
		goto err;
	}

	if (pthread_mutex_init(&tq->mutex, NULL) != 0) {
		goto err;
	}

	if (pthread_cond_init(&tq->cond, NULL) != 0) {
		goto err;
	}

	return tq;
err:
	if (tq) {
		pthread_mutex_destroy(&tq->mutex);
		pthread_cond_destroy(&tq->cond);
		list_destroy(tq->list);
		free(tq);
	}

	return NULL;
}

void thread_queue_destroy(thread_queue_t *tq)
{
	if (!tq) {
		return;
	}

	list_destroy(tq->list);
	pthread_mutex_destroy(&tq->mutex);
	pthread_cond_destroy(&tq->cond);

	free(tq);
}

int thread_queue_enqueue(thread_queue_t *tq, int value)
{
	int err = 0;

	if (!tq || !tq->list) {
		return -1;
	}

	pthread_mutex_lock(&tq->mutex);
	err = list_insert_tail(tq->list, value);

	if (err == 0) {
		pthread_cond_signal(&tq->cond);
	}

	pthread_mutex_unlock(&tq->mutex);

	return err;
}
int thread_queue_dequeue(thread_queue_t *tq)
{
	int val = INT_MIN;

	if (!tq || !tq->list) {
		return -1;
	}

	pthread_mutex_lock(&tq->mutex);

	while (list_is_empty(tq->list)) {
		pthread_cond_wait(&tq->cond, &tq->mutex);
	}

	if (tq->list->head) {
		val = tq->list->head->value;
		list_remove_head(tq->list);
	}

	pthread_mutex_unlock(&tq->mutex);

	return val;
}

int thread_queue_front(thread_queue_t *tq)
{
	int value = INT_MIN;

	if (!tq || !tq->list) {
		goto out;
	}

	pthread_mutex_lock(&tq->mutex);

	if (tq->list->head) {
		value = tq->list->head->value;
	}

	pthread_mutex_unlock(&tq->mutex);

out:
	return value;
}

size_t thread_queue_size(thread_queue_t *tq)
{
	size_t size = 0;

	if (!tq || !tq->list) {
		return 0;
	}

	pthread_mutex_lock(&tq->mutex);
	size = list_size(tq->list);
	pthread_mutex_unlock(&tq->mutex);

	return size;
}

void thread_queue_clear(thread_queue_t *tq)
{
	if (!tq || !tq->list) {
		return;
	}

	list_destroy(tq->list);
	tq->list = list_create();
}