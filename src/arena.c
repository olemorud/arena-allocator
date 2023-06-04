
#include "arena.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// (sizeof(intptr_t) isn't guaranteed to be the machine word size but on most
// compilers it is)
#define WORD_SIZE (sizeof(intptr_t))

#define ARENA_SIZE ((size_t)(1 * sysconf(_SC_PAGE_SIZE)))

arena_t arena_new()
{
    arena_t a = {
        .next = 0,
        .prev = 0,
        .cap = ARENA_SIZE,
        .data = malloc(ARENA_SIZE)
    };

    if (a.data == NULL)
        exit(errno);

    return a;
}

void _arena_realloc_or_panic(arena_t* a, size_t len)
{
    // TODO: gracefully recover.
    //  although unlikely to be recoverable in most use cases
    a->data = realloc(a->data, len);

    if (a->data == NULL)
        exit(errno);

    a->cap = len;
}

void arena_reset(arena_t* a)
{
    a->next = 0;
    a->prev = 0;
}

void* arena_alloc(arena_t* a, size_t len)
{
    // align len to machine word size
    len = (len + WORD_SIZE - 1) & ~(WORD_SIZE - 1);

    a->prev = a->next;
    a->next += len;

    if (a->next > a->cap)
        _arena_realloc_or_panic(a, a->cap * 2);

    return (byte_t*)(a->data) + a->prev;
}

void* arena_calloc(arena_t* a, size_t nmemb, size_t size)
{
    void* p = arena_alloc(a, nmemb * size);
    memset((byte_t*)(a->data) + a->prev, 0, size);
    return p;
}

void* arena_realloc_tail(arena_t* a, size_t len)
{
    a->next = a->prev;

    return arena_alloc(a, len);
}

void arena_delete(arena_t* a)
{
    free(a->data);
    arena_reset(a);
    a->data = NULL;
}

// included for completeness
void arena_free(arena_t* a, void* p)
{
    (void)p;
    (void)a;
    return;
}
