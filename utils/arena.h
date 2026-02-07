#pragma once
#include <stddef.h>

#include "allocator.h"

typedef enum{
	ARENA_FIXED = 0,//returns NULL pointer when allocating too much
	ARENA_LINEAR = 1,	//grows one block at a time
	ARENA_QUADRATIC = 2,//grows quadratic
} arena_scaling_e;

typedef struct {
	allocator_t* allocator;

	size_t block_size;
	arena_scaling_e scaling;

	void* data;
	size_t size;
	size_t position;
} arena_t;

arena_t arena_create(allocator_t* allocator, size_t block_size, arena_scaling_e scaling);
void arena_destroy(arena_t* arena);
// allocations are zero initialized
//Pointers returned by arena_allocate are invalidated if the arena grows (realloc)!
void* arena_allocate(arena_t* arena, size_t size);
//index can't be invalidated!
size_t arena_allocate_index(arena_t* arena, size_t size);

void arena_set_position(arena_t* arena, size_t new_position);
void arena_reset(arena_t* arena);//equivalent to "arena_set_position(&a, 0)", does not shrink memory

size_t arena_allocated(arena_t* arena);//returns arena->position
