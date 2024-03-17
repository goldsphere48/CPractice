#ifndef DYNAMIC_ALLOCATOR_H
#define DYNAMIC_ALLOCATOR_H
#include "defines.h"
#include <stdint.h>

typedef struct dynamic_allocator
{
	void* memory;
} dynamic_allocator;

b8 dynamic_allocator_create(u64 total_size, u64* memory_requirement, void* memory, dynamic_allocator* out_allocator);
void dynamic_allocator_destroy(dynamic_allocator* allocator);
void* dynamic_allocator_allocate_aligned(dynamic_allocator* allocator, u64 size, u64 aligment);
void* dynamic_allocator_allocate(dynamic_allocator* allocator, u64 size);
void dynamic_allocator_free(dynamic_allocator* allocator, void* memeory);

#endif