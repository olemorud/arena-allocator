
add_executable(arena_allocator_test test_arena.c ../src/arena.c)

target_include_directories(arena_allocator_test PUBLIC ../include)
