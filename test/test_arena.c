
#include "arena.h"
#include "knob.h"

#include <stdlib.h> // exit(), EXIT_FAILURE
#include <string.h> // memset
#include <stdio.h>  // fprintf
#include <unistd.h> // sysconf



int main()
{
    // calling print functions early makes my strace output more readable
    printf("\n");
    fprintf(stderr, "\n");

    size_t page_size = sysconf(_SC_PAGE_SIZE);
    if (page_size == -1) {
        perror("sysconf");
        exit(EXIT_FAILURE);
    }

    /*
     * test arena from arena_new();
     * ===============================
     */
    {
        write(STDERR_FILENO, "\n\n", 2);
        arena_t a = arena_new();
        if (arena_new_failed(&a)) {
            fprintf(stderr, "arena_new failed\n");
            exit(EXIT_FAILURE);
        }

        // test many small allocations
        for (int i = 0; i < page_size * 8; i++) {
            char* s = arena_alloc(&a, 4 * sizeof *s);
            if (!s) {
                fprintf(stderr, "arena_alloc failed\n");
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
                fprintf(stderr, "arena_alloc failed\n");
                exit(EXIT_FAILURE);
            }
        }

        int ok = arena_delete(&a);
        if (ok == -1) {
            fprintf(stderr, "arena_delete failed\n");
            exit(EXIT_FAILURE);
        }
    }

    /*
     * test arena made by arena_attach();
     * ===============================
     */
    {
        write(STDERR_FILENO, "\n\n", 2);
        char* p = malloc(page_size);
        arena_t a = arena_attach(p, page_size);

        bool failed = false;

        // attempt to grow more than the buffer size
        int i;
        for (i=0; i < page_size + 1; i++) {
            char* s = arena_alloc(&a, 8);
            if (!s) {
                failed = true;
                break;
            }
        }

        if (!failed) {
            fprintf(stderr, "allocation was supposed to fail, but didn't\n");
            exit(EXIT_FAILURE);
        } else if (i != page_size / KNOB_ALIGNMENT ) {
            fprintf(stderr, "allocation failed after %d allocations, should instead fail after %d. Bad!\n", i, page_size/sizeof(void*));
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "arena from arena_attach failed after %d allocations, good!", i);
        }

        // deleting an arena not made with arena_new should fail
        int ok = arena_delete(&a);
        if (ok != -1) {
            fprintf(stderr, "arena_delete was supposed to fail, but didn't\n");
            exit(EXIT_FAILURE);
        }

        free(arena_detatch(a));
    }

    
    return EXIT_SUCCESS;
}
