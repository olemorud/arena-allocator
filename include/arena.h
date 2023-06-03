
#ifndef ARENA_H
#define ARENA_H

#include <stddef.h> // ptrdiff_t

typedef unsigned char byte_t;

typedef struct arena {
    size_t next, prev, cap;
    void* data;
} __attribute__((aligned(sizeof(void*)))) arena_t;

arena_t arena_new();
void arena_reset(arena_t* a);
void* arena_alloc(arena_t* a, size_t len);
void* arena_calloc(arena_t* a, size_t nmemb, size_t size);
void* arena_realloc_tail(arena_t* a, size_t len);

#endif
