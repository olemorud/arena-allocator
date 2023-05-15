
#include "arena.h"
#include "alloc_backend.h"

#include <errno.h>
#include <unistd.h>

#define ARENA_SIZE ((size_t)(128*sysconf(_SC_PAGE_SIZE)))

typedef unsigned char byte;

/*
 * Allocates and returns new arena
 */
struct arena* arena_new()
{
    size_t size = ARENA_SIZE;

    byte *p = call_alloc_backend(size);

    if (p == NULL)
        return NULL;

    arena_t *a = (arena_t*)p;

    *a = (arena_t){
        .begin = p + sizeof(struct arena),
        .next  = p + sizeof(struct arena),
        .cap   = size - sizeof(struct arena)
    };

    return (arena_t*)a;
}


/*
 * Frees all memory in arena
 */
void arena_reset(struct arena *a)
{
    a->next = a->begin;
}


/*
 * Allocate new memory using arena
 */
void* arena_alloc(struct arena *a, size_t len)
{
    void *p = a->next;
    a->next = (byte*)(a->next) + len;

    if ((byte*)(a->next) > (byte*)(a->begin) + a->cap) {
        errno = ENOMEM;
        return NULL;
    }

    return p;
}

