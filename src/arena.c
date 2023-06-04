
#include "arena.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// (sizeof(intptr_t) isn't guaranteed to be the machine word size but on most
// compilers it is)
#define SYS_PAGE_SIZE ((size_t)sysconf(_SC_PAGE_SIZE))

#define BIG_PAGE (SYS_PAGE_SIZE + 1)

typedef unsigned char byte_t;

struct page {
    void* data;
    size_t offset, prev_offset;
    struct page* next;
};

// linked list terminology:
// https://web.archive.org/web/20230604145332/https://i.stack.imgur.com/2FbXf.png

arena_t arena_new(void)
{
    struct page* p = malloc(sizeof *p);

    if (p == NULL)
        exit(errno);

    p->next = NULL;
    p->offset = 0;
    p->prev_offset = 0;
    p->data = malloc(SYS_PAGE_SIZE);

    if (p->data == NULL)
        exit(errno);

    arena_t a = {
        .head = p,
        .last = p,
    };

    return a;
}

void _arena_new_page(arena_t* a, size_t size)
{
    /* potentially reuse page from previously
       reset arena */
    if (a->head->next != NULL) {
        a->head = a->head->next;
        a->head->offset = 0;
        a->head->prev_offset = 0;
        return;
    }

    a->head->next = calloc(1, sizeof *(a->head->next));

    if (a->head->next == NULL)
        exit(errno);

    a->head = a->head->next;
    a->head->data = malloc(size);

    if (a->head->data == NULL)
        exit(errno);
}

void arena_reset(arena_t* a)
{
    a->head = a->last;
    a->head->offset = 0;
    a->head->prev_offset = 0;
}

void* _arena_big_alloc(arena_t* a, size_t size)
{
    _arena_new_page(a, size);
    a->head->offset = BIG_PAGE;
    a->head->prev_offset = BIG_PAGE;

    return a->head->data;
}

void* arena_alloc(arena_t* a, size_t size)
{
    if (size > SYS_PAGE_SIZE)
        return _arena_big_alloc(a, size);

    // align size to machine word size
    size = (size + _WORD_SIZE - 1) & ~(_WORD_SIZE - 1);

    if (a->head->offset > SYS_PAGE_SIZE - size) {
        _arena_new_page(a, SYS_PAGE_SIZE);
        return arena_alloc(a, size);
    }

    a->head->prev_offset = a->head->offset;
    a->head->offset += size;

    return (byte_t*)(a->head->data) + a->head->prev_offset;
}

void* arena_calloc(arena_t* a, size_t nmemb, size_t size)
{
    void* p = arena_alloc(a, nmemb * size);
    memset(p, 0, nmemb * size);
    return p;
}

void* arena_realloc_tail(arena_t* a, size_t len)
{
    if (a->head->offset == BIG_PAGE) {
        a->head->data = realloc(a->head->data, len);

        if (a->head->data == NULL)
            exit(errno);

        return a->head->data;
    }

    a->head->offset = a->head->prev_offset;

    return arena_alloc(a, len);
}

void arena_delete(arena_t* a)
{
    struct page* p = a->last;

    while (p != NULL) {
        struct page* next = p->next;
        free(p->data);
        free(p);
        p = next;
    }
}

// included for completeness
void arena_free(arena_t* a, void* p)
{
    (void)p;
    (void)a;
    return;
}
