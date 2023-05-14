
#ifndef ARENA_H
#define ARENA_H


#include <stddef.h> // ptrdiff_t

struct arena {
    unsigned char *begin,
                  *next;
    ptrdiff_t cap;
};

struct arena* arena_new();
void arena_reset(struct arena *a);
void* arena_alloc(struct arena *a, size_t len);


#endif
