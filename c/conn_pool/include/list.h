#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>

struct node;
typedef struct node node_t;

struct node {
	int value;
	node_t *next;
	node_t *prev;
};

typedef struct {
	size_t size;
	node_t *head;
	node_t *tail;
} list_t;

list_t *list_create(void);
void list_destroy(list_t *list);

int list_insert_head(list_t *list, int value);
int list_insert_tail(list_t *list, int value);
int list_remove_head(list_t *list);
int list_remove_tail(list_t *list);

size_t list_size(list_t *list);
int list_is_empty(list_t *list);

#endif /* __LIST_H__ */