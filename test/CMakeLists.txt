
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(test_arena test_arena.c)

target_link_libraries(test_arena arena)
