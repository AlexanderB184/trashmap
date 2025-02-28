#define TRASHMAP_IMPL
#include "../trashmap.h"

#include <stdio.h>
#include <string.h>

char* my_strdup(const char * s) {
    size_t l = strlen(s);
    char* d = (char*)malloc(l+1);
    return strcpy(d, s);
}

int main()
{
    trashmap_t map;
    trashmap_init(&map, 4);
    char cmd[512], key[512], value[512];
    while (1) {
        int r = fscanf(stdin, "%s %s", cmd, key);
        (void)r;
        if (trashmap_strcmp(cmd, "get") == 0) {
            printf("map[\"%s\"] => \"%s\"\n", key, trashmap_get(&map, key));
        } else if (trashmap_strcmp(cmd, "set") == 0) {
            r = fscanf(stdin, "%s", value);
            (void)r;
            trashmap_set(&map, my_strdup(key), my_strdup(value));
            printf("map[\"%s\"] <= \"%s\"\n", key, value);
        } else if (trashmap_strcmp(cmd, "has") == 0) {
            if (trashmap_has(&map, key)) {
                printf("map[\"%s\"]? => TRUE\n", key);
            } else {
                printf("map[\"%s\"]? => FALSE\n", key);
            }
        } else if (trashmap_strcmp(cmd, "hash") == 0) {
            uint32_t hash = trashmap_hash(key);
            printf("hash(\"%s\") => %08x\n", key, hash);
        } else {
            printf("unrecognised command. try 'get', 'set', 'has', or 'hash'\n");
        }
    }
    return 0;
}
