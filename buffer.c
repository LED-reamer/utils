#include "buffer.h"
#include "logging.h"

#define MALLOC buffer->allocator->malloc
#define REALLOC buffer->allocator->realloc
#define FREE buffer->allocator->free

buffer_t* buffer_create(allocator_t* allocator)
{
	buffer_t* buffer = allocator->malloc(sizeof(buffer_t));
	buffer->allocator = allocator;
	buffer->size = 0;
	buffer->data = NULL;
	buffer->read_offset = 0;

	return buffer;
}

void buffer_destroy(buffer_t* buffer)
{
	buffer_free_data(buffer);
	FREE(buffer);
}

void buffer_copy_data(buffer_t* buffer, void* data, size_t size)
{
	buffer_reserve_memory(buffer, size);
	memcpy(buffer->data, data, size);
}

void buffer_reserve_memory(buffer_t* buffer, size_t size)
{
	if(buffer->size != size)
	{
		if(size == 0)
		{
			if(buffer->data)
				FREE(buffer->data);
			buffer->data = NULL;
			buffer->size = 0;
		}
		else
		{
			buffer->data = REALLOC(buffer->data, size);
			buffer->size = size;
		}
	}
}

void buffer_free_data(buffer_t* buffer)
{
	//if(buffer->data)
	//	FREE(buffer->data);
	//buffer->size = 0;
	//TODO if you see this comment again you can remove the commented code above... it has been proven it works...
	buffer_reserve_memory(buffer, 0);
}

buffer_result_e buffer_load_data_from_file(buffer_t* buffer, const char* filename)
{
	FILE *file = fopen(filename, "rb");
	if(buffer_load_data_from_file_ptr(buffer, file) != BUFFER_OK)
		return BUFFER_ERROR;
	fclose(file);
	return BUFFER_OK;
}

buffer_result_e buffer_load_data_from_file_ptr(buffer_t* buffer, FILE* file)
{
	if(file != NULL)
	{
		fseek(file, 0, SEEK_END);
		long file_size = ftell(file);
		fseek(file, 0, SEEK_SET);
		
		buffer_reserve_memory(buffer, file_size);
		fread(buffer->data, file_size, 1, file);
	}
	else
	{
		buffer_free_data(buffer);
		return BUFFER_ERROR;
	}

	return BUFFER_OK;
}

void buffer_write_to_file(buffer_t* buffer, const char* filename)
{
	FILE *file = fopen(filename, "wb");
	buffer_write_to_file_ptr(buffer, file);
	fclose(file);
}

void buffer_write_to_file_ptr(buffer_t* buffer, FILE* file)
{
	fwrite(buffer->data, buffer->size, 1, file);
}

uint8_t check_for_read_offset_overflow(buffer_t* buffer, size_t tried_offset)//returns 1 on error
{
	if(buffer->read_offset + tried_offset > buffer->size)
	{
		ERROR("Buffer read out of bounds! (You reached the end of the buffer)");
		return 1;
	}
	return 0;
}

void buffer_set_offset(buffer_t* buffer, size_t new_offset)
{
	if(new_offset > buffer->size)
	{
		ERROR("Could not set new_offset for buffer!");
	}
	else
		buffer->read_offset = new_offset;
}

uint64_t null_int = 0;
void* buffer_read_buffer(buffer_t* buffer, size_t size)
{
	if(!check_for_read_offset_overflow(buffer, size))
	{
		void* ptr = buffer->data + buffer->read_offset;
		buffer->read_offset += size;
		return ptr;
	}
	else
		return &null_int;
}

const char* buffer_read_cstring(buffer_t* buffer)
{
	size_t string_start_pos = buffer->read_offset;
	while(!check_for_read_offset_overflow(buffer, sizeof(char)))
	{
		char current_char = buffer_read(buffer, int8_t);
		if(current_char == '\0')
			break;
	}

	return buffer->data + string_start_pos;
}

const char* buffer_read_allocated_cstring(buffer_t* buffer, allocator_t* allocator)
{
	const char* string = buffer_read_cstring(buffer);
	char* allocated_string = allocator->malloc(strlen(string) + 1);
	memcpy(allocated_string, string, strlen(string) + 1);
	return allocated_string;
}

void buffer_write_buffer(buffer_t* buffer, void* data, size_t size)
{
	buffer_reserve_memory(buffer, buffer->size + size);
	if(!check_for_read_offset_overflow(buffer, size))
	{
		memcpy(buffer->data + buffer->read_offset, data, size);
		buffer->read_offset += size;
	}
}
