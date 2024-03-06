#include "darray.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

static darray_header *_darray_get_header(void *darray)
{
	return (darray_header *)((char *)darray - sizeof(darray_header));
}

static darray_header *_darray_allocate(int stride, int capacity)
{
	int total_size = capacity * stride + sizeof(darray_header);
	void *ptr = malloc(total_size);
	memset(ptr, 0, total_size);
	darray_header *header_ptr = (darray_header *)ptr;
	return header_ptr;
}

static void _darray_set_length(void *darray, int length)
{
	if (darray)
	{
		darray_header *header_ptr = _darray_get_header(darray);
		header_ptr->length = length;
	}
}

void *darray_create(int length, int stride)
{
	darray_header *header_ptr = _darray_allocate(stride, length);
	header_ptr->capacity = length;
	header_ptr->length = length;
	header_ptr->stride = stride;
	return (void *)((char *)header_ptr + sizeof(darray_header));
}

void darray_free(void *darray)
{
	if (darray)
	{
		darray_header *header_ptr = _darray_get_header(darray);
		header_ptr->capacity = 0;
		header_ptr->length = 0;
		header_ptr->stride = 0;
		free(header_ptr);
	}
}

void *darray_resize(void *darray)
{
	if (darray)
	{
		darray_header *header_ptr = _darray_get_header(darray);
		int new_capacity = header_ptr->capacity * DARRAY_RESIZE_FACTOR;
		darray_header *new_darray_header_ptr = _darray_allocate(header_ptr->stride, new_capacity);
		void *new_array = (void *)((char *)new_darray_header_ptr + sizeof(darray_header));
		memcpy(new_array, darray, header_ptr->length * header_ptr->stride);
		new_darray_header_ptr->capacity = new_capacity;
		new_darray_header_ptr->length = header_ptr->length;
		new_darray_header_ptr->stride = header_ptr->stride;
		darray_free(darray);
		return new_array;
	}
}

void *darray_push_back(void *darray, const void *value_ptr)
{
	if (darray)
	{
		darray_header *header_ptr = _darray_get_header(darray);
		if (header_ptr->length + 1 > header_ptr->capacity)
			darray = (darray);

		void *new_element_position_ptr = (void *)((char *)darray + header_ptr->length * header_ptr->stride);
		memcpy(new_element_position_ptr, value_ptr, header_ptr->stride);
		_darray_set_length(darray, header_ptr->length + 1);
		return darray;
	}

	return NULL;
}

void darray_pop_back(void *darray, void *dest)
{
	if (darray && dest)
	{
		darray_header *header_ptr = _darray_get_header(darray);
		if (header_ptr->length > 0)
		{
			void *element_position = (void *)((char *)darray + (header_ptr->length - 1) * header_ptr->stride);
			memcpy(dest, element_position, header_ptr->stride);
			_darray_set_length(darray, header_ptr->length - 1);
		}
	}
}

void darray_set_at(void *darray, int index, const void *value_ptr)
{
	if (darray)
	{
		darray_header *header_ptr = _darray_get_header(darray);
		if (header_ptr->length > index && index >= 0)
		{
			void *element_position_ptr = (void *)((char *)darray + header_ptr->stride * index);
			memcpy(element_position_ptr, value_ptr, header_ptr->stride);
		}
	}
}

void *darray_get_at(void *darray, int index)
{
	if (darray)
	{
		darray_header *header_ptr = _darray_get_header(darray);
		if (header_ptr->length > index && index >= 0)
		{
			void *element_position_ptr = (void *)((char *)darray + header_ptr->stride * index);
			return element_position_ptr;
		}
	}

	return NULL;
}

int darray_length(void *darray)
{
	if (darray)
	{
		darray_header *header_ptr = _darray_get_header(darray);
		return header_ptr->length;
	}

	return -1;
}

int darray_capacity(void *darray)
{
	if (darray)
	{
		darray_header *header_ptr = _darray_get_header(darray);
		return header_ptr->capacity;
	}

	return -1;
}

int darray_stride(void *darray)
{
	if (darray)
	{
		darray_header *header_ptr = _darray_get_header(darray);
		return header_ptr->stride;
	}

	return -1;
}