#pragma once
#include "hstd/error.h"
#include "hstd/file.h"
#include <stddef.h>

typedef struct{
	char* data;
	size_t size;

	path_t filepath;
}buffer_t;

error_t buffer_from_file(buffer_t* buffer, path_t filepath);
error_t buffer_copy(buffer_t* buffer, char* source, size_t size);
void buffer_destroy(buffer_t* buffer);
