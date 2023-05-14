
#include "alloc_backend.h"

#define _GNU_SOURCE
#include <sys/mman.h>

/*
 * Separate function to make switching
 * allocation backend easier
 *
 * must take size_t size as an argument
 * must return NULL on failure
 */
void* call_alloc_backend(size_t size)
{
    void *p = mmap(
            NULL,
            size,
            PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE,
            -1, /* man mmap(2): "[...], some implementations require fd to be
                   -1 if MAP_ANONYMOUS is specified [...]" */
            0
    );

    if (p == MAP_FAILED)
        return NULL;

    return p;
}
