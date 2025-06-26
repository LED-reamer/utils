#pragma once
#include "allocator.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


typedef struct
{
	allocator_t* allocator;
	
	size_t size;
	void* data;

	//buffer reading (for parsing etc)
	size_t read_offset;
}buffer_t;

typedef enum
{
	BUFFER_ERROR = 0,
	BUFFER_OK = 1,
}buffer_result_e;

buffer_t buffer_create(allocator_t* allocator);
void buffer_destroy(buffer_t* buffer);

//data handling
void buffer_copy_data(buffer_t* buffer, void* data, size_t size);//auto resizes
void buffer_reserve_memory(buffer_t* buffer, size_t size);
void buffer_free_data(buffer_t* buffer);
buffer_result_e buffer_load_data_from_file(buffer_t* buffer, const char* filename);
buffer_result_e buffer_load_data_from_file_ptr(buffer_t* buffer, FILE* file);

void buffer_write_to_file(buffer_t* buffer, const char* filename);
void buffer_write_to_file_ptr(buffer_t* buffer, FILE* file);

//buffer reading --- all functions advance the read_offset
void buffer_set_offset(buffer_t* buffer, size_t new_offset);
void* buffer_read_buffer(buffer_t* buffer, size_t size);
#define buffer_read(buffer, type) *(type*)buffer_read_buffer((buffer), sizeof(type))
const char* buffer_read_cstring(buffer_t* buffer);//returned string only exists for the lifetime of buffer
const char* buffer_read_allocated_cstring(buffer_t* buffer, allocator_t* allocator);//returned string needs to be freed by the user

//buffer writing --- all functions still advance the read_offset and reserve more memory
void buffer_write_buffer(buffer_t* buffer, void* data, size_t size);
#define buffer_write(buffer, data, type) buffer_write_buffer((buffer), &data, sizeof(type))
#define buffer_write_cstring(buffer, string) buffer_write_buffer((buffer), (void*)string, strlen(string)+1)
