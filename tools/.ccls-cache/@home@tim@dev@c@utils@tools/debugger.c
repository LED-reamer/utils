#include "debugger.h"
#include "../logging.h"
#include "../allocator.h"
#include "../hashmap.h"//includes linked_list.h
#include <stdlib.h>
#include <string.h>

bool debug_initialized = false;
bool debug_live_log = false;
size_t debugger_allocated = 0;
size_t debugger_allocated_max = 0;
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
	debug_live_log = live_log;
	pointers = hashmap_create(&own_allocator);
	allocator_set_default(&debug_allocator);
	debug_initialized = true;
}

void debugger_deinit(){
	LOG("Max allocated was %zu bytes", debugger_allocated_max);
	if(debugger_allocated != 0)
		debug_leak_analysis();
	else
		LOG(ANSI_GREEN "[ no memory leak ]");
	
	
	allocator_set_default(NULL);
	hashmap_destroy(pointers);
	debug_initialized = false;
}

size_t debugger_get_allocated_bytes(){
	if(debug_initialized == false){
		ERROR("debugger was not initialized!");
		return 0;
	}

	return debugger_allocated;
}

size_t size_max(size_t a, size_t b){
	if(a > b) return a;
	return b;
}

//allocator implementations
void* dbg_malloc(size_t size){
	void* ptr = malloc(size);
	if(debug_live_log)
		LOG("malloc: %zu (%p)", size, ptr);
	hashmap_set(pointers, (hashmap_key_t)ptr, (void*)size);

	debugger_allocated += size;
	debugger_allocated_max = size_max(debugger_allocated_max, debugger_allocated);
	return ptr;
}

void* dbg_realloc(void* ptr, size_t size){
	void* new_ptr = realloc(ptr, size);
	if(hashmap_contains_key(pointers, (hashmap_key_t)ptr))
	{
		size_t prev_size = (size_t)hashmap_get(pointers, (hashmap_key_t)ptr);
		if(debug_live_log)
			LOG("realloc existing: %zu (%p) -> %zu (%p)", prev_size, ptr, size, new_ptr);
		hashmap_remove(pointers, (hashmap_key_t)ptr);
		debugger_allocated -= prev_size;
	}
	else
	{
		if(debug_live_log)
			LOG("realloc new: %zu (%p)", size, new_ptr);
	}
	hashmap_set(pointers, (hashmap_key_t)new_ptr, (void*)size);
	debugger_allocated += size;
	debugger_allocated_max = size_max(debugger_allocated_max, debugger_allocated);
	return new_ptr;
}

void* dbg_calloc(size_t nmemb, size_t size){
	void* ptr = calloc(nmemb, size);
	if(debug_live_log)
		LOG("calloc: %zu (%p)", nmemb * size, ptr);
	hashmap_set(pointers, (hashmap_key_t)ptr, (void*)(nmemb * size));

	debugger_allocated += nmemb * size;
	debugger_allocated_max = size_max(debugger_allocated_max, debugger_allocated);
	return ptr;
}

void dbg_free(void* ptr){
	size_t size = (size_t)hashmap_get(pointers, (hashmap_key_t)ptr);
	hashmap_remove(pointers, (hashmap_key_t)ptr);
	if(debug_live_log)
		LOG("free: %zu (%p)", size, ptr);
	free(ptr);
	debugger_allocated -= size;
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
	ERROR(ANSI_RED"[ memory leak ]" ANSI_RESET " -> leaked %zu bytes total", debugger_allocated);

	hashmap_bucket_t* bucket;
	size_t size;
	linked_list_foreach(pointers->buckets, &bucket, &size)
	{
		void* leak_ptr = (void*)bucket->key;
		size_t leak_size = (size_t)bucket->value_ptr;
		ERROR("\t-> leak at %p (%zu bytes)", leak_ptr, leak_size);
	}
}
