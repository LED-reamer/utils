#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <stdlib.h>
#include "logger.h"

typedef struct{
	void* (*malloc)(size_t size);
	void* (*calloc)(size_t count, size_t size);
	void* (*realloc)(void* ptr, size_t size);
	void (*free)(void* ptr);
}allocator_t;

#ifndef ALLOCATOR_DEBUG
static allocator_t allocator = {
	.malloc = &malloc,
	.calloc = &calloc,
	.realloc = &realloc,
	.free = &free,
};
#else

void* allocator_malloc(size_t size){
	logger.info("allocated %zu bytes", size);
	return malloc(size);
}
void* allocator_calloc(size_t count, size_t size){
	logger.info("callocated %zu bytes", count * size);
	return calloc(count, size);
}
void* allocator_realloc(void* ptr, size_t size){
	logger.info("reallocated %zu bytes at %p", size, ptr);
	return realloc(ptr, size);
}
void allocator_free(void* ptr){
	logger.info("freed %p", ptr);
	return free(ptr);
}

static allocator_t allocator = {
	.malloc = &allocator_malloc,
	.calloc = &allocator_calloc,
	.realloc = &allocator_realloc,
	.free = &allocator_free,
};
#endif

#endif
