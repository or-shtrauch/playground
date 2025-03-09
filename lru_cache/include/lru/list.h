#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>

struct node;
typedef struct node node_t;

typedef struct {
    const char *key;
    size_t key_length;
    int value;
} pair_t;

struct node {
    pair_t pair;
    node_t *next;
    node_t *prev;
};

typedef struct
{
    size_t size;
    node_t *head;
    node_t *tail;
} list_t;

void list_add(list_t *list, node_t *node);
int list_remove(list_t *list, node_t *node);
node_t *list_get(list_t *list, const char *key);

#endif /* __LIST_H__ */