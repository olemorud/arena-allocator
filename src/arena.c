
#include "arena.h"

#include <errno.h> // errno
#include <stdbool.h>
#include <stdio.h> // fprintf
#include <string.h> // strerror
#include <sys/mman.h>
#include <unistd.h>

#define ARENA_ALIGN (sizeof(void *))
#define ARENA_GROW_FACTOR 2UL

#ifndef NDEBUG
#define arena_err(msg) \
    fprintf(stderr, "%s (%s:%d): %s\n", msg, __func__, __LINE__, strerror(errno))
#else
#define arena_err(msg)
#endif

static bool arena_grow(struct arena *a)
{
    int ok = mprotect(
        a->data + a->cap,
        a->cap * ARENA_GROW_FACTOR,
        PROT_READ | PROT_WRITE);

    if (ok == -1) {
        arena_err("mprotect");
        return false;
    }

    a->cap *= ARENA_GROW_FACTOR;

    return true;
}

inline void arena_reset(arena_t *a)
{
    a->size = 0;
}

arena_t arena_new()
{
    size_t size = sysconf(_SC_PAGE_SIZE);
    void  *p    = mmap(NULL, 1UL << 40UL, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    mprotect(p, size, PROT_READ | PROT_WRITE);
    if (p == MAP_FAILED)
        arena_err("mmap");

    return arena_attach(p, size);
}

void *arena_alloc(arena_t *a, size_t size)
{
    size = (size + ARENA_ALIGN - 1) & ~(ARENA_ALIGN - 1); // align

    void *p = a->data + a->size;
    a->size += size;
    if (a->size > a->cap) {
        if (!arena_grow(a))
            return NULL;
    }
    return p;
}

void *arena_calloc(arena_t *a, size_t nmemb, size_t size)
{
    void *p = arena_alloc(a, nmemb * size);
    if (p == NULL)
        return p;
    memset(p, 0, nmemb * size);
    return p;
}

void arena_delete(struct arena *a)
{
    munmap(a->data, a->cap);
    a->cap  = -1;
    a->size = -1;
}
