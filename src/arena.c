
#include "arena.h"
#include "knob.h"

#include <errno.h> // errno
#include <stdbool.h>
#include <stdio.h> // fprintf
#include <string.h> // strerror
#include <sys/mman.h>
#include <unistd.h>


#ifndef NDEBUG
#define arena_err(msg) \
    fprintf(stderr, "%s (%s:%d): %s\n", msg, __func__, __LINE__, strerror(errno))
#else
#define arena_err(msg)
#endif

static bool arena_grow(struct arena *a, size_t min_size)
{
    if (!(a->flags & ARENA_GROW)) {
        return false;
    }

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

    void *p = mmap(NULL, KNOB_MMAP_SIZE, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (p == MAP_FAILED) {
        arena_err("mmap");
        goto mmap_failed;
    }

    int ok = mprotect(p, size, PROT_READ | PROT_WRITE);
    if (ok == -1) {
        arena_err("mprotect");
        goto mprotect_failed;
    }

    struct arena a = {
        .data = p,
        .size = 0,
        .cap = size,
        .flags = ARENA_GROW,
    };

    return a;

mprotect_failed:
    ok = munmap(p, KNOB_MMAP_SIZE);
    if (ok == -1) {
        arena_err("munmap");
    }
mmap_failed:
sysconf_failed:
    return (arena_t) { 0 };
}

void* arena_alloc(arena_t *a, size_t size)
{
    // align
    if (!(a->flags & ARENA_DONTALIGN)) {
        size = (size + KNOB_ALIGNMENT - 1) & ~(KNOB_ALIGNMENT - 1);
    }

    void *p = a->data + a->size;
    if (a->size + size > a->cap) {
        if (!arena_grow(a, a->size))
            return NULL;
    }
    a->size += size;
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
    if (!(a->flags & ARENA_GROW)) {
        return -1;
    }
    int ok = munmap(a->data, a->cap);
    if (ok == -1) {
        arena_err("munmap");
        return -1;
    }
    a->cap  = -1;
    a->size = -1;
    return 0;
}
