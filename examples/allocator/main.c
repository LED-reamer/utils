#include "utils/allocator.h"
#include "utils/tools/memory_tracker.h"

#include <stdlib.h>//for custom allocator functions

void* custom_malloc(size_t size){ return malloc(size); }
void* custom_realloc(void* ptr, size_t size){ return realloc(ptr, size); }
void* custom_calloc(size_t nmemb, size_t size){ return calloc(nmemb, size); }
void custom_free(void* ptr){ free(ptr); }

allocator_t my_custom_allocator = (allocator_t){
	.amalloc = custom_malloc,
	.arealloc = custom_realloc,
	.acalloc = custom_calloc,
	.afree = custom_free,
};

int main(void){
	memory_tracker_init(true);
	// -= default allocator =-
	allocator_t* my_allocator = allocator_get_default();

	void* some_memory = my_allocator->amalloc(1024);
	some_memory = my_allocator->arealloc(some_memory, 2048);
	int* some_more_memory = my_allocator->acalloc(500, sizeof(int));
	my_allocator->afree(some_memory);
	my_allocator->afree(some_more_memory);

	// -= custom allocator =-

	void* custom_allocated_memory = my_custom_allocator.amalloc(1000);
	my_custom_allocator.afree(custom_allocated_memory);
	//...

	// You can also set your custom allocator as the new default
	allocator_set_default(&my_custom_allocator);

	allocator_t* still_my_custom_allocator = allocator_get_default();


	//functions that allocate memory should always ask for an allocator_t*
	// example: buffer_create(allocator_t* allocator, ...)
	memory_tracker_deinit();
	return 0;
}
