
CC := gcc
CFLAGS := -g -O3 -std=c2x -Wall -Wextra -Wpedantic -Werror -Iinclude

BUILD_DIR := build
BIN_DIR := lib
TEST_DIR := test

OBJS := $(BUILD_DIR)/arena.o $(BUILD_DIR)/alloc_backend.o
FPIC_OBJS := $(BUILD_DIR)/fpic/arena.o $(BUILD_DIR)/fpic/alloc_backend.o

static : $(BIN_DIR)/libarena.a

dynamic : $(BIN_DIR)/libarena.so

tests : test/test_arena

$(TEST_DIR)/test_arena : src/test_arena.c $(BIN_DIR)/libarena.a | $(TEST_DIR)
	$(CC) -o $@ $(CFLAGS) $^

$(BIN_DIR)/libarena.a : $(OBJS) | $(BIN_DIR)
	ar cr $@ $^

$(BIN_DIR)/libarena.so : $(FPIC_OBJS) | $(BIN_DIR)
	$(CC) -shared -o $@ $^

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
