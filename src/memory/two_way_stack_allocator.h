#include <stdint.h>

typedef uintptr_t stack_marker;

typedef struct
{
	stack_marker front_offset;
	stack_marker back_offset;
	int total_memory_available;
	void *start_ptr;
} two_way_stack_allocator;

two_way_stack_allocator *two_way_stack_allocator_create(size_t size);
void two_way_stack_allocator_free(two_way_stack_allocator *allocator);
void *two_way_stack_allocator_allocate_front(two_way_stack_allocator *allocator, size_t size, size_t alignment, stack_marker *out_marker);
void *two_way_stack_allocator_allocate_back(two_way_stack_allocator *allocator, size_t size, size_t alignment, stack_marker *out_marker);
void two_way_stack_allocator_reset_front_to(two_way_stack_allocator *allocator, stack_marker marker);
void two_way_stack_allocator_reset_back_to(two_way_stack_allocator *allocator, stack_marker marker);