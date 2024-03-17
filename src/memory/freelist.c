#include "freelist.h"
#include <memory.h>

#define MIN_ENTRIES_COUNT_REQUIRED 20

typedef struct freelist_node
{
	size_t size;
	size_t offset;
	struct freelist_node* next;
} freelist_node;

typedef struct freelist_state
{
	size_t total_size;
	size_t max_entries;
	freelist_node* head;
	freelist_node* nodes;
} freelist_state;

#define MEMROY_USED_FOR_NODES_PERCENT 0.1f

static int get_estimated_max_entries(u64 total_size)
{
	return total_size * MEMROY_USED_FOR_NODES_PERCENT / sizeof(freelist_node);
}

b8 freelist_create(u64 total_size, u64* memory_requirement, void* memory, freelist* out_freelist)
{
	if (!memory_requirement)
	{
		return FALSE;
	}

	int max_entries = get_estimated_max_entries(total_size);

	if (max_entries < MIN_ENTRIES_COUNT_REQUIRED)
	{
		max_entries = MIN_ENTRIES_COUNT_REQUIRED;
	}

	*memory_requirement = sizeof(freelist_state) + sizeof(freelist_node) * max_entries;

	if (total_size < *memory_requirement)
	{
		return FALSE;
	}

	if (!memory)
	{
		return TRUE;
	}

	out_freelist->memory = memory;

	memset(out_freelist->memory, 0, *memory_requirement);
	freelist_state* state = out_freelist->memory;
	state->nodes = (u8*)out_freelist->memory + sizeof(freelist_state);
	state->max_entries = max_entries;
	state->total_size = total_size;

	state->head = &state->nodes[0];
	state->head->offset = 0;
	state->head->size = total_size;
	state->head->next = 0;

	for (int i = 1; i < max_entries; ++i)
	{
		freelist_node* node = &state->nodes[i];
		node->offset = INVALID_ID_U64;
		node->size = INVALID_ID_U64;
	}

	return TRUE;
}

static void return_node(freelist_node* node)
{
	node->offset = INVALID_ID_U64;
	node->size = INVALID_ID_U64;
	node->next = NULL;
}

static freelist_node* get_node(freelist* list)
{
	freelist_state* state = list->memory;
	for (int i = 1; i < state->max_entries; ++i)
	{
		if (state->nodes[i].size == 0)
		{
			state->nodes[i].offset = 0;
			state->nodes[i].next = NULL;
			return &state->nodes[i];
		}
	}

	return NULL;
}

u64 freelist_get_free_space(freelist* list)
{
	freelist_state* state = list->memory;
	freelist_node* node = state->head;
	u64 free_space = 0;
	while (node)
	{
		free_space += node->size;
		node = node->next;
	}

	return free_space;
}

b8 freelist_allocate_block(freelist* list, u64 size, u64* out_offset)
{
	if (list == NULL || list->memory == NULL || out_offset == NULL)
	{
		return FALSE;
	}

	freelist_state* state = list->memory;
	freelist_node* node = state->head;
	freelist_node* previous = NULL;
	while (node)
	{
		if (node->size == size)
		{
			*out_offset = node->offset;
			freelist_node* node_to_return;
			if (previous)
			{
				node_to_return = node;
				previous->next = node->next;
			}
			else
			{
				node_to_return = state->head;
				state->head = node->next;
			}
			return_node(node_to_return);
			return TRUE;
		}
		else if (node->size > size)
		{
			*out_offset = node->offset;
			node->size -= size;
			node->offset += size;
			return TRUE;
		}

		previous = node;
		node = node->next;
	}

	return FALSE;
}

b8 freelist_free_block(freelist* list, u64 size, u64 offset)
{
	if (!list || !size || !list->memory)
	{
		return FALSE;
	}

	freelist_state* state = list->memory;
	freelist_node* node = state->head;
	freelist_node* previous = NULL;

	if (!node)
	{
		freelist_node* new_node = get_node(list);
		new_node->offset = offset;
		new_node->size = size;
		new_node->next = NULL;
		state->head = new_node;
		return TRUE;
	}

	while (node)
	{
		if (node->offset + node->size == offset)
		{
			node->size += size;
			if (node->next && node->offset + node->size == node->next->offset)
			{
				node->size += node->next->size;
				freelist_node* next = node->next;
				node->next = node->next->next;
				return_node(next);
			}
			return TRUE;
		}

		if (node->offset == offset)
		{
			// Attempt to free already freed block
			return FALSE;
		}

		if (node->offset > offset)
		{
			freelist_node* new_node = get_node(list);
			if (new_node)
			{
				new_node->offset = offset;
				new_node->size = size;

				if (previous)
				{
					previous->next = new_node;
					new_node->next = node;
				}
				else
				{
					new_node->next = node;
					state->head->next = new_node;
				}

				if (new_node->offset + new_node->size == node->offset)
				{
					new_node->size += node->size;
					new_node->next = node->next;
					return_node(node);
				}

				if (previous && previous->offset + previous->size == new_node->offset)
				{
					previous->size += new_node->size;
					previous->next = new_node->next;
					return_node(new_node);
				}

				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}

		if (!node->next && node->offset + node->size < offset)
		{
			freelist_node* new_node = get_node(list);
			new_node->offset = offset;
			new_node->size = size;
			new_node->next = NULL;
			node->next = new_node;
			return TRUE;
		}

		previous = node;
		node = node->next;
	}

	return FALSE;
}

b8 freelist_resize(freelist* list, u64 new_total_size, u64* memory_requirement, void* new_memory)
{
	if (!list || !list->memory)
	{
		return FALSE;
	}

	freelist* old_list = list;
	freelist_create(new_total_size, memory_requirement, new_memory, list);

	if (!new_memory)
	{
		return;
	}

	freelist_state* state = list->memory;
	freelist_state* old_state = old_list->memory;
	freelist_node* old_node = old_state->head;
	freelist_node* node = state->head;

	u64 size_diff = new_total_size - old_state->total_size;

	if (!old_node)
	{
		state->head->offset = old_state->total_size;
		state->head->size = size_diff;
		state->head->next = NULL;
	}
	else
	{
		while (old_node)
		{
			freelist_node* new_node = get_node(list);
			new_node->offset = old_node->offset;
			new_node->size = old_node->size;
			node->next = node;
			if (old_node->next)
			{
				node = node->next;
				old_node = old_node->next;
			}
			else
			{
				if (old_node->size + old_node->offset == old_state->total_size)
				{
					new_node->size += size_diff;
				}
				else
				{
					freelist_node* new_node_end = get_node(list);
					new_node_end->offset = old_state->total_size;
					new_node_end->size = size_diff;
					new_node_end->next = NULL;
					new_node->next = new_node_end;
				}
				break;
			}
		}
	}

	return TRUE;
}

void freelist_clear(freelist* list)
{
	if (!list || !list->memory)
	{
		return;
	}

	freelist_state* state = list->memory;
	memset(state->nodes, 0, sizeof(freelist_node) * state->max_entries);

	state->head->offset = 0;
	state->head->size = state->total_size;
	state->head->next = NULL;
}

void freelist_destroy(freelist* list)
{
	if (!list || !list->memory)
	{
		return;
	}

	freelist_state* state = list->memory;
	memset(state->nodes, 0, sizeof(freelist_node) * state->max_entries);

	list->memory = NULL;
}