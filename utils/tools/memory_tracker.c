#define NO_MEMORY_DEBUG
#include "memory_tracker.h"
#include "../logging.h"
#include <stdlib.h>
#include <string.h>

typedef struct{
	bool live_log;
	//stats
	size_t allocations;//general malloc, calloc and new reallocs(equivalent to malloc)
	size_t malloc_calls, realloc_calls, calloc_calls, free_calls;
	size_t currently_allocated, max_allocated;
}debugger_data_t;
static debugger_data_t ctx = {0};

typedef struct{
	void* ptr;
	size_t size;
	const char* func;
	const char* file;
	size_t line;
}allocation_info_t;
static allocation_info_t* infos = NULL;
static size_t num_infos = 0;

void memory_tracker_init(bool live_log){
	ctx = (debugger_data_t){
		.live_log = live_log,
	};
}

void analyse_allocations(){
	LOG("Max allocated %zu bytes, %zu allocations total, (%zu mallocs, %zu reallocs, %zu callocs, %zu frees)", ctx.max_allocated, ctx.allocations, ctx.malloc_calls, ctx.realloc_calls, ctx.calloc_calls, ctx.free_calls);
	if(ctx.currently_allocated == 0){
		LOG(ANSI_GREEN "[ no memory leak ]" ANSI_RESET);
		return;
	}
	ERROR(ANSI_RED"[ memory leak ]" ANSI_RESET " -> leaked %zu bytes total", ctx.currently_allocated);
	size_t sum_memory_leaks = 0;
	size_t sum_leaked_memory = 0;
	for(size_t i = 0; i < num_infos; i++){
		ERROR("\tLeaked %zu bytes at %p allocated in %s %s:%zu", infos[i].size, infos[i].ptr, infos[i].func, infos[i].file, infos[i].line);
		sum_memory_leaks++;
		sum_leaked_memory += infos[i].size;
	}
	if(sum_leaked_memory != ctx.currently_allocated) ERROR("sum_leaked_memory != ctx.currently_allocated ... memory tracker is not working correctly");
	ERROR("=> summary: "ANSI_RED"%zu leaks"ANSI_RESET" of "ANSI_RED"%zu bytes"ANSI_RESET, sum_memory_leaks, sum_leaked_memory);
}

void memory_tracker_deinit(){
	analyse_allocations();
	if(num_infos != 0 && infos == NULL) {ERROR("memory leak in memory_tracker!"); return;};
	if(num_infos != 0)
		free(infos);
	ctx = (debugger_data_t){0};
}

void memory_tracker_get_stats(size_t* currently_allocated, size_t* max_allocated){
	*currently_allocated = ctx.currently_allocated;
	*max_allocated = ctx.max_allocated;
}

void infos_add_info(allocation_info_t info){
	num_infos++;
	infos = realloc(infos, num_infos * sizeof(allocation_info_t));
	infos[num_infos-1] = info;
}

//num_infos = 6
// OO0OOO
// 012345

void infos_remove_ptr(void* ptr)/*splice at index*/{
	if(num_infos == 0 || infos == NULL) return;
	size_t index = 0;
	bool found = false;
	for(size_t i = 0; i < num_infos; i++){
		if(infos[i].ptr == ptr){
			index = i;
			found = true;
		}
	}
	if(!found){
		ERROR("Tried removing ptr from list that was never added");
		return;
	}
	
	if(index < num_infos-1){//if index is last we don't need to move the memory back
		memmove(infos+index, infos+index+1, (num_infos - 1 - index) * sizeof(allocation_info_t));
	}
	num_infos--;
	infos = realloc(infos, num_infos * sizeof(allocation_info_t));
}

allocation_info_t* infos_get_ptr(void* ptr){
	for(size_t i = 0; i < num_infos; i++){
		if(infos[i].ptr == ptr) return &infos[i];
	}
	return NULL;
}

size_t size_max(size_t a, size_t b){
	if(a > b) return a;
	return b;
}

//allocator implementations
void* dbg_malloc(size_t size, const char* func, const char* file, size_t line){
	ctx.malloc_calls++;
	ctx.allocations++;
	ctx.currently_allocated += size;
	ctx.max_allocated = size_max(ctx.max_allocated, ctx.currently_allocated);
	void* ptr = malloc(size);
	if(ctx.live_log)
		LOG("(%p) allocated %zu bytes in %s %s:%zu", ptr, size, func, file, line);
	infos_add_info((allocation_info_t){ .ptr = ptr, .size = size, .func = func, .file = file, .line = line });
	return ptr;
}

void* dbg_realloc(void* ptr, size_t size, const char* func, const char* file, size_t line){
	allocation_info_t* previous_allocation = infos_get_ptr(ptr);
	ctx.realloc_calls++;
	void* new_ptr = realloc(ptr, size);
	if(previous_allocation == NULL){//new allocation (equivalent to malloc)
		if(ctx.live_log)
			LOG("(%p) malloced(via realloc) to %zu bytes in %s %s:%zu", ptr, size, func, file, line);
		infos_add_info((allocation_info_t){ .ptr = new_ptr, .size = size, .func = func, .file = file, .line = line });
		ctx.allocations++;
		ctx.currently_allocated += size;
		ctx.max_allocated = size_max(ctx.max_allocated, ctx.currently_allocated);
	}else{
		if(ctx.live_log)
			LOG("(%p) resized to %zu bytes in %s %s:%zu", new_ptr, size, func, file, line);
		ctx.currently_allocated += - previous_allocation->size + size;
		ctx.max_allocated = size_max(ctx.max_allocated, ctx.currently_allocated);
		//update info
		previous_allocation->ptr = new_ptr;
		previous_allocation->size = size;
		previous_allocation->func = func;
		previous_allocation->file = file;
		previous_allocation->line = line;
	}
	return new_ptr;
}

void* dbg_calloc(size_t nmemb, size_t size, const char* func, const char* file, size_t line){
	ctx.calloc_calls++;
	ctx.allocations++;
	ctx.currently_allocated += nmemb * size;
	ctx.max_allocated = size_max(ctx.max_allocated, ctx.currently_allocated);
	void* ptr = calloc(nmemb, size);
	if(ctx.live_log)
		LOG("(%p) callocated %zu bytes in %s %s:%zu", ptr, nmemb * size, func, file, line);
	infos_add_info((allocation_info_t){ .ptr = ptr, .size = nmemb * size, .func = func, .file = file, .line = line });
	return ptr;
}

void dbg_free(void* ptr, const char* func, const char* file, size_t line){
	if(ptr == NULL){
		ERROR("Tried freeing NULL pointer");
		return;
	}
	allocation_info_t* previous_allocation = infos_get_ptr(ptr);
	if(previous_allocation == NULL){
		ERROR("Tried to double free %p at %s %s:%zu", ptr, func, file, line);
		return;
	}
	ctx.free_calls++;
	ctx.currently_allocated -= previous_allocation->size;
	ctx.max_allocated = size_max(ctx.max_allocated, ctx.currently_allocated);
	free(ptr);
	if(ctx.live_log)
		LOG("(%p) was freed in %s %s:%zu", ptr, func, file, line);

	infos_remove_ptr(ptr);
}
