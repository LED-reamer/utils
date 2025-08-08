#pragma once
#include <stddef.h>

typedef struct
{
	void* (*amalloc)(size_t);
	void* (*arealloc)(void*, size_t);
	void* (*acalloc)(size_t, size_t);
	void (*afree)(void*);
} allocator_t;

allocator_t* allocator_get_default();
void allocator_set_default(allocator_t* allocator);
