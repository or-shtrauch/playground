#ifndef __MAP_H__
#define __MAP_H__

#include <stdlib.h>
#include "lru/list.h"

typedef struct
{
    size_t capacity;
    size_t size;
    list_t *list;
} map_t;

typedef unsigned long long hash_t;

int map_add(map_t *map, const char *key, size_t key_length, int value);
int *map_get(map_t *map, const char *key, size_t key_length);

#endif /* __MAP_H__ */