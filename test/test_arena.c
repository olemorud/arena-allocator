
#include "arena.h"
#include "knob.h"

#include <signal.h>
#include <stdio.h>  // fprintf
#include <stdlib.h> // exit, EXIT_FAILURE
#include <string.h> // memset
#include <sys/wait.h> // waitpid
#include <unistd.h> // sysconf

int main()
{
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
        fprintf(stderr, "creating new arena with arena_new()\n    ");
        arena_t a = arena_new();
        if (arena_new_failed(&a)) {
            fprintf(stderr, "arena_new failed\n");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "OK\n");

        fprintf(stderr, "attempting many small allocations\n    ");
        for (int i = 0; i < page_size * 8; i++) {
            char* s = arena_alloc(&a, 4 * sizeof *s);
            if (!s) {
                fprintf(stderr, "arena_alloc failed\n");
                exit(EXIT_FAILURE);
            }
            memset(s, 'a', 4);
        }
        fprintf(stderr, "OK\n");

        fprintf(stderr, "testing allocations of cap * 3 + 123\n    ");
        for (int i = 0; i < 2; i++) {
            size_t n = a.cap * 3 + 123;
            volatile char* s = arena_alloc(&a, n);
            if (!s) {
                fprintf(stderr, "arena_alloc failed\n");
                exit(EXIT_FAILURE);
            }
        }
        fprintf(stderr, "OK\n");

        fprintf(stderr, "calling arena_delete() on arena from arena_new()\n    ");
        int ok = arena_delete(&a);
        if (ok == -1) {
            fprintf(stderr, "arena_delete failed\n");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "OK\n");
    }

    /*
     * test arena made by arena_attach();
     * ===============================
     */
    {
        fprintf(stderr, "creating new arena with arena_attach() and malloc()\n    ");
        char* p = malloc(page_size);
        if (!p) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        arena_t a = arena_attach(p, page_size);
        fprintf(stderr, "OK\n");

        bool failed = false;

        fprintf(stderr, "try to grow more than the buffer size (should fail)\n    ");
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
            fprintf(stderr, "allocation failed after %d allocations, but should fail after %d!\n", i, page_size/sizeof(void*));
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "OK\n");

        fprintf(stderr, "deleting an arena not made with arena_new (should fail)\n    ");
        int ok = arena_delete(&a);
        if (ok != -1) {
            fprintf(stderr, "arena_delete was supposed to fail, but didn't\n");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "OK\n");

        free(arena_detatch(a));
    }

    /*
     * test the memory protection
     * ===============================
     */
    {
        fprintf(stderr, "attempting to access memory beyond arena cap (should sigsegv)\n    ");
        pid_t p;
        switch (p = fork()) {
        case -1: /* error */
            perror(fork());
            exit(EXIT_FAILURE);

        case 0: /* child */
            arena_t a = arena_new();
            if (arena_new_failed(&a)) {
                fprintf(stderr, "arena_new failed\n");
                exit(EXIT_FAILURE);
            }

            char* s = arena_alloc(&a, 12345);
            if (!s) {
                fprintf(stderr, "arena_alloc failed\n");
                exit(EXIT_FAILURE);
            }

            // should sigsegv
            a.data[a.cap] = 1;
            exit(EXIT_SUCCESS);
        }

        int status;
        int ok = waitpid(p, &status, 0);
        if (!ok) {
            perror("waitpid");
        }

        if (!( WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV)) {
            fprintf(stderr, "allocating beyond arena cap succeeded when it shouldn't!\n");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "OK\n");
    }

    return EXIT_SUCCESS;
}
