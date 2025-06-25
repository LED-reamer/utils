#pragma once
#include <stddef.h>
#include <stdbool.h>

void debugger_init(bool live_log);
void debugger_deinit();
size_t debugger_get_allocated_bytes();
size_t debugger_get_max_allocated_bytes();
allocator_t* debugger_get_max_allocated_bytes();
