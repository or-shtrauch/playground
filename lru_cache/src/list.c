#include <stdlib.h>
#include <string.h>

#include "lru/errors.h"
#include "lru/list.h"

void list_add(list_t *list, node_t *node)
{
    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
        list->size = 1;
    }
    else
    {
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
        list->size++;
    }
}

int list_remove(list_t *list, node_t *node)
{
    if (!node->next && !node->prev)
    {
        return E_NOT_FOUND;
    }

    if (node->prev != NULL)
    {
        node->prev->next = node->next;
    }
    else
    {
        list->head = node->next;
    }

    if (node->next != NULL)
    {
        node->next->prev = node->prev;
    }
    else
    {
        list->tail = node->prev;
    }

    list->size--;
    return E_OK;
}

node_t *list_get(list_t *list, const char* key)
{
    node_t *tmp = list->head;
    while (tmp != NULL)
    {
        if (strncmp(tmp->pair.key, key, tmp->pair.key_length) == 0)
        {
            return tmp;
        }
        tmp = tmp->next;
    }

    return NULL;
}