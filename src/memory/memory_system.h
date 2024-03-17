#ifndef MEMORY_SYSTEM_H
#define MEMORY_SYSTEM_H

#include "defines.h"

void memory_system_initialize(u64 total_size);
void memory_system_shutdown();

void* allocate_memory(u64 size);
void* allocate_memory_aligned(u64 size, u64 aligment);
void free_memory(void* memory);

#endif