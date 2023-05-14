
CC := gcc
CFLAGS := -g -O3 -std=c99 -Wall -Wextra -Wpedantic

OBJS := obj/arena.o obj/alloc_backend.o

all : test/test_arena

test/test_arena : src/test_arena.c $(OBJS) | test
	$(CC) -o $@ $(CFLAGS) $^

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
