#include "pool_allocator.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

pool_allocator *pool_allocator_create(size_t max_capacity, size_t stride)
{
	pool_allocator *allocator = malloc(sizeof(pool_allocator));
	size_t element_size = stride + sizeof(uintptr_t);
	allocator->total_memory_available = max_capacity;
	allocator->stride = stride;
	allocator->start_ptr = malloc(max_capacity * element_size);

	memset(allocator->start_ptr, 0, max_capacity * element_size);

	for (int i = 0; i < max_capacity - 1; ++i)
	{
		uintptr_t *element_ptr = (uintptr_t *)((uint8_t *)allocator->start_ptr + i * element_size);
		uintptr_t *next_element_ptr = (uintptr_t *)((uint8_t *)allocator->start_ptr + (i + 1) * element_size);
		*element_ptr = (uintptr_t)next_element_ptr;
	}

	allocator->head = allocator->start_ptr;

	return allocator;
}

void pool_allocator_free(pool_allocator *allocator)
{
	if (allocator)
	{
		free(allocator->start_ptr);
		allocator->start_ptr = NULL;
		allocator->head = NULL;
		allocator->stride = 0;
		allocator->total_memory_available = 0;
		free(allocator);
	}
}

void *pool_allocator_allocate(pool_allocator *allocator)
{
	if (allocator)
	{
		if (allocator->head)
		{
			uintptr_t *element_ptr = (uintptr_t *)allocator->head;
			void *element_data_ptr = (uint8_t *)element_ptr + sizeof(uintptr_t);
			allocator->head = (uintptr_t *)(*element_ptr);
			return element_data_ptr;
		}
	}

	return NULL;
}

void pool_allocator_return(pool_allocator *allocator, void *element)
{
	if (allocator && element)
	{
		uintptr_t *element_ptr = (uintptr_t *)((uint8_t *)element - sizeof(uintptr_t));
		*element_ptr = (uintptr_t)allocator->head;
		allocator->head = (void *)element_ptr;
	}
}