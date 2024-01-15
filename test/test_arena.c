
#include "arena.h"
#include <stdlib.h> // exit(), EXIT_FAILURE
#include <string.h> // memset
#include <stdio.h>  // fprintf
#include <unistd.h> // sysconf

int main()
{

    printf(".");

    arena_t a = arena_new();
    if (arena_new_failed(&a)) {
        fprintf(stderr, "\narena_new failed");
        exit(EXIT_FAILURE);
    }

    size_t size = sysconf(_SC_PAGE_SIZE);
    if (size == -1) {
        perror("sysconf");
        exit(EXIT_FAILURE);
    }

    // test many small allocations
    for (int i = 0; i < size * 8; i++) {
        char* s = arena_alloc(&a, 4 * sizeof *s);
        if (!s) {
            fprintf(stderr, "\narena_alloc failed");
            exit(EXIT_FAILURE);
        }
        memset(s, 'a', 4);
    }

    // test a few allocations above the cap*2
    for (int i = 0; i < 2; i++) {
        size_t n = a.cap * 3 + 123;
        printf("allocating %d bytes\n", n);
        volatile char* s = arena_alloc(&a, n);
        if (!s) {
            fprintf(stderr, "\narena_alloc failed");
            exit(EXIT_FAILURE);
        }
    }

    int ok = arena_delete(&a);
    if (ok == -1) {
        fprintf(stderr, "arena_delete failed");
        exit(EXIT_FAILURE);
    }
    
    return EXIT_SUCCESS;
}
