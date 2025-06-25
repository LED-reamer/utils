#pragma once
#include "../allocator.h"

typedef struct
{
	float position[3];
	float normal[3];
	float color[3];//uses uint16_t attribute_byte_count for color if the file supports the extension
}stl_vertex_t;

typedef stl_vertex_t stl_model_t;// dynamic array


stl_model_t* stl_model_load_from_file(allocator_t* allocator, const char* filename);
stl_model_t* stl_model_load_from_memory(allocator_t* allocator, void* data, size_t size);

void stl_model_destroy(stl_model_t* stl_model);
