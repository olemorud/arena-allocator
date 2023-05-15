


// _start test_arena_alloc.c
#include "arena.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static struct arena *default_arena = NULL;

int main()
{
    default_arena = arena_new();

    if (default_arena == NULL) {
        err(errno, "failed to allocate arena");
    }

	printf("\ntest 1: %s", "attempt to allocate arena->cap x 1 byte");

	for (size_t i = 0; i < default_arena->cap; i++) {
		volatile void *c = arena_alloc(default_arena, 1);
		if (c == NULL) {
			err(EXIT_FAILURE, "failed to allocate memory");
		}	
	}

    return 0;
}
