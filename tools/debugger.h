#pragma once
#include "../allocator.h"

#include <stddef.h>
#include <stdbool.h>
/*
#ifdef malloc
#undef malloc
#undef realloc
#undef calloc
#undef free
#endif

#define malloc(...) dbg_malloc(...)
#define realloc(...) dbg_realloc(...)
#define calloc(...) dbg_calloc(...)
#define free(...) dbg_free(...)

extern void* dbg_malloc(size_t);
extern void* dbg_realloc(void*, size_t);
extern void* dbg_calloc(size_t, size_t);
extern void dbg_free(void*);
*/
void debugger_init(bool live_log);
void debugger_deinit();
size_t debugger_get_allocated_bytes();
size_t debugger_get_max_allocated_bytes();
allocator_t* debugger_get_allocator();
