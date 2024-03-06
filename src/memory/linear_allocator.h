#define KB(count) (count * 1024)
#define MB(count) (count * 1024 * 1024)
#define GB(count) (count * 1024 * 1024 * 1024);

typedef struct
{
	int offset;
	int total_memory_available;
	void *start_ptr;
} linear_allocator;

linear_allocator *linear_allocator_create(int max_capacity);
void linear_allocator_destroy(linear_allocator *allocator);
void *linear_allocator_allocate(linear_allocator *allocator, size_t size, size_t aligment);
void linear_allocator_reset(linear_allocator *allocator);
