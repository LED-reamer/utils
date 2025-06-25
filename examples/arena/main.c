#include "utils/arena.h"

int main(void){
	//second argument defines the chunk size
	//third argument how many chunks are allocated from the start
	arena_t my_arena = arena_create(allocator_get_default(), 1024*1024, 0);

	// allocate as much as you like
	void* data = arena_allocate(&my_arena, 5000);
	void* data2 = arena_allocate(&my_arena, 45);
	void* data3 = arena_allocate(&my_arena, 123);
	float* floats = arena_allocate(&my_arena, sizeof(float)*50);

	//free everything at once
	arena_reset(&my_arena);

	//... and start allocating again...
	void* more_data = arena_allocate(&my_arena, 64);

	//destroying the arena surely also frees previous allocations
	arena_destroy(&my_arena);
	return 0;
}
