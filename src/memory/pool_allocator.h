#include <stdint.h>

typedef struct
{
	size_t total_memory_available;
	size_t stride;
	void* start_ptr;
	void* head;
} pool_allocator;

pool_allocator* pool_allocator_create(size_t max_capacity, size_t stride);
void pool_allocator_free(pool_allocator* allocator);
void* pool_allocator_allocate(pool_allocator* allocator);
void pool_allocator_return(pool_allocator* allocator, void* element_data_ptr);