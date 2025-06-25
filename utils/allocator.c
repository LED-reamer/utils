#include "allocator.h"
#include <stdlib.h>

allocator_t util_default_allocator = { 
	.malloc = malloc,
	.realloc = realloc,
	.calloc = calloc,
	.free = free,
};

allocator_t* current_default_allocator = &util_default_allocator;

allocator_t* allocator_get_default()
{
	return current_default_allocator;
}

void allocator_set_default(allocator_t* allocator)
{
	if(allocator == NULL)
		current_default_allocator = &util_default_allocator;
	else
		current_default_allocator = allocator;
}
