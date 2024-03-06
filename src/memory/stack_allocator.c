#include "stack_allocator.h"
#include <memory.h>
#include <stdlib.h>
#include "memory_utils.h"

stack_allocator *stack_allocator_create(size_t max_capacity)
{
	stack_allocator *allocator = malloc(sizeof(stack_allocator));
	allocator->start_ptr = malloc(max_capacity);
	allocator->total_memory_available = max_capacity;
	allocator->offset = 0;
	return allocator;
}

void stack_allocator_free(stack_allocator *allocator)
{
	if (allocator)
	{
		free(allocator->start_ptr);
		allocator->offset = 0;
		allocator->total_memory_available = 0;
		allocator->start_ptr = NULL;
		free(allocator);
	}
}

void *stack_allocator_allocate(stack_allocator *allocator, size_t size, size_t alignment, stack_marker *out_marker)
{
	if (allocator)
	{
		uint8_t *current_address_ptr = (uint8_t *)allocator->start_ptr + allocator->offset;
		uintptr_t current_address = (uintptr_t)current_address_ptr;
		size_t padding = calculate_padding(current_address, alignment);
		if (allocator->offset + padding + size <= allocator->total_memory_available)
		{
			if (out_marker && (*out_marker) == 0)
			{
				*out_marker = allocator->offset;
				allocator->offset += size + padding;
				return (void *)(current_address_ptr + padding);
			}
		}
	}

	return NULL;
}

void stack_allocator_reset_to(stack_allocator *allocator, stack_marker marker)
{
	if (allocator)
	{
		if (allocator->offset > marker)
		{
			allocator->offset = marker;
		}
	}
}