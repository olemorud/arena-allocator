
CC := gcc
CFLAGS := -g -O3 -std=c2x -Wall -Wextra -Wpedantic -Werror

OBJS := obj/arena.o obj/alloc_backend.o

all : test/test_arena

test/test_arena : src/test_arena.c obj/libarena.a | test
	$(CC) -o $@ $(CFLAGS) $^

obj/libarena.a : obj/arena.o obj/alloc_backend.o | obj
	ar cr $@ $^

obj/%.o : src/%.c | obj
	$(CC) -o $@ -c $(CFLAGS) $<

obj:
	mkdir -p $@

bin:
	mkdir -p $@

test:
	mkdir -p $@

clean:
	rm -rf obj bin test

.PHONY: clean obj test bin all
