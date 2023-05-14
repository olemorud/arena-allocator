


// _start test_arena_alloc.c
#include "arena.h"
#include <err.h>
#include <stdio.h>
#include <errno.h>

static struct arena *default_arena = NULL;

int main()
{
    default_arena = arena_new();

    if (default_arena == NULL) {
        err(errno, "failed to allocate arena");
    }

	printf("\n%p\n", default_arena->next);

    char *ten_A = arena_alloc(default_arena, 11 * sizeof *ten_A);
    char *ten_B = arena_alloc(default_arena, 11 * sizeof *ten_B);
    char *ten_C = arena_alloc(default_arena, 11 * sizeof *ten_C);

    for (size_t i=0; i<10; ++i) {
        ten_A[i] = 'A';
        ten_B[i] = 'B';
        ten_C[i] = 'C';
    }

    ten_A[10] = '\0';
    ten_B[10] = '\0';
    ten_C[10] = '\0';

    printf("\n%s %s %s", ten_A, ten_B, ten_C);
	printf("\n%p\n", default_arena->next);

	arena_reset(default_arena);

	printf("\n%p\n", default_arena->next);

    return 0;
}
