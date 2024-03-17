#include "memory_system.h"
#include "dynamic_allocator.h"
#include <memory.h>
#include <stdlib.h>

typedef struct memory_system_state
{
	dynamic_allocator allocator;
	void* memory;
} memory_system_state;

static memory_system_state memory_system;

void memory_system_initialize(u64 total_size)
{
	u64 memory_requirement = 0;
	dynamic_allocator_create(total_size, &memory_requirement, NULL, NULL);

	memory_system.memory = malloc(memory_requirement);
	memset(memory_system.memory, 0, memory_requirement);
	dynamic_allocator_create(total_size, &memory_requirement, memory_system.memory, &memory_system.allocator);
}

void memory_system_shutdown()
{
	dynamic_allocator_destroy(&memory_system.allocator);
	free(memory_system.memory);
	memory_system.memory = NULL;
}

void* allocate_memory(u64 size)
{
	return allocate_memory_aligned(size, 1);
}

void* allocate_memory_aligned(u64 size, u64 aligment)
{
	return dynamic_allocator_allocate_aligned(&memory_system.allocator, size, aligment);
}

void free_memory(void* memory)
{
	dynamic_allocator_free(&memory_system.allocator, memory);
}