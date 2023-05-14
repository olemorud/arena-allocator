

#include "arena.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h> // ptrdiff_t
#include <sys/mman.h>

#define ARENA_PAGES ((size_t)(128))

#define ALIGN 1

struct arena* arena_new()
{
    size_t page_size = sysconf(_SC_PAGE_SIZE);
    size_t size = page_size * ARENA_PAGES;

    void *p = mmap(
            NULL,
            size,
            PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE,
            -1, /* man mmap(2): "[...], some implementations require fd to be
                   -1 if MAP_ANONYMOUS is specified [...]" */
            0
    );

    if (p == MAP_FAILED) {
        return NULL;
    }

    struct arena a = {
        .start = p + sizeof(struct arena),
        .next  = p + sizeof(struct arena),
        .cap   = size
    };

    memcpy(p, &a, sizeof a);

    return p;
}


void arena_reset(struct arena *a)
{
    a->next = a->start;
}


void* arena_alloc(struct arena *a, size_t len)
{
    void *p = a->next;
    a->next += len;

    if (a->next - a->start >= a->cap) {
        errno = ENOMEM;
        return NULL;
    }

    return p;
}

