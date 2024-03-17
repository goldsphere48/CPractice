#include "dynamic_allocator.h"
#include "freelist.h"
#include "memory_utils.h"
#include <memory.h>
#include <stdlib.h>

typedef struct dynamic_allocator_state
{
	u64 total_size;
	freelist freelist;
	void* freelist_block;
	void* memory_block;
} dynamic_allocator_state;

typedef struct allocation_header
{
	u64 size;
	u64 offset;
} allocation_header;

b8 dynamic_allocator_create(u64 total_size, u64* memory_requirement, void* memory, dynamic_allocator* out_allocator)
{
	if (!memory_requirement)
	{
		return FALSE;
	}

	size_t freelist_requirements = 0;
	freelist_create(total_size, &freelist_requirements, NULL, NULL);

	*memory_requirement = sizeof(dynamic_allocator_state) + freelist_requirements + total_size;

	if (!memory)
	{
		return TRUE;
	}

	out_allocator->memory = memory;
	dynamic_allocator_state* state = memory;
	state->total_size = total_size;

	state->freelist_block = (u8*)out_allocator->memory + sizeof(dynamic_allocator_state);
	state->memory_block = (u8*)state->freelist_block + freelist_requirements;

	freelist_create(total_size, &freelist_requirements, state->freelist_block, &state->freelist);

	return TRUE;
}

void dynamic_allocator_destroy(dynamic_allocator* allocator)
{
	if (allocator)
	{
		dynamic_allocator_state* state = allocator->memory;
		freelist_destroy(&state->freelist);
		state->total_size = 0;
		memset(allocator->memory, 0, sizeof(dynamic_allocator_state));
		allocator->memory = NULL;
	}
}

void* dynamic_allocator_allocate_aligned(dynamic_allocator* allocator, u64 size, u64 aligment)
{
	if (allocator && size > 0 && aligment > 0)
	{
		dynamic_allocator_state* state = allocator->memory;
		u64 required_size = sizeof(allocation_header) + size + aligment;
		u64 offset = 0;
		if (freelist_allocate_block(&state->freelist, required_size, &offset))
		{
			void* out_memory = (u8*)state->memory_block + offset;
			u64 padding = calculate_padding((uint64_t)out_memory, aligment);
			void* aligned_ptr = (u8*)out_memory + padding;
			allocation_header* header = out_memory;
			header->offset = offset;
			header->size = required_size;
			return (u8*)out_memory + sizeof(allocation_header);
		}
	}

	return NULL;
}

void* dynamic_allocator_allocate(dynamic_allocator* allocator, u64 size)
{
	return dynamic_allocator_allocate_aligned(allocator, size, 1);
}

void dynamic_allocator_free(dynamic_allocator* allocator, void* memory)
{
	if (allocator)
	{
		dynamic_allocator_state* state = allocator->memory;
		allocation_header* header = (u8*)memory - sizeof(allocation_header);
		freelist_free_block(&state->freelist, header->size, header->offset);
	}
}