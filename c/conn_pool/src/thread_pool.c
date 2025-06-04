#include "thread_queue.h"
#include <limits.h>
#include <stdlib.h>

#include "thread_pool.h"

thread_pool_t *thread_pool_create(size_t size, worker_func handler)
{
	int i;
	thread_pool_t *tp = NULL;

	tp = calloc(1, sizeof(thread_pool_t));
	if (!tp) {
		return NULL;
	}

	tp->queue = thread_queue_create();
	if (!tp->queue) {
		goto err;
	}

	if (pthread_rwlock_init(&tp->rw_lock, NULL) < 0) {
		goto err;
	}

	tp->should_run = 1;
	tp->size = size;
	tp->handler = handler;
	tp->threads = calloc(size, sizeof(pthread_t));

	if (!tp->threads) {
		goto err;
	}

	for (i = 0; i < size; i++) {
		if (pthread_create(&tp->threads[i], NULL, handler, tp) < 0) {
			thread_pool_set_should_run(tp, 0);

			for (; i >= 0; i--) {
				pthread_join(tp->threads[i], NULL);
			}

			goto err;
		}
	}

	return tp;

err:
	if (tp) {
		pthread_rwlock_destroy(&tp->rw_lock);
		thread_queue_destroy(tp->queue);
		free(tp->threads);
		free(tp);
	}

	return NULL;
}

int thread_pool_get_should_run(thread_pool_t *tp)
{
	int should_run = 0;

	if (!tp) {
		return 0;
	}

	pthread_rwlock_rdlock(&tp->rw_lock);
	should_run = tp->should_run;
	pthread_rwlock_unlock(&tp->rw_lock);

	return should_run;
}

int thread_pool_set_should_run(thread_pool_t *tp, int value)
{
	if (!tp) {
		return -1;
	}

	pthread_rwlock_wrlock(&tp->rw_lock);
	tp->should_run = value;
	pthread_rwlock_unlock(&tp->rw_lock);

	return 0;
}

int thread_pool_get_conn(thread_pool_t *tp)
{
	if (!tp || !tp->queue) {
		return INT_MIN;
	}

	return thread_queue_dequeue(tp->queue);
}

int thread_pool_push_conn(thread_pool_t *tp, int fd)
{
	if (!tp || !tp->queue) {
		return -1;
	}

	return thread_queue_enqueue(tp->queue, fd);
}

void thread_pool_destroy(thread_pool_t *tp)
{
	int i;

	if (!tp || !tp->threads) {
		return;
	}

	thread_pool_set_should_run(tp, 0);

	if (tp->queue) {
		pthread_cond_broadcast(&tp->queue->cond);
		pthread_mutex_unlock(&tp->queue->mutex);
	}

	for (i = 0; i < tp->size; i++) {
		pthread_join(tp->threads[i], NULL);
	}

	thread_queue_destroy(tp->queue);
	pthread_rwlock_destroy(&tp->rw_lock);
	free(tp->threads);
	free(tp);
}