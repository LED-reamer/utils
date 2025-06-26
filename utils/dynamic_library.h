//link with -ldl
#pragma once

typedef void* dynamic_library_t;

dynamic_library_t dynamic_library_load(char* file_name);
void dynamic_library_close(dynamic_library_t dl);
void* dynamic_library_get_symbol(dynamic_library_t dl, char* symbol_string);
char* dynamic_library_error();
