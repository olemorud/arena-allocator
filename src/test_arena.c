

// _start test_arena_alloc.c
#include "arena.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    arena_t default_arena = arena_new();

    printf("\nAttempting to allocate and write to 1 byte 1024 times");
    for (size_t i = 0; i < 1024; i++) {
        char* c = arena_alloc(&default_arena, sizeof *c);

        *c = i & 0xFF;

        if (c == NULL)
            err(EXIT_FAILURE, "failed to allocate memory");
    }
    printf("\n    OK!\n");

    printf("\nAttempting to allocate and write to `_SC_PAGESIZE+1` bytes");
    size_t psz = sysconf(_SC_PAGESIZE) + 1;
    char* c = arena_alloc(&default_arena, psz);

    if (c == NULL)
        err(EXIT_FAILURE, "failed to allocate memory");

    for (size_t i = 0; i < psz; i++)
        *c = i & 0xFF;

    printf("\n    OK!\n");

    return 0;
}
