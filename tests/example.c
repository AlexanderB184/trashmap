#define TRASHMAP_IMPL
#include "../trashmap.h"

#include <stdio.h>

int main() {
    trashmap_t map = {0};

    trashmap_init(&map, 16);

    trashmap_set(&map, "hello", "world");

    const char * world = trashmap_get(&map, "hello");

    printf("hello %s\n", world);

    return 0;
}