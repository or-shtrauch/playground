#include <stdio.h>
#include <stdlib.h>

#include "lru/errors.h"
#include "lru/map.h"

int main(void)
{
	map_t map = {0};

	map_add(&map, "age", 3, 15);
	map_add(&map, "id", 2, 789456);

	printf("size %zu cap: %zu\n", map.size, map.capacity);

	int *val = map_get(&map, "age", 3);

	printf("val is %d\n", val ? *val : -1);

	return E_OK;
}