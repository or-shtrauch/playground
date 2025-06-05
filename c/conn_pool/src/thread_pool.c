#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "thread_pool.h"

thread_pool_t *thread_pool_create(size_t size, worker_func handler)
{
	int i;
	thread_pool_t *pool = NULL;

	pool = calloc(1, sizeof(thread_pool_t));
	if (!pool) {
		return NULL;
	}

	pool->queue = queue_create();
	if (!pool->queue) {
		goto err;
	}

	if (pthread_mutex_init(&pool->queue_mutex, NULL) < 0) {
		goto err;
	}

	if (pthread_cond_init(&pool->queue_cond, NULL) < 0) {
		goto err;
	}

	if (pthread_rwlock_init(&pool->should_run_rw_lock, NULL) < 0) {
		goto err;
	}

	pool->should_run = 1;
	pool->size = size;
	pool->handler = handler;
	pool->threads = calloc(size, sizeof(pthread_t));

	if (!pool->threads) {
		goto err;
	}

	for (i = 0; i < size; i++) {
		if (pthread_create(&pool->threads[i], NULL, handler, pool) <
		    0) {
			thread_pool_set_should_run(pool, 0);

			for (; i >= 0; i--) {
				pthread_join(pool->threads[i], NULL);
			}

			goto err;
		}
	}

	return pool;

err:
	if (pool) {
		pthread_mutex_destroy(&pool->queue_mutex);
		pthread_cond_destroy(&pool->queue_cond);
		pthread_rwlock_destroy(&pool->should_run_rw_lock);
		queue_destroy(pool->queue);
		free(pool->threads);
		free(pool);
	}

	return NULL;
}

int thread_pool_get_should_run(thread_pool_t *pool)
{
	int should_run = 0;

	if (!pool) {
		return 0;
	}

	pthread_rwlock_rdlock(&pool->should_run_rw_lock);
	should_run = pool->should_run;
	pthread_rwlock_unlock(&pool->should_run_rw_lock);

	return should_run;
}

int thread_pool_set_should_run(thread_pool_t *pool, int value)
{
	if (!pool) {
		return -1;
	}

	pthread_rwlock_wrlock(&pool->should_run_rw_lock);
	pool->should_run = value;
	pthread_rwlock_unlock(&pool->should_run_rw_lock);

	return 0;
}

int thread_pool_get_conn(thread_pool_t *pool)
{
	int val;

	if (!pool || !pool->queue) {
		return INT_MIN;
	}

	pthread_mutex_lock(&pool->queue_mutex);

	while (queue_is_empty(pool->queue)) {
		if (!thread_pool_get_should_run(pool)) {
			goto out;
		}

		// printf("thread: %lu -- pthread_cond_wait\n",
		//        (unsigned long)pthread_self());
		pthread_cond_wait(&pool->queue_cond, &pool->queue_mutex);
	}

	val = queue_dequeue(pool->queue);

out:
	pthread_mutex_unlock(&pool->queue_mutex);

	return val;
}

int thread_pool_push_conn(thread_pool_t *pool, int fd)
{
	int exit = 0;
	if (!pool || !pool->queue) {
		return -1;
	}

	pthread_mutex_lock(&pool->queue_mutex);
	exit = queue_enqueue(pool->queue, fd);

	pthread_cond_broadcast(&pool->queue_cond);

	pthread_mutex_unlock(&pool->queue_mutex);

	return exit;
}

size_t thread_pool_get_size(thread_pool_t *pool)
{
	size_t size;
	if (!pool || !pool->queue) {
		return 0;
	}

	pthread_mutex_lock(&pool->queue_mutex);
	size = queue_size(pool->queue);
	pthread_mutex_unlock(&pool->queue_mutex);

	return size;
}

void thread_pool_destroy(thread_pool_t *pool)
{
	int i;

	if (!pool || !pool->threads) {
		return;
	}

	thread_pool_set_should_run(pool, 0);

	if (pool->queue) {
		pthread_mutex_lock(&pool->queue_mutex);
		pthread_cond_broadcast(&pool->queue_cond);
		pthread_mutex_unlock(&pool->queue_mutex);
		printf("broadcast on end\n");
	}

	for (i = 0; i < pool->size; i++) {
		// printf("thread: main -- joining thread %lu\n",
		//        (unsigned long)pool->threads[i]);
		pthread_join(pool->threads[i], NULL);
		// printf("thread: main -- joined thread %lu\n",
		//        (unsigned long)pool->threads[i]);
	}

	if (pool->queue) {
		queue_destroy(pool->queue);
	}

	pthread_mutex_destroy(&pool->queue_mutex);
	pthread_cond_destroy(&pool->queue_cond);
	pthread_rwlock_destroy(&pool->should_run_rw_lock);
	free(pool->threads);
	free(pool);
}