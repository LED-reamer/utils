//link with -ldl

#pragma once

typedef void* dynamic_library_t;

dynamic_library_t dl_load(char* file_name);
void dl_close(dynamic_library_t dl);
void* dl_get_symbol(dynamic_library_t dl, char* symbol_string);
char* dl_error();
