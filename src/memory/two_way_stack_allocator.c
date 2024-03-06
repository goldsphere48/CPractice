#include "two_way_stack_allocator.h"
#include <memory.h>
#include <stdlib.h>
#include "memory_utils.h"

two_way_stack_allocator *two_way_stack_allocator_create(size_t max_capacity)
{
	two_way_stack_allocator *allocator = malloc(sizeof(two_way_stack_allocator));
	allocator->start_ptr = malloc(max_capacity);
	allocator->total_memory_available = max_capacity;
	allocator->front_offset = 0;
	allocator->back_offset = 0;
	return allocator;
}

void two_way_stack_allocator_free(two_way_stack_allocator *allocator)
{
	if (allocator)
	{
		free(allocator->start_ptr);
		allocator->front_offset = 0;
		allocator->back_offset = 0;
		allocator->total_memory_available = 0;
		allocator->start_ptr = NULL;
		free(allocator);
	}
}

void *two_way_stack_allocator_allocate_front(two_way_stack_allocator *allocator, size_t size, size_t alignment, stack_marker *out_marker)
{
	if (allocator)
	{
		uint8_t *current_address_ptr = (uint8_t *)allocator->start_ptr + allocator->front_offset;
		uintptr_t current_address = (uintptr_t)current_address_ptr;
		size_t padding = calculate_padding(current_address, alignment);
		if (allocator->front_offset + padding + size + allocator->back_offset <= allocator->total_memory_available)
		{
			if (out_marker && (*out_marker) == 0)
			{
				*out_marker = allocator->front_offset;
				allocator->front_offset += size + padding;
				return (void *)(current_address_ptr + padding);
			}
		}
	}

	return NULL;
}

void *two_way_stack_allocator_allocate_back(two_way_stack_allocator *allocator, size_t size, size_t alignment, stack_marker *out_marker)
{
	if (allocator)
	{
		uintptr_t new_offset = allocator->back_offset + size;
		uint8_t *address_before_padding_ptr = (uint8_t *)allocator->start_ptr + allocator->total_memory_available - new_offset;
		uintptr_t address_before_padding = (uintptr_t)address_before_padding_ptr;
		size_t padding = (alignment - calculate_padding(address_before_padding, alignment)) % alignment;
		new_offset += padding;
		if (new_offset + allocator->front_offset <= allocator->total_memory_available)
		{
			if (out_marker && (*out_marker) == 0)
			{
				*out_marker = allocator->back_offset;
				allocator->back_offset = new_offset;
				return address_before_padding_ptr - padding;
			}
		}
	}

	return NULL;
}

void two_way_stack_allocator_reset_front_to(two_way_stack_allocator *allocator, stack_marker marker)
{
	if (allocator)
	{
		if (allocator->front_offset > marker)
		{
			allocator->front_offset = marker;
		}
	}
}

void two_way_stack_allocator_reset_back_to(two_way_stack_allocator *allocator, stack_marker marker)
{
	if (allocator)
	{
		if (allocator->back_offset > marker)
		{
			allocator->back_offset = marker;
		}
	}
}