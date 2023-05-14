


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

    char *p = arena_alloc(default_arena, sizeof p * 11);

    for (size_t i=0; i<10; ++i) {
        p[i] = 'A';
    }

    p[10] = '\0';

    printf("%s", p);

    return 0;
}
