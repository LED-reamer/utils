#include "allocator.h"

#include <stdlib.h>

#include "tools/memory_tracker.h"

void* allocator_malloc(size_t n) { return malloc(n); }
void* allocator_realloc(void* ptr, size_t n) { return realloc(ptr, n); }
void* allocator_calloc(size_t nmemb, size_t n) { return calloc(nmemb, n); }
void allocator_free(void* ptr) { free(ptr); }

allocator_t util_default_allocator = {
	.amalloc = allocator_malloc,
	.arealloc = allocator_realloc,
	.acalloc = allocator_calloc,
	.afree = allocator_free,
};

allocator_t* current_default_allocator = &util_default_allocator;

allocator_t* allocator_get_default() {
	return current_default_allocator;
}

void allocator_set_default(allocator_t* allocator) {
	if (allocator == NULL)
		current_default_allocator = &util_default_allocator;
	else
		current_default_allocator = allocator;
}
