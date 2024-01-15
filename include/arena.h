
#ifndef ARENA_H
#define ARENA_H

#include <stdbool.h>
#include <stddef.h> // size_t

typedef struct arena {
    char  *data;
    size_t size;
    size_t cap;
    bool   grow;
} arena_t;

/**
 * Allocate a new arena.
 * The underlying memory is allocated with mmap.
 * Errors can be checked with `arena_new_failed()`
 */
arena_t arena_new();

/**
 * Delete memory mapped for arena.
 * Should only be used with arenas from arena_new().
 * Returns 0 on success, -1 on failure
 */
int arena_delete(arena_t *a);

/**
 * Attach an arena to an existing memory region.
 * The arena will not expand the region if space is exceeded.
 */
static inline arena_t arena_attach(void *ptr, size_t size)
{
    return (arena_t) { .data = ptr, .size = 0, .cap = size, .grow = false };
}

/**
 * Detach an arena from an existing memory region.
 */
static inline void *arena_detatch(arena_t arena)
{
    return arena.data;
}

/**
 * Returns true if creating new arena failed
 */
static inline bool arena_new_failed(arena_t *a)
{
    return a->data == NULL;
}

/**
 * Reset an arena.
 */
static inline void arena_reset(arena_t *a)
{
    a->size = 0;
}

/**
 * Allocate memory from an arena.
 * Returns NULL and sets errno on failure.
 */
void *arena_alloc(arena_t *a, size_t len);

/**
 * Allocate and zero memory from an arena.
 * Returns NULL and sets errno on failure.
 */
void *arena_calloc(arena_t *a, size_t nmemb, size_t size);

#endif
