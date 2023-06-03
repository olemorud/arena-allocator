
#include "arena.h"
#include "alloc_backend.h"

#include <errno.h>
#include <unistd.h>

#define ARENA_SIZE ((size_t)(128 * sysconf(_SC_PAGE_SIZE)))

typedef unsigned char byte;

/*
 * Allocates and returns new arena
 */
struct arena* arena_new()
{
    size_t size = ARENA_SIZE;

    byte* p = call_alloc_backend(size);

    if (p == NULL)
        return NULL;

    arena_t* a = (arena_t*)p;

    void* beg = p + sizeof(struct arena);
    a->begin = beg;
    a->next = beg;
    a->prev = beg;
    a->cap = size - sizeof(struct arena);

    return a;
}

/*
 * Frees all memory in arena
 */
void arena_reset(struct arena* a)
{
    a->next = a->begin;
    a->prev = a->begin;
}

/*
 * Allocate new memory using arena
 */
void* arena_alloc(struct arena* a, size_t len)
{
    void* p = a->next;

    if ((byte*)p + len > (byte*)(a->begin) + a->cap) {
        errno = ENOMEM;
        return NULL;
    }

    a->next = (byte*)(a->next) + len;
    a->prev = p;

    return p;
}

/*
 * Reallocate last block in arena
 */
void* arena_realloc_tail(struct arena* a, size_t len)
{
    void* p = a->prev;

    if ((byte*)p + len > (byte*)(a->begin) + a->cap) {
        errno = ENOMEM;
        return NULL;
    }

    a->next = (byte*)p + len;

    return a->prev;
}
