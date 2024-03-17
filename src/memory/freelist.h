#ifndef FREELIST_H
#define FREELIST_H

#include "defines.h"
#include <stdint.h>

typedef struct freelist
{
	void* memory;
} freelist;

/**
 * @param total_size total memory that should be accessable for user
 * @param memory_requirement size of memory for internal structues using
 * @param memory pointer to start of memory for freelist needs
 * @param out_freelist pointer to start of freelist structure
 * @return success of creating freelist
 */
b8 freelist_create(u64 total_size, u64* memory_requirement, void* memory, freelist* out_freelist);
b8 freelist_allocate_block(freelist* list, u64 size, u64* out_offset);
b8 freelist_free_block(freelist* list, u64 size, u64 offset);
b8 freelist_resize(freelist* list, u64 new_total_size, u64* memory_requirement, void* new_memory);
u64 freelist_get_free_space(freelist* list);
void freelist_clear(freelist* list);
void freelist_destroy(freelist* list);

#endif