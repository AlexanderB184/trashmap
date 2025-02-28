#ifndef TRASHMAP_H
#define TRASHMAP_H

/**
 * Header only library for a string to string style append only hashmap
 * This library is not thread safe.
 * Uses linear probing style hash map with an internal arena for map items.
 * Library works in C from c99 and C++ from c++20 without any warnings from `-Wall -Wextra -Wpedantic`.
 * This library was made for my own purposes for parsing HTTP headers but can be used in many contexts.
 * 
 * Using the library:
 * 
 * In exactly ONE file define TRASHMAP_IMPL prior to including trashmap.h e.g. 
 * 
 * #define TRASHMAP_IMPL
 * #include "trashmap.h"
 * // your code here
 * 
 * 
 * Features:
 * 
 * The default allocator (libc) for trashmap can be overwritten by creating custom defines for:
 * 
 * TRASHMAP_ALLOC(SIZE)
 * TRASHMAP_REALLOC(PTR, SIZE)
 * TRASHMAP_FREE(PTR)
 * 
 * All asserts can be removed by defining `TRASHMAP_STRIP_ASSERTS` prior to including or using `-DTRASHMAP_STRIP_ASSERTS`
 * Alternatively the default assert function can be overwritten by creating a custom define for:
 * 
 * TRASHMAP_ASSERT(COND)
 * 
 * To use a custom hash function define `TRASHMAP_CUSTOM_HASH` above the implementing include,
 * and then implement a function with exact same signature, i.e.
 * 
 * 
 * #define TRASHMAP_CUSTOM_HASH
 * #define TRASHMAP_IMPL
 * #include "trashmap.h"
 * 
 * // function must have this exact signature
 * uint32_t trashmap_hash(const char * key) {
 *     // custom hash function implementation
 *     // ...
 * }
 * 
 * Api:
 * 
 * trashmap_init: initialize an empty hashmap with `count` initial slots.
 * void trashmap_init(trashmap_t* map, size_t count);
 * 
 * trashmap_deinit: release all resources associated with hashmap.
 * void trashmap_deinit(trashmap_t* map);
 * 
 * trashmap_clear: empties the hashmap while retaining all allocated resources so they can be reused.
 * void trashmap_deinit(trashmap_t* map);
 * 
 * trashmap_hash: implementation of the FNV-1a hashing algorithm.
 * uint32_t trashmap_hash(const char * key);
 * 
 * trashmap_has: checks if the key appears in the hash map.
 * bool trashmap_has(const trashmap_t* map, const char * key);
 * 
 * trashmap_get: gets the associated value for the key, NULL if key does not appear in the hash map.
 * const char* trashmap_get(const trashmap_t* map, const char * key);
 * 
 * trashmap_set: inserts an element into the hash map, or updates the value if it already exists.
 * does NOT duplicate strings, ensure all strings are allocated somewhere permanently before passing to trashmap_set.
 * void trashmap_set(trashmap_t* map, const char * key, const char * value);
 * 
 * trashmap_reserve: reserves enough space for `extra` addition items
 * void trashmap_reserve(trashmap_t* map, size_t extra);
 * 
 * Reimplementation of needed string.h functionality. 
 * This removes string.h as a dependency and means that the only dependency is malloc/realloc/free
 * 
 * trashmap_strcmp: reimplementation of libc strcmp
 * int trashmap_strcmp(const char * lhs, const char * rhs);
 * 
 * trashmap_memset: reimplementation of libc memset
 * void * trashmap_memset(void * dest, unsigned char byte, size_t count);
 * 
 * 
 * 
 * TODO:
 * add trashmap_strdup
 * clones a string and stores it in an arena allocator which is released when trashmap_deinit is called
 * issues:
 * if trashmap_set overrides a value created with trashmap_strdup, that value is lost and cannot be recovered.
 * char * trashmap_strdup(trashmap_t* map, const char * str);
 * 
 * make variants with sized strings instead of c style null terminated strings
 * e.g.
 * const char* trashmap_get_n(const trashmap_t* map, const char * key, size_t key_len);
 * 
 * this works okay for get and has but to do this for set would require, 
 * either duplicating the string, or making the entire hashmap use sized strings.
*/

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct trashmap_slot_t {
    uint32_t hash;
    uint32_t index;
} trashmap_slot_t;

typedef struct trashmap_item_t {
    const char * key, * value;
} trashmap_item_t;

typedef struct trashmap_t {
    trashmap_slot_t * slots;
    trashmap_item_t * items;
    size_t slot_count;
    size_t count;
    size_t capacity;
} trashmap_t;

// initialize an empty hashmap with `count` initial slots.
void trashmap_init(trashmap_t* map, size_t count);

// release all resources associated with hashmap.
void trashmap_deinit(trashmap_t* map);

// empties the hashmap while retaining all allocated resources so they can be reused.
void trashmap_clear(trashmap_t* map);

// implementation of the FNV-1a hashing algorithm
uint32_t trashmap_hash(const char * key);

// reserves enough space for `extra` addition items
void trashmap_reserve(trashmap_t* map, size_t extra);

// reimplementation of libc strcmp
int trashmap_strcmp(const char * lhs, const char * rhs);

// reimplementation of libc memset
void * trashmap_memset(void * dest, unsigned char byte, size_t count);

// checks if the key appears in the hash map.
bool trashmap_has(const trashmap_t* map, const char * key);

// gets the associated value for the key, NULL if key does not appear in the hash map.
const char* trashmap_get(const trashmap_t* map, const char * key);

// inserts an element into the hash map, or updates the value if it already exists.
// does NOT duplicate strings, ensure all strings are allocated somewhere permanently before passing to trashmap_set.
void trashmap_set(trashmap_t* map, const char * key, const char * value);


// to use an alternate allocator define: TRASHMAP_ALLOC(SIZE), TRASHMAP_REALLOC(PTR, SIZE) and TRASHMAP_FREE(PTR)
#ifndef TRASHMAP_ALLOC
#include <stdlib.h>
#define TRASHMAP_ALLOC(SIZE) malloc(SIZE)
#define TRASHMAP_REALLOC(PTR, SIZE) realloc(PTR, SIZE)
#define TRASHMAP_FREE(PTR) free(PTR)
#endif // TRASHMAP_ALLOC

// to use a strip assets, use `-DTRASHMAP_STRIP_ASSERTS`
#ifdef TRASHMAP_STRIP_ASSERTS
#define TRASHMAP_ASSERT(COND)
#else
// to use a custom assert, define TRASHMAP_ASSERT(cond)
#ifndef TRASHMAP_ASSERT
#include <assert.h>
#define TRASHMAP_ASSERT(COND) assert(COND)
#endif

#endif // TRASHMAP_STRIP_ASSERTS

#ifdef __cplusplus
#define TRASHMAP_LITERAL(TYPE) TYPE
#else 
#define TRASHMAP_LITERAL(TYPE) (TYPE)
#endif // __cplusplus

#endif // TRASHMAP_H

#ifdef TRASHMAP_IMPL

#ifndef TRASHMAP_CUSTOM_HASH_FUNCTION
uint32_t trashmap_hash(const char * key) {
    // implementation of the FNV-1a algorithm
    const unsigned char * str = (const unsigned char *)key;
    #define FNV_1A_OFFSET_BASIS 2166136261
    uint32_t hash = FNV_1A_OFFSET_BASIS;
    while (*str) {
        hash = hash ^ *(str++);
        // equivalent to hash = hash * 16777619
        hash = hash + (hash << 1) + (hash << 4) + (hash << 7) + (hash << 8) + (hash << 24);
    }
    return hash;
}
#endif // TRASHMAP_CUSTOM_HASH_FUNCTION

int trashmap_strcmp(const char * lhs, const char * rhs) {
    for (;*lhs && *rhs; lhs++, rhs++) {
        int diff = *rhs - *lhs;
        if (diff) { return diff; }
    }
    return *rhs - *lhs;
}

void * trashmap_memset(void * dest, unsigned char byte, size_t count) {
    unsigned char * d = (unsigned char *)dest;
    for (size_t i = 0; i < count; i++) {
        d[i] = byte;
    }
    return dest;
}

void trashmap_init(trashmap_t* map, size_t count) {
    TRASHMAP_ASSERT(count && "hash map must have at least 1 slot to start");
    size_t length = count * sizeof(*map->slots);
    map->slots = (trashmap_slot_t*)trashmap_memset(TRASHMAP_ALLOC(length), 0xFF, length);
    TRASHMAP_ASSERT(map->slots && "out of memory");
    map->slot_count = count;
    map->items = NULL;
    map->count = 0;
    map->capacity = 0;
}

void trashmap_deinit(trashmap_t* map) {
    if (map->slots) TRASHMAP_FREE(map->slots);
    if (map->items) TRASHMAP_FREE(map->items);
}

void trashmap_clear(trashmap_t* map) {
    map->count = 0;
    trashmap_memset(map->slots, 0xFF, map->slot_count * sizeof(*map->slots));
}

const char* trashmap_get(const trashmap_t* map, const char * key) {
    uint32_t hash = trashmap_hash(key);
    uint32_t start = hash % map->slot_count;
    uint32_t idx = start;
    do {
        uint32_t bhash = map->slots[idx].hash;
        uint32_t bidx = map->slots[idx].index;
        if (bidx == UINT32_MAX) {
            return NULL;
        }
        if (bhash == hash && trashmap_strcmp(key, map->items[bidx].key) == 0) {
            return map->items[map->slots[idx].index].value;
        }
        idx = (idx + 1) % map->slot_count;
    } while (idx != start);
    return NULL;
}

bool trashmap_has(const trashmap_t* map, const char * key) {
    uint32_t hash = trashmap_hash(key);
    uint32_t start = hash % map->slot_count;
    uint32_t idx = start;
    do {
        uint32_t bhash = map->slots[idx].hash;
        uint32_t bidx = map->slots[idx].index;
        if (bidx == UINT32_MAX) {
            return false;
        }
        if (bhash == hash && trashmap_strcmp(key, map->items[bidx].key) == 0) {
            return true;
        }
        idx = (idx + 1) % map->slot_count;
    } while (idx != start);
    return false;
}


void trashmap_reserve(trashmap_t* map, size_t extra) {
    if (map->count + extra > map->capacity) {
        if (map->capacity == 0) {
            map->capacity = 16;
        }
        while (map->count + extra > map->capacity) {
            map->capacity *= 2;
        }
        map->items = (trashmap_item_t*)TRASHMAP_REALLOC(map->items, map->capacity * sizeof(*map->items));
        TRASHMAP_ASSERT(map->items && "out of memory");
    }
    // ensure load factor is not more than 75%
    if (map->count + extra > map->slot_count * 3 / 4) {

        // expands slots and copies all old slots into new spaces
        size_t new_slot_count = map->slot_count * 2;
        size_t length = new_slot_count * sizeof(trashmap_slot_t);

        trashmap_slot_t* new_slots = (trashmap_slot_t*)trashmap_memset(TRASHMAP_ALLOC(length), 0xFF, length);
        TRASHMAP_ASSERT(new_slots && "out of memory");

        for (size_t map_idx = 0; map_idx < map->slot_count; map_idx++) {
            if (map->slots[map_idx].index == UINT32_MAX) continue;
            uint32_t start = map->slots[map_idx].hash % new_slot_count;
            uint32_t new_idx = start;
            do {
                if (new_slots[new_idx].index == UINT32_MAX) {
                    new_slots[new_idx] = map->slots[map_idx];
                    break;
                }
                new_idx = (new_idx + 1) % new_slot_count;
            } while (new_idx != start);
        }

        TRASHMAP_FREE(map->slots);

        map->slots = new_slots;
        map->slot_count = new_slot_count;
    }
}

void trashmap_set(trashmap_t* map, const char * key, const char * value) {    

    trashmap_reserve(map, 1);

    uint32_t hash = trashmap_hash(key);

    uint32_t start = hash % map->slot_count;
    uint32_t idx = start;

    do {
        uint32_t item_idx = map->slots[idx].index;
        uint32_t item_hash = map->slots[idx].hash;
        if (item_idx == UINT32_MAX) {
            map->items[map->count] = TRASHMAP_LITERAL(trashmap_item_t){.key = key, .value = value};
            map->slots[idx] = TRASHMAP_LITERAL(trashmap_slot_t){.hash = hash, .index = (uint32_t)map->count};
            map->count += 1;
            return;
        } else if (item_hash == hash && trashmap_strcmp(key, map->items[item_idx].key) == 0) {
            map->items[item_idx].value = value;
            return;
        }
        idx = (idx + 1) % map->slot_count;
    } while (idx != start);
    TRASHMAP_ASSERT(0 && "corrupted hash map");
}

#endif // TRASHMAP_IMPL