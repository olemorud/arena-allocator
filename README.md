
## `arena_t arena_new()`

Allocate a new arena.
The underlying memory is allocated with mmap.


## `void arena_delete(arena_t *a)`

Delete memory mapped for arena.
Should only be used with arenas from arena\_new().


## `arena_t arena_attach(void *ptr, size_t size)`

Attach an arena to an existing memory region.
The arena will not expand the region if space is exceeded.


## `void *arena_detatch(arena_t arena)`
Detach an arena from an existing memory region.


## `void arena_reset(arena_t *a)`
Reset an arena.



## `void *arena_alloc(arena_t *a, size_t len)`
Allocate memory from an arena.
Returns NULL and sets errno on failure.



## `void *arena_calloc(arena_t *a, size_t nmemb, size_t size)`
Allocate and zero memory from an arena.
Returns NULL and sets errno on failure.

