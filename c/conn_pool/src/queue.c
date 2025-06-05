#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "list.h"

queue_t *queue_create()
{
	queue_t *q = NULL;

	q = calloc(1, sizeof(queue_t));
	if (!q) {
		return NULL;
	}

	q->list = list_create();
	if (!q->list) {
		goto err;
	}

	return q;

err:
	if (q) {
		list_destroy(q->list);
		free(q);
	}

	return NULL;
}

void queue_destroy(queue_t *q)
{
	if (!q) {
		return;
	}

	list_destroy(q->list);
	free(q);
}

int queue_enqueue(queue_t *q, int value)
{
	int err = 0;

	if (!q || !q->list) {
		return -1;
	}

	err = list_insert_tail(q->list, value);
	return err;
}

int queue_dequeue(queue_t *q)
{
	int val = INT_MIN;

	if (!q || !q->list) {
		return -1;
	}

	if (q->list->head) {
		val = q->list->head->value;
		list_remove_head(q->list);
	}

	return val;
}

int queue_front(queue_t *q)
{
	int value = INT_MIN;

	if (q || q->list->head) {
		value = q->list->head->value;
	}

out:
	return value;
}

size_t queue_size(queue_t *q)
{
	size_t size = 0;

	if (!q || !q->list) {
		return 0;
	}

	size = list_size(q->list);

	return size;
}

int queue_is_empty(queue_t *q)
{
	return list_is_empty(q->list);
}
void queue_clear(queue_t *q)
{
	if (!q || !q->list) {
		return;
	}

	list_destroy(q->list);
	q->list = list_create();
}

// thread_queue_t *thread_queue_create()
// {
// 	thread_queue_t *tq = NULL;

// 	tq = calloc(1, sizeof(thread_queue_t));
// 	if (!tq) {
// 		return NULL;
// 	}

// 	tq->list = list_create();
// 	if (!tq->list) {
// 		goto err;
// 	}

// 	if (pthread_mutex_init(&tq->mutex, NULL) != 0) {
// 		goto err;
// 	}

// 	if (pthread_cond_init(&tq->cond, NULL) != 0) {
// 		goto err;
// 	}

// 	return tq;
// err:
// 	if (tq) {
// 		pthread_mutex_destroy(&tq->mutex);
// 		pthread_cond_destroy(&tq->cond);
// 		list_destroy(tq->list);
// 		free(tq);
// 	}

// 	return NULL;
// }

// void thread_queue_destroy(thread_queue_t *tq)
// {
// 	if (!tq) {
// 		return;
// 	}

// 	list_destroy(tq->list);
// 	pthread_mutex_destroy(&tq->mutex);
// 	pthread_cond_destroy(&tq->cond);

// 	free(tq);
// }

// int thread_queue_enqueue(thread_queue_t *tq, int value)
// {
// 	int err = 0;

// 	if (!tq || !tq->list) {
// 		return -1;
// 	}

// 	pthread_mutex_lock(&tq->mutex);
// 	err = list_insert_tail(tq->list, value);

// 	if (err == 0) {
// 		pthread_cond_broadcast(&tq->cond);
// 	}

// 	pthread_mutex_unlock(&tq->mutex);

// 	return err;
// }
// int thread_queue_dequeue(thread_queue_t *tq)
// {
// 	int val = INT_MIN;

// 	if (!tq || !tq->list) {
// 		return -1;
// 	}

// 	pthread_mutex_lock(&tq->mutex);

// 	while (list_is_empty(tq->list)) {
// 		printf("thread: %lu -- pthread_cond_wait\n",
// 		       (unsigned long)pthread_self());
// 		pthread_cond_wait(&tq->cond, &tq->mutex);
// 	}

// 	if (tq->list->head) {
// 		val = tq->list->head->value;
// 		printf("thread: %lu -- size: %zu\n",
// 		       (unsigned long)pthread_self(), tq->list->size);
// 		list_remove_head(tq->list);
// 		printf("thread: %lu -- size: %zu\n",
// 		       (unsigned long)pthread_self(), tq->list->size);
// 	}

// 	pthread_mutex_unlock(&tq->mutex);

// 	printf("thread: %lu -- done dq\n", (unsigned long)pthread_self());

// 	return val;
// }

// int thread_queue_front(thread_queue_t *tq)
// {
// 	int value = INT_MIN;

// 	if (!tq || !tq->list) {
// 		goto out;
// 	}

// 	pthread_mutex_lock(&tq->mutex);

// 	if (tq->list->head) {
// 		value = tq->list->head->value;
// 	}

// 	pthread_mutex_unlock(&tq->mutex);

// out:
// 	return value;
// }

// size_t thread_queue_size(thread_queue_t *tq)
// {
// 	size_t size = 0;

// 	if (!tq || !tq->list) {
// 		return 0;
// 	}

// 	pthread_mutex_lock(&tq->mutex);
// 	size = list_size(tq->list);
// 	pthread_mutex_unlock(&tq->mutex);

// 	return size;
// }

// void thread_queue_clear(thread_queue_t *tq)
// {
// 	if (!tq || !tq->list) {
// 		return;
// 	}

// 	list_destroy(tq->list);
// 	tq->list = list_create();
// }