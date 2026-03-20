#include "buffer.h"
#define ALLOCATOR_DEBUG
#include "hstd/allocator.h"
#include <string.h>


error_t buffer_from_file(buffer_t* buffer, path_t filepath){
	*buffer = (buffer_t){0};
	buffer->filepath = filepath;

	file_t f = file_open(filepath.name);
	if(f.ptr == NULL) return error("Could not open file");
	buffer->size = file_get_size(&f)+1/*for null terminator*/;
	buffer->data = allocator.malloc(buffer->size);
	if(buffer->data == NULL) return error("Could not allocate enough memory");
	size_t bytes_read = file_read(&f, buffer->data, buffer->size-1/*don't read null terminator*/);
	if(bytes_read != buffer->size-1) return error("Could not copy file content to buffer");
	if(!file_close(&f)) return error("Could not close file");

	buffer->data[buffer->size-1] = '\0';

	return ok();
}

error_t buffer_copy(buffer_t* buffer, cstring source, size_t size){
	*buffer = (buffer_t){0};
	buffer->size = size;
	buffer->data = allocator.malloc(buffer->size);
	if(buffer->data == NULL) return error("Could not allocate enough memory");
	memcpy(buffer->data, source, size);
	return ok();
}

void buffer_destroy(buffer_t* buffer){
	allocator.free(buffer->data);
	*buffer = (buffer_t){0};
}
