#include "debugger.h"
#include "../logging.h"
#include "../hashmap.h"//includes linked_list.h
#include <stdlib.h>
#include <string.h>

typedef struct{
	bool initialized;
	bool live_log;
	size_t allocated;
	size_t allocated_max;
	size_t num_allocations;
	size_t num_malloc_calls;
	size_t num_calloc_calls;
	size_t num_realloc_calls;
	size_t num_free_calls;
}__debugger_data_t;

static __debugger_data_t __debugger_data = {0};


void debug_leak_analysis();

//allocators
void* dbg_malloc(size_t);
void* dbg_realloc(void*, size_t);
void* dbg_calloc(size_t, size_t);
void dbg_free(void*);
allocator_t debug_allocator = {
	.malloc = dbg_malloc,
	.realloc = dbg_realloc,
	.calloc = dbg_calloc,
	.free = dbg_free,
};

void* own_malloc(size_t);
void* own_realloc(void*, size_t);
void* own_calloc(size_t, size_t);
void own_free(void*);
allocator_t own_allocator = {
	.malloc = own_malloc,
	.realloc = own_realloc,
	.calloc = own_calloc,
	.free = own_free,
};

//other
hashmap_t* pointers;

void debugger_init(bool live_log){
	__debugger_data = (__debugger_data_t){0};
	__debugger_data.live_log = live_log;
	pointers = hashmap_create(&own_allocator);
	allocator_set_default(&debug_allocator);
	__debugger_data.initialized = true;
}

void printsize(size_t size) {                   
    static const char *SIZES[] = { "B", "KB", "MB", "GB" };
    size_t div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES)) {
        rem = (size % 1024);
        div++;   
        size /= 1024;
    }
    if(div == 0)
    	printf("%zu%s\n", size, SIZES[div]);
    else
    	printf("%.3f%s\n", (float)size + (float)rem / 1024.0, SIZES[div]);
}

void debugger_deinit(){
	LOG("Max allocated was %zu bytes, %zu allocations total, (%zu mallocs, %zu reallocs, %zu callocs, %zu frees)", __debugger_data.allocated_max, __debugger_data.num_allocations, __debugger_data.num_malloc_calls, __debugger_data.num_realloc_calls, __debugger_data.num_calloc_calls, __debugger_data.num_free_calls);
	printf("\t\t\t--> ");printsize(__debugger_data.allocated_max);
	if(__debugger_data.allocated != 0)
		debug_leak_analysis();
	else
		LOG(ANSI_GREEN "[ no memory leak ]" ANSI_RESET);
	
	
	allocator_set_default(NULL);
	hashmap_destroy(pointers);
	__debugger_data.initialized = false;
}

size_t debugger_get_allocated_bytes(){
	if(__debugger_data.initialized == false){
		ERROR("debugger was not initialized!");
		return 0;
	}

	return __debugger_data.allocated;
}

size_t debugger_get_max_allocated_bytes(){
	if(__debugger_data.initialized == false){
		ERROR("debugger was not initialized!");
		return 0;
	}

	return __debugger_data.allocated_max;
}

allocator_t* debugger_get_allocator()
{
	return &debug_allocator;
}

size_t size_max(size_t a, size_t b){
	if(a > b) return a;
	return b;
}

//allocator implementations
void* dbg_malloc(size_t size){
	__debugger_data.num_malloc_calls++;
	void* ptr = malloc(size);
	if(__debugger_data.live_log)
		LOG("malloc: %zu (%p)", size, ptr);
	hashmap_set(pointers, (hashmap_key_t)ptr, (void*)size);

	__debugger_data.allocated += size;
	__debugger_data.num_allocations++;
	__debugger_data.allocated_max = size_max(__debugger_data.allocated_max, __debugger_data.allocated);
	return ptr;
}

void* dbg_realloc(void* ptr, size_t size){
	__debugger_data.num_realloc_calls++;
	void* new_ptr = realloc(ptr, size);
	if(hashmap_contains_key(pointers, (hashmap_key_t)ptr))
	{
		size_t prev_size = (size_t)hashmap_get(pointers, (hashmap_key_t)ptr);
		if(__debugger_data.live_log)
			LOG("realloc existing: %zu (%p) -> %zu (%p)", prev_size, ptr, size, new_ptr);
		hashmap_remove(pointers, (hashmap_key_t)ptr);
		__debugger_data.allocated -= prev_size;
	}
	else
	{
		if(__debugger_data.live_log)
			LOG("realloc new: %zu (%p)", size, new_ptr);
	}
	hashmap_set(pointers, (hashmap_key_t)new_ptr, (void*)size);
	__debugger_data.allocated += size;
	__debugger_data.num_allocations++;
	__debugger_data.allocated_max = size_max(__debugger_data.allocated_max, __debugger_data.allocated);
	return new_ptr;
}

void* dbg_calloc(size_t nmemb, size_t size){
	__debugger_data.num_calloc_calls++;
	void* ptr = calloc(nmemb, size);
	if(__debugger_data.live_log)
		LOG("calloc: %zu (%p)", nmemb * size, ptr);
	hashmap_set(pointers, (hashmap_key_t)ptr, (void*)(nmemb * size));

	__debugger_data.allocated += nmemb * size;
	__debugger_data.num_allocations++;
	__debugger_data.allocated_max = size_max(__debugger_data.allocated_max, __debugger_data.allocated);
	return ptr;
}

void dbg_free(void* ptr){
	__debugger_data.num_free_calls++;
	if(!hashmap_contains_key(pointers, (hashmap_key_t)ptr)){
		ERROR(ANSI_RED"[ double free ]" ANSI_RESET " -> tried freeing at %p", ptr);
	}
	
	size_t size = (size_t)hashmap_get(pointers, (hashmap_key_t)ptr);
	hashmap_remove(pointers, (hashmap_key_t)ptr);
	
	if(__debugger_data.live_log)
		LOG("free: %zu (%p)", size, ptr);
	free(ptr);
	__debugger_data.allocated -= size;
}


void* own_malloc(size_t size){
	return malloc(size);
}
void* own_realloc(void* ptr, size_t size){
	return realloc(ptr, size);
}
void* own_calloc(size_t nmemb, size_t size){
	return calloc(nmemb, size);
}
void own_free(void* ptr){
	free(ptr);
}

void debug_leak_analysis(){
	ERROR(ANSI_RED"[ memory leak ]" ANSI_RESET " -> leaked %zu bytes total", __debugger_data.allocated);

	size_t num_leaks = 0;
	size_t sum_leaked_memory = 0;

	hashmap_bucket_t* bucket;
	size_t size;
	linked_list_foreach(pointers->buckets, &bucket, &size)
	{
		void* leak_ptr = (void*)bucket->key;
		size_t leak_size = (size_t)bucket->value_ptr;
		ERROR("\t-> leak at %p (%zu bytes)", leak_ptr, leak_size);

		num_leaks++;
		sum_leaked_memory += leak_size;
	}
	ERROR("=> summary: "ANSI_RED"%zu leaks"ANSI_RESET" of "ANSI_RED"%zu bytes"ANSI_RESET, num_leaks, sum_leaked_memory);
}
