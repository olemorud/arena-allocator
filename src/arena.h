
#ifndef ARENA_H
#define ARENA_H

#include <stddef.h> // ptrdiff_t

typedef struct arena {
    void *begin,
         *next;
    size_t cap;
} __attribute__((aligned(sizeof(void*)))) arena_t;

arena_t* arena_new();
void arena_reset(arena_t *a);
void* arena_alloc(arena_t *a, size_t len);


#endif
