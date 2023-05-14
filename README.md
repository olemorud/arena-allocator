

`arena_t* arena_new()`

Initializes and returns new arena

`void arena_reset(arena_t *a)`

Free memory allocated in arena

`void* arena_alloc(arena_t *a, size_t len)`

Allocate new memory using arena
