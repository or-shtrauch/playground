#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include "thread_queue.h"
#include <pthread.h>

struct thread_pool;
typedef struct thread_pool thread_pool_t;

typedef void *(*worker_func)(void *);

struct thread_pool {
	size_t size;
	pthread_t *threads;
	worker_func handler;
	thread_queue_t *queue;
	pthread_rwlock_t rw_lock;
	int should_run;
};

thread_pool_t *thread_pool_create(size_t size, worker_func handler);
int thread_pool_get_should_run(thread_pool_t *tp);
int thread_pool_set_should_run(thread_pool_t *tp, int value);
int thread_pool_get_conn(thread_pool_t *tp);
int thread_pool_push_conn(thread_pool_t *tp, int fd);
void thread_pool_destroy(thread_pool_t *tp);

#endif /* __THREAD_POOL_H__ */