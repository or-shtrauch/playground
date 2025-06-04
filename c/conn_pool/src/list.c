#include <stddef.h>
#include <stdlib.h>
#include "list.h"

list_t *list_create()
{
	list_t *list = NULL;

	list = calloc(1, sizeof(list_t));

	if (!list) {
		return NULL;
	}

	return list;
}

void list_destroy(list_t *list)
{
	node_t *tmp = NULL;
	node_t *prev = NULL;

	if (list == NULL) {
		return;
	}

	tmp = list->head;

	while (tmp != NULL) {
		prev = tmp;
		tmp = tmp->next;

		free(prev);
	}

	free(list);
}

static node_t *create_node(int value)
{
	node_t *node = calloc(1, sizeof(node_t));
	if (!node) {
		return NULL;
	}

	node->value = value;

	return node;
}

int list_insert_head(list_t *list, int value)
{
	node_t *node = NULL;

	if (!list) {
		return -1;
	}

	node = create_node(value);
	if (!node) {
		return -1;
	}

	if (!list->head) {
		list->head = node;
		list->tail = node;
	} else {
		node->next = list->head;
		list->head->prev = node;
		list->head = node;
	}

	list->size++;
	return 0;
}

int list_insert_tail(list_t *list, int value)
{
	node_t *node = NULL;

	if (!list) {
		return -1;
	}

	node = create_node(value);
	if (!node) {
		return -1;
	}

	if (!list->tail) {
		list->head = node;
		list->tail = node;
	} else {
		list->tail->next = node;
		node->prev = list->tail;
		list->tail = node;
	}

	list->size++;
	return 0;
}

int list_remove_head(list_t *list)
{
	node_t *tmp = NULL;

	if (!list || list->size == 0) {
		return -1;
	}

	tmp = list->head;

	if (list->head == list->tail) {
		list->head = NULL;
		list->tail = NULL;
	} else {
		list->head = list->head->next;
		list->head->prev = NULL;
	}

	free(tmp);
	list->size--;

	return 0;
}

int list_remove_tail(list_t *list)
{
	node_t *tmp = NULL;

	if (!list || list->size == 0) {
		return -1;
	}

	tmp = list->tail;

	if (list->head == list->tail) {
		list->head = NULL;
		list->tail = NULL;
	} else {
		list->tail = list->tail->prev;
		list->tail->next = NULL;
	}

	free(tmp);
	list->size--;

	return 0;
}

size_t list_size(list_t *list)
{
	if (!list) {
		return 0;
	}

	return list->size;
}

int list_is_empty(list_t *list)
{
	return list == NULL || list->size == 0;
}