#include "linear_allocator.h"
#include <stdlib.h>
#include <memory.h>
#include "memory_utils.h"

linear_allocator *linear_allocator_create(int max_capacity)
{
	linear_allocator *allocator = malloc(sizeof(linear_allocator));
	allocator->start_ptr = malloc(max_capacity);
	allocator->offset = 0;
	allocator->total_memory_available = max_capacity;
	memset(allocator->start_ptr, 0, max_capacity);
	return allocator;
}

void linear_allocator_destroy(linear_allocator *allocator)
{
	if (allocator)
	{
		free(allocator->start_ptr);
		allocator->start_ptr = NULL;
		allocator->offset = 0;
		allocator->total_memory_available = 0;
		free(allocator);
		allocator = NULL;
	}
}

void *linear_allocator_allocate(linear_allocator *allocator, size_t size, size_t aligment)
{
	if (allocator)
	{
		uint8_t *current_address_ptr = (uint8_t *)allocator->start_ptr + allocator->offset;
		uintptr_t current_address = (uintptr_t)current_address_ptr;
		size_t padding = calculate_padding(current_address, aligment);

		if (allocator->offset + size + padding <= allocator->total_memory_available)
		{
			allocator->offset += size + padding;
			return current_address_ptr + padding;
		}
	}

	return NULL;
}

void linear_allocator_reset(linear_allocator *allocator)
{
	allocator->offset = 0;
}
