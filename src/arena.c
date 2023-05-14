

#include "arena.h"
#include "alloc_backend.h"

#include <errno.h>
#include <unistd.h>
#include <string.h>

#define ARENA_SIZE ((size_t)(128*sysconf(_SC_PAGE_SIZE)))

/*
 * Allocates and returns new arena
 */
struct arena* arena_new()
{
    size_t size = ARENA_SIZE;

    unsigned char *p = call_alloc_backend(size);

    if (p == NULL)
        return NULL;

    struct arena a = {
        .begin = p + sizeof(struct arena),
        .next  = p + sizeof(struct arena),
        .cap   = size
    };

    memcpy(p, &a, sizeof a);

    return (struct arena*)p;
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
    a->next += len;

    if (a->next - a->begin >= a->cap) {
        errno = ENOMEM;
        return NULL;
    }

    return p;
}

