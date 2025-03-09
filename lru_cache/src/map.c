#include <string.h>

#include "lru/errors.h"
#include "lru/map.h"


#define INIT_CAPACITY 256

static hash_t djb2_hash(const char *buffer, size_t size)
{
    unsigned long hash = 5381;
    size_t i = 0;
    int c;
    while ((c = *buffer++ && i < size))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        i++;
    }

    return hash;
}

int map_add(map_t *map, const char *key, size_t key_length, int value)
{
    hash_t index = 0;
    node_t *node = NULL;

    if (!map || !key || key_length <= 0)
    {
        return E_INVAL;
    }

    if (map->capacity == 0 && !map->list)
    {
        map->capacity = INIT_CAPACITY;
        map->size = 0;

        map->list = calloc(map->capacity, sizeof(list_t));
        if (!map->list)
        {
            return E_NO_MEM;
        }
    }

    if (map->capacity == map->size)
    {
        // todo: realloc
    }

    index = djb2_hash(key, key_length) % map->capacity;
    if (index >= map->capacity) {
        return E_INVAL;
    }

    node = calloc(1, sizeof(node_t));
    if (!node)
    {
        return E_NO_MEM;
    }

    node->pair.key = strdup(key);
    node->pair.key_length = key_length;
    node->pair.value = value;
    list_add(&map->list[index], node);
    map->size++;

    return E_OK;
}

int *map_get(map_t *map, const char *key, size_t key_length)
{
    hash_t index = 0;
    node_t *node = NULL;

    if (!map || !key || key_length <= 0 ||
        map->capacity == 0 || map->size == 0 || !map->list)
    {
        return NULL;
    }

    index = djb2_hash(key, key_length) % map->capacity;
    if (index > map->capacity) {
        return NULL;
    }

    node = list_get(&map->list[index], key);
    if (!node) {
        return NULL;
    }

    return &node->pair.value;
}