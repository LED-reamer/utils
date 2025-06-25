#include "arena.h"
#include "logging.h"

#include <string.h>


#define MALLOC(size) arena->allocator->malloc(size);
#define REALLOC(dest, newsize) arena->allocator->realloc(dest, newsize);
#define FREE(ptr) arena->allocator->free(ptr);


arena_t* arena_create(allocator_t* allocator, size_t chunk_size, size_t num_start_chunks)
{
	arena_t* arena = allocator->malloc(sizeof(arena_t));
	arena->allocator = allocator;
	
	arena->chunk_size = chunk_size;
	arena->num_start_chunks = num_start_chunks;
	arena->num_chunks = num_start_chunks;
	arena->data = MALLOC(chunk_size * num_start_chunks);
	arena->current_pos = (void*)arena->data;
	return arena;
}

void* arena_alloc(arena_t* arena, size_t size)
{
    if ((arena->current_pos + size) > (arena->data + (arena->num_chunks * arena->chunk_size)))
    {
    	arena->num_chunks += (size_t)((size / arena->chunk_size) + 1);
        
        size_t new_size = arena->num_chunks * arena->chunk_size;
        void* new_data = REALLOC(arena->data, new_size);
        
        if (!new_data)
        {
            ERROR("Could not allocate extra arena memory");
            return NULL;
        }
        
        arena->current_pos = new_data + (arena->current_pos - arena->data);
        arena->data = new_data;
    }

    void* ptr = arena->current_pos;
    arena->current_pos += size;

    return memset(ptr, 0, size);
}

void arena_reset(arena_t* arena)
{
	if(arena->num_chunks != arena->num_start_chunks)
	{
		arena->num_chunks = arena->num_start_chunks;
		arena->data = REALLOC(arena->data, arena->num_chunks * arena->chunk_size);
	}
	
	arena->current_pos = arena->data;
}

void arena_destroy(arena_t* arena)
{
	if(arena->data != NULL)
		FREE(arena->data);
	FREE(arena);
}
