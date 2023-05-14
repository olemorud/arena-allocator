
#include <stddef.h> // ptrdiff_t

struct arena {
    void *start,
         *next;
    ptrdiff_t cap;
} __attribute__((aligned(64)));

struct arena* arena_new();
void arena_reset(struct arena *a);
void* arena_alloc(struct arena *a, size_t len);
