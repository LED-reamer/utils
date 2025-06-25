#include "allocator.h"
#define _DEBUG

#ifdef _DEBUG
#define UTIL_ALLOCATOR_STATS
#endif


#if !defined(UTIL_ALLOCATOR_MALLOC) || !defined(UTIL_ALLOCATOR_REALLOC) || !defined(UTIL_ALLOCATOR_CALLOC) || !defined(UTIL_ALLOCATOR_FREE)
#include <stdlib.h>
#include <string.h>
#endif

#ifdef UTIL_ALLOCATOR_STATS
#include <stdio.h>
#include <stddef.h>
	size_t num_allocs = 0;
	size_t num_reallocs = 0;
	size_t num_frees = 0;
	size_t bytes_allocated = 0;
	size_t bytes_reallocated = 0;

	void print_stats()
	{
		printf("num_allocs = %zu\n", num_allocs);
		printf("num_reallocs = %zu\n", num_reallocs);
		printf("num_frees = %zu\n", num_frees);
		printf("bytes_allocated = %zu\n", bytes_allocated);
		printf("bytes_reallocated = %zu\n", bytes_reallocated);
	}
	
	#ifndef UTIL_ALLOCATOR_MALLOC
	void* util_malloc(size_t size)
	{
		num_allocs++;
		bytes_allocated += size;
		return malloc(size);
	}
	#define UTIL_ALLOCATOR_MALLOC util_malloc
	#endif
	
	#ifndef UTIL_ALLOCATOR_REALLOC
	void* util_realloc(void* ptr, size_t size)
	{
		num_reallocs++;
		bytes_reallocated += size;
		return realloc(ptr, size);
	}
	#define UTIL_ALLOCATOR_REALLOC util_realloc
	#endif
	
	#ifndef UTIL_ALLOCATOR_CALLOC
	void* util_calloc(size_t nmemb, size_t size)
	{
		void* ptr = util_malloc(nmemb * size);
		ptr = memset(ptr, 0, nmemb * size);
		return ptr;
		//return calloc(nmemb, size);
	}
	#define UTIL_ALLOCATOR_CALLOC util_calloc
	#endif
	
	#ifndef UTIL_ALLOCATOR_FREE
	void util_free(void* ptr)
	{
		num_frees++;
		free(ptr);
	}
	#define UTIL_ALLOCATOR_FREE util_free
	#endif

#else

	#ifndef UTIL_ALLOCATOR_MALLOC
	#define UTIL_ALLOCATOR_MALLOC malloc
	#endif
	
	#ifndef UTIL_ALLOCATOR_REALLOC
	#define UTIL_ALLOCATOR_REALLOC realloc
	#endif
	
	#ifndef UTIL_ALLOCATOR_CALLOC
	#define UTIL_ALLOCATOR_CALLOC calloc
	#endif
	
	#ifndef UTIL_ALLOCATOR_FREE
	#define UTIL_ALLOCATOR_FREE free
	#endif
	
#endif

allocator_t util_default_allocator = { 
	.malloc = &UTIL_ALLOCATOR_MALLOC,
	.realloc = &UTIL_ALLOCATOR_REALLOC,
	.calloc = &UTIL_ALLOCATOR_CALLOC,
	.free = &UTIL_ALLOCATOR_FREE,
};

allocator_t* current_default_allocator = &util_default_allocator;

allocator_t* allocator_get_default()
{
	return current_default_allocator;
}

void allocator_set_default(allocator_t* allocator)
{
	if(allocator == NULL)
		current_default_allocator = &util_default_allocator;
	else
		current_default_allocator = allocator;
}
