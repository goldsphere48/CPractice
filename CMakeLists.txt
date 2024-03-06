cmake_minimum_required(VERSION 3.25)
project(DArray VERSION 0.1.0 LANGUAGES C)

add_executable(DArray 
	src/main.c 
	src/memory/memory_utils.c
	src/containers/darray.c
	src/memory/linear_allocator.c
	src/memory/stack_allocator.c
	src/memory/two_way_stack_allocator.c
	src/memory/pool_allocator.c
)