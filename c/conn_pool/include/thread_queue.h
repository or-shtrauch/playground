#ifndef __THREAD_QUEUE_H__
#define __THREAD_QUEUE_H__

#include <pthread.h>

#include "list.h"

typedef struct {
	list_t *list;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} thread_queue_t;

thread_queue_t *thread_queue_create();
void thread_queue_destroy(thread_queue_t *tq);

int thread_queue_enqueue(thread_queue_t *tq, int value);
int thread_queue_dequeue(thread_queue_t *tq);
int thread_queue_front(thread_queue_t *tq);

size_t thread_queue_size(thread_queue_t *tq);
void thread_queue_clear(thread_queue_t *tq);

#endif /* __THREAD_QUEUE_H__ */