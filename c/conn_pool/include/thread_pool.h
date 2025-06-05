#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include "queue.h"
#include <pthread.h>

struct thread_pool;
typedef struct thread_pool thread_pool_t;

typedef void *(*worker_func)(void *);

struct thread_pool {
	size_t size;
	pthread_t *threads;
	worker_func handler;
	queue_t *queue;
	pthread_mutex_t queue_mutex;
	pthread_cond_t queue_cond;
	pthread_rwlock_t should_run_rw_lock;
	int should_run;
};

thread_pool_t *thread_pool_create(size_t size, worker_func handler);
int thread_pool_get_should_run(thread_pool_t *pool);
int thread_pool_set_should_run(thread_pool_t *pool, int value);
int thread_pool_get_conn(thread_pool_t *pool);
int thread_pool_push_conn(thread_pool_t *pool, int fd);
size_t thread_pool_get_size(thread_pool_t *pool);
void thread_pool_destroy(thread_pool_t *pool);

#endif /* __THREAD_POOL_H__ */