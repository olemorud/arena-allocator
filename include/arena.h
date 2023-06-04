
#ifndef ARENA_H
#define ARENA_H

#include <stddef.h> // ptrdiff_t
#include <stdint.h> // uintptr_t

#define _WORD_SIZE (sizeof(intptr_t))

// linked list terminology:
// https://web.archive.org/web/20230604145332/https://i.stack.imgur.com/2FbXf.png
typedef struct arena {
    struct page *head, *last;
} __attribute__((aligned(_WORD_SIZE))) arena_t;

arena_t arena_new(void);
void arena_reset(arena_t* a);
void* arena_alloc(arena_t* a, size_t len);
void* arena_calloc(arena_t* a, size_t nmemb, size_t size);
void* arena_realloc_tail(arena_t* a, size_t len);
void arena_delete(arena_t* a);
void arena_free(arena_t* a, void* p);

#endif
