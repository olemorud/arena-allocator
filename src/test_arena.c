

// _start test_arena_alloc.c
#include "arena.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static struct arena default_arena = { 0 };

int main()
{
    default_arena = arena_new();

    printf("\nAttempt to do allocations of 1 byte, cap is %zu, ", default_arena.cap);

    size_t i;
    for (i = 0; default_arena.next < default_arena.cap; i++) {
        char* c = arena_alloc(&default_arena, 1);
        *c = i & 0xff;
        if (c == NULL) {
            err(EXIT_FAILURE, "failed to allocate memory");
        }
    }
    printf("did %zu allocations", i);

    printf("\n    OK");

    return 0;
}
