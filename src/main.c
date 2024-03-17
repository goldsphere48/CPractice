#include "containers/darray.h"
#include "memory/linear_allocator.h"
#include "memory/memory_system.h"
#include "memory/pool_allocator.h"
#include "memory/stack_allocator.h"
#include "memory/two_way_stack_allocator.h"
#include <stdio.h>

int main()
{
	{
		linear_allocator* allocator = linear_allocator_create(MB(1));

		void* ptr = linear_allocator_allocate(allocator, 1000, 64);
		void* ptr2 = linear_allocator_allocate(allocator, 1000, 64);
		linear_allocator_reset(allocator);
		linear_allocator_destroy(allocator);
	}
	{
		stack_allocator* allocator = stack_allocator_create(MB(1));

		stack_marker first_marker = 0;
		void* ptr = stack_allocator_allocate(allocator, 100, 1, &first_marker);

		stack_marker second_marker = 0;
		void* ptr2 = stack_allocator_allocate(allocator, 100, 1, &second_marker);

		stack_allocator_reset_to(allocator, second_marker);
		stack_allocator_reset_to(allocator, first_marker);

		stack_allocator_free(allocator);
	}
	{
		two_way_stack_allocator* allocator = two_way_stack_allocator_create(MB(1));

		stack_marker first_marker = 0;
		void* ptr1 = two_way_stack_allocator_allocate_front(allocator, 100, 1, &first_marker);

		stack_marker second_marker = 0;
		void* ptr2 = two_way_stack_allocator_allocate_front(allocator, 100, 1, &second_marker);

		two_way_stack_allocator_reset_front_to(allocator, second_marker);
		two_way_stack_allocator_reset_front_to(allocator, first_marker);

		first_marker = 0;
		void* ptr3 = two_way_stack_allocator_allocate_back(allocator, 100, 1, &first_marker);

		second_marker = 0;
		void* ptr4 = two_way_stack_allocator_allocate_back(allocator, 100, 1, &second_marker);

		two_way_stack_allocator_reset_back_to(allocator, second_marker);
		two_way_stack_allocator_reset_back_to(allocator, first_marker);
	}
	{
		pool_allocator* allocator = pool_allocator_create(3, sizeof(int));
		int* e1 = (int*)pool_allocator_allocate(allocator);
		*e1 = 1;
		int* e2 = (int*)pool_allocator_allocate(allocator);
		*e2 = 1;
		int* e3 = (int*)pool_allocator_allocate(allocator);
		*e3 = 1;
		pool_allocator_return(allocator, e2);
		pool_allocator_return(allocator, e1);
		int* e4 = (int*)pool_allocator_allocate(allocator);
		*e4 = 2;

		pool_allocator_free(allocator);
	}
	{
		memory_system_initialize(MB(512));

		int* array = allocate_memory(sizeof(int) * 1024);

		for (int i = 0; i < 1024; i++)
		{
			array[i] = i;
		}

		free_memory(array);

		memory_system_shutdown();
	}
	return 0;
}