#include "arena.h"
#include "logging.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

arena_t arena_create(allocator_t* allocator, size_t block_size, arena_scaling_e scaling) {
	if(block_size == 0) block_size = 1;
	arena_t arena = (arena_t){
		.allocator = allocator,
		.block_size = block_size,
		.scaling = scaling,
		.size = 0,
		.position = 0,
	};
	return arena;
}

void arena_destroy(arena_t* arena) {
	if (arena->data != NULL)
		arena->allocator->afree(arena->data);
	*arena = (arena_t){0};
}

bool __arena_check_size(arena_t* arena, size_t new_size){
	if (new_size > arena->size) /*needs resizing*/ {
		size_t new_allocation_size = 0;
		switch(arena->scaling){
			case ARENA_FIXED: return false; break;//could not allocate more
			case ARENA_LINEAR:
				size_t needed_blocks = (new_size + arena->block_size - 1) / arena->block_size;
				if (needed_blocks > SIZE_MAX / arena->block_size)
				    return false;
				new_allocation_size = needed_blocks * arena->block_size;
				break;
			case ARENA_QUADRATIC:
				new_allocation_size = arena->size;
				if (new_allocation_size == 0)
				    new_allocation_size = arena->block_size;
				
				while (new_allocation_size < new_size) {
				    if (new_allocation_size > SIZE_MAX / 2) return false;
				    new_allocation_size *= 2;
				}
				break;
			default: return false; break;
		}

		void* new_data = arena->allocator->arealloc(arena->data, new_allocation_size);
		if (!new_data) return false;
		if (new_allocation_size > arena->size)
			memset(new_data + arena->size, 0, new_allocation_size - arena->size);
		arena->data = new_data;
		arena->size = new_allocation_size;
	}

	return true;
}

void* arena_allocate(arena_t* arena, size_t size) {
	if(!__arena_check_size(arena, arena->position + size)) return NULL;//allocation failed

	void* ptr = arena->data + arena->position;
	arena->position += size;
	return ptr;
}

size_t arena_allocate_index(arena_t* arena, size_t size){
	if(!__arena_check_size(arena, arena->position + size)){
		ERROR("arena_allocate_index could not allocate %zu bytes", size);
		return 0;
	}
	
	size_t index_position = arena->position;
	arena->position += size;
	return index_position;
}

void arena_set_position(arena_t* arena, size_t new_position){
	if(new_position < arena->size) arena->position = new_position;
}

void arena_reset(arena_t* arena) {
	arena_set_position(arena, 0);
}

size_t arena_allocated(arena_t* arena){
	return arena->position;
}
