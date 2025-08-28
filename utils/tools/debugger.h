#pragma once
#include "memory_tracker.h"

void debugger_init();
void debugger_update();
void debugger_deinit();


#define debugger_timestamp_begin(name) __debugger_timestamp_begin(name, __func__, __FILE__, __LINE__)
void __debugger_timestamp_begin(const char* name, const char* function, const char* file, size_t line);
void debugger_timestamp_end(const char* name);
