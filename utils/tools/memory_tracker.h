#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#if !defined(NO_MEMORY_DEBUG)

#ifdef malloc
#undef malloc
#undef realloc
#undef calloc
#undef free
#endif

#define malloc(size) dbg_malloc(size, __func__, __FILE__, __LINE__)
#define realloc(ptr, size) dbg_realloc(ptr, size, __func__, __FILE__, __LINE__)
#define calloc(nmemb, size) dbg_calloc(nmemb, size, __func__, __FILE__, __LINE__)
#define free(ptr) dbg_free(ptr, __func__, __FILE__, __LINE__)

#endif

void memory_tracker_init(bool live_log);
void memory_tracker_deinit();

void memory_tracker_get_stats(size_t* currently_allocated, size_t* max_allocated);

void* dbg_malloc(size_t size, const char* func, const char* file, size_t line);
void* dbg_realloc(void* ptr, size_t size, const char* func, const char* file, size_t line);
void* dbg_calloc(size_t nmemb, size_t size, const char* func, const char* file, size_t line);
void dbg_free(void* ptr, const char* func, const char* file, size_t line);
