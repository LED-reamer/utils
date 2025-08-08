#pragma once
#include <stddef.h>

#include "allocator.h"

typedef struct
{
	allocator_t* allocator;

	size_t num_start_chunks;  // fixed value
	size_t num_chunks;
	size_t chunk_size;	// fixed value
	void* data;
	void* current_pos;
} arena_t;

arena_t arena_create(allocator_t* allocator, size_t chunk_size, size_t num_start_chunks);
// allocations are zero initialized
void* arena_allocate(arena_t* arena, size_t size);
void arena_reset(arena_t* arena);
void arena_destroy(arena_t* arena);
