#include "memory_utils.h"

size_t calculate_padding(uintptr_t address, size_t aligment)
{
	return (aligment - (address % aligment)) % aligment;
}