#include <stdint.h>

typedef uintptr_t stack_marker;

typedef struct
{
	stack_marker offset;
	int total_memory_available;
	void *start_ptr;
} stack_allocator;

stack_allocator *stack_allocator_create(size_t size);
void stack_allocator_free(stack_allocator *allocator);
void *stack_allocator_allocate(stack_allocator *allocator, size_t size, size_t alignment, stack_marker *out_marker);
void stack_allocator_reset_to(stack_allocator *allocator, stack_marker marker);