#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <pthread.h>

#include "list.h"

// typedef struct {
// 	list_t *list;
// 	pthread_mutex_t mutex;
// 	pthread_cond_t cond;
// } thread_queue_t;

typedef struct {
	list_t *list;
} queue_t;

queue_t *queue_create();
void queue_destroy(queue_t *q);

int queue_enqueue(queue_t *q, int value);
int queue_dequeue(queue_t *q);
int queue_front(queue_t *q);

size_t queue_size(queue_t *q);
int queue_is_empty(queue_t *q);
void queue_clear(queue_t *q);

#endif /* __QUEUE_H__ */