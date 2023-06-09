
all:

BUILD ?= debug
COMPILER ?= gcc

# ==== set compiler flags ====
# credits Maxim Egorushkin:
# https://stackoverflow.com/questions/48791883/best-practice-for-building-a-make-file/48793058#48793058

CC := $(COMPILER)

# -fsanitize={address,undefined}
CFLAGS.gcc.debug := -Og -ggdb -pg -fsanitize=address -fno-omit-frame-pointer
CFLAGS.gcc.release := -O3 -g -march=native -DNDEBUG
CFLAGS.gcc := -fanalyzer

CFLAGS.clang.debug :=-O0 -g3 -DBACKTRACE -rdynamic
CFLAGS.clang.release :=-O3 -g -march=native -DNDEBUG
CFLAGS.clang :=

CFLAGS := ${CFLAGS.${COMPILER}} ${CFLAGS.${COMPILER}.${BUILD}} \
		  -Iinclude \
		  -std=c2x \
		  -Wall -Wextra -Wpedantic -Werror

BUILD_DIR := build
BIN_DIR := lib
TEST_DIR := test

OBJS := $(BUILD_DIR)/arena.o
FPIC_OBJS := $(BUILD_DIR)/fpic/arena.o

all : static

static : $(BIN_DIR)/libarena.a

dynamic : $(BIN_DIR)/libarena.so

tests : test/test_arena

$(TEST_DIR)/test_arena : src/test_arena.c $(BIN_DIR)/libarena.a | $(TEST_DIR)
	$(CC) -o $@ $(CFLAGS) $^

$(BIN_DIR)/libarena.a : $(OBJS) | $(BIN_DIR)
	ar cr $@ $^

$(BIN_DIR)/libarena.so : $(FPIC_OBJS) | $(BIN_DIR)
	$(CC) -o $@ -shared $(CFLAGS) $^

$(BUILD_DIR)/%.o : src/%.c | $(BUILD_DIR)
	$(CC) -o $@ -c $(CFLAGS) $<

$(BUILD_DIR)/fpic/%.o : src/%.c  | $(BUILD_DIR)/fpic
	$(CC) -o $@ -c -fPIC $(CFLAGS) $<

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/fpic:
	mkdir -p $@

$(BIN_DIR) :
	mkdir -p $@

$(TEST_DIR) :
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(TEST_DIR)

.PHONY: clean $(BUILD_DIR) test bin all
