#pragma once
#include <stddef.h>

typedef struct
{
	void* (*malloc)(size_t);
	void* (*realloc)(void*, size_t);
	void* (*calloc)(size_t, size_t);
	void  (*free)(void *);
}allocator_t;

allocator_t* allocator_get_default();
void allocator_set_default(allocator_t* allocator);

void print_stats();
