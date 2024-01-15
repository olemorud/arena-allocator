
#include "arena.h"

#include <errno.h> // errno
#include <stdbool.h>
#include <stdio.h> // fprintf
#include <string.h> // strerror
#include <sys/mman.h>
#include <unistd.h>

#define ARENA_ALIGN (sizeof(void *))
#define ARENA_GROW_FACTOR 2UL
#define ARENA_OVERCOMMIT_SIZE (1UL << 40UL)

#ifndef NDEBUG
#define arena_err(msg) \
    fprintf(stderr, "%s (%s:%d): %s\n", msg, __func__, __LINE__, strerror(errno))
#else
#define arena_err(msg)
#endif

static bool arena_grow(struct arena *a, size_t min_size)
{
    size_t new_cap = a->cap * 2;
    while (new_cap < min_size) {
        new_cap *= 2;
    }

    int ok = mprotect(
        a->data + a->cap,
        new_cap - a->cap,
        PROT_READ | PROT_WRITE
    );

    if (ok == -1) {
        arena_err("mprotect");
        return false;
    }

    a->cap = new_cap;

    return true;
}


arena_t arena_new()
{
    size_t size = sysconf(_SC_PAGE_SIZE);
    if (size == -1) {
        arena_err("sysconf");
        goto sysconf_failed;
    }

    void *p = mmap(NULL, ARENA_OVERCOMMIT_SIZE, PROT_NONE,
            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (p == MAP_FAILED) {
        arena_err("mmap");
        goto mmap_failed;
    }

    int ok = mprotect(p, size, PROT_READ | PROT_WRITE);
    if (ok == -1) {
        arena_err("mprotect");
        goto mprotect_failed;
    }

    return arena_attach(p, size);
    
mprotect_failed:
    munmap(p, ARENA_OVERCOMMIT_SIZE);
mmap_failed:
sysconf_failed:
    return (arena_t) { 0 };
}

void* arena_alloc(arena_t *a, size_t size)
{
    size = (size + ARENA_ALIGN - 1) & ~(ARENA_ALIGN - 1); // align

    void *p = a->data + a->size;
    a->size += size;
    if (a->size > a->cap) {
        if (!arena_grow(a, a->size))
            return NULL;
    }
    return p;
}

void* arena_calloc(arena_t *a, size_t nmemb, size_t size)
{
    void *p = arena_alloc(a, nmemb * size);
    if (p == NULL)
        return p;
    memset(p, 0, nmemb * size);
    return p;
}

int arena_delete(struct arena *a)
{
    int ok = munmap(a->data, a->cap);
    if (ok == -1) {
        arena_err("munmap");
        return -1;
    }
    a->cap  = -1;
    a->size = -1;
    return 0;
}
