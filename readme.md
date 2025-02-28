# Trash Map

Header only library for a string to string style append only hashmap. 
This library is not thread safe. It uses a linear probing style hash map with an internal arena for map items. 
Library works in C from c99 and C++ from c++20 without any warnings from `-Wall -Wextra -Wpedantic`. 
This library was made for my purposes, such as parsing HTTP headers, but it can be used in many contexts.

## Using the library

In exactly ONE file define TRASHMAP_IMPL before including trashmap.h e.g.

``` C
#define TRASHMAP_IMPL
#include "trashmap.h"

// your code here

```

## Example

``` C
trashmap_t map = {};

trashmap_init(&map, 16);

trashmap_set(&map, "hello", "world");

const char * world = trashmap_get(&map, "hello");

printf("hello %s\n", world);
```

## Customization

The default allocator (libc) for trashmap can be overwritten by creating custom defines for:

``` C
#define TRASHMAP_ALLOC(SIZE)
#define TRASHMAP_REALLOC(PTR, SIZE)
#define TRASHMAP_FREE(PTR)
```

All asserts can be removed by defining `TRASHMAP_STRIP_ASSERTS` before including or using `-DTRASHMAP_STRIP_ASSERTS`.
Alternatively, the default assert function can be overwritten by creating a custom define for:

``` C
#define TRASHMAP_ASSERT(COND)
```

To use a custom hash function define `TRASHMAP_CUSTOM_HASH` above the implementing include,
and then implement a function with the same signature as trashmap_hash:

``` C
#define TRASHMAP_CUSTOM_HASH
#define TRASHMAP_IMPL
#include "trashmap.h"

// function must have this exact signature
uint32_t trashmap_hash(const char * key) {
    // custom hash function implementation
    // ...
}

```

## API

trashmap_init: initialize an empty hashmap with `count` initial slots.

``` C
void trashmap_init(trashmap_t* map, size_t count);
```

trashmap_deinit: release all resources associated with hashmap.

``` C
void trashmap_deinit(trashmap_t* map);
```

trashmap_clear: empties the hashmap while retaining all allocated resources so they can be reused.

``` C
void trashmap_deinit(trashmap_t* map);
```

trashmap_hash: implementation of the FNV-1a hashing algorithm.

``` C
uint32_t trashmap_hash(const char * key);
```

trashmap_has: checks if the key appears in the hash map.

``` C
bool trashmap_has(const trashmap_t\* map, const char * key);
```

trashmap_get: gets the associated value for the key, NULL if key does not appear in the hash map.

``` C
const char* trashmap_get(const trashmap_t* map, const char * key);
```

trashmap_set: inserts an element into the hash map, or updates the value if it already exists.
does NOT duplicate strings, ensure all strings are allocated somewhere permanently before passing to trashmap_set.

``` C
void trashmap_set(trashmap_t* map, const char * key, const char * value);
```

trashmap_reserve: reserves enough space for `extra` addition items

``` C
void trashmap_reserve(trashmap_t* map, size_t extra);
```

Reimplementation of the necessary string.h functionality. This removes string.h as a dependency and means that the only dependency is malloc/realloc/free

trashmap_strcmp: reimplementation of libc strcmp

``` C
int trashmap_strcmp(const char * lhs, const char * rhs);
```

trashmap_memset: reimplementation of libc memset

``` C
void * trashmap_memset(void * dest, char byte, size_t count);
```
