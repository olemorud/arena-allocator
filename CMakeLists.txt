cmake_minimum_required(VERSION 3.16)

project("ArenaAllocator"
	VERSION 1.0
	LANGUAGES C
	DESCRIPTION "Arena Allocator in C"
)

add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	message(STATUS "Building tests")
	add_subdirectory(test)
endif()

