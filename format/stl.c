#include "stl.h"
#include "../dynamic_array.h"
#include "../buffer.h"
#include "../logging.h"

stl_model_t* stl_model_load_from_file(allocator_t* allocator, const char* filename)
{
	buffer_t* buffer = buffer_create(allocator);
	buffer_load_data_from_file(buffer, filename);
	stl_model_t* stl_model = stl_model_load_from_memory(allocator, buffer->data, buffer->size);
	buffer_destroy(buffer);

	return stl_model;
}

void __get_color_from_attribute(uint16_t attribute_byte_count, float color[3])
{
    if (attribute_byte_count & 0x8000)
    {
        uint8_t blue   = attribute_byte_count & 0x1F;
        uint8_t green  = (attribute_byte_count >> 5) & 0x1F;
        uint8_t red    = (attribute_byte_count >> 10) & 0x1F;

        color[0] = red   / 31.0f;
        color[1] = green / 31.0f;
        color[2] = blue  / 31.0f;
    }
    else
    {
        color[0] = 0.0f;
        color[1] = 0.0f;
        color[2] = 0.0f;
    }
}

stl_model_t* stl_model_load_from_memory(allocator_t* allocator, void* data, size_t size)
{
	buffer_t* buffer = buffer_create(allocator);
	buffer_copy_data(buffer, data, size);

	buffer_set_offset(buffer, 80);//skip 80 byte header
	uint32_t num_triangles = buffer_read(buffer, uint32_t);
	if(num_triangles < 1) {ERROR("Invalid stl file! No triangles in file"); return NULL;}
	
	stl_model_t* stl_model = array_create(allocator, stl_vertex_t, 0, num_triangles*3);


	for(uint32_t i = 0; i < num_triangles; i++)
	{
		float normal[3] = { 0 };
		//normals
		normal[0] = buffer_read(buffer, float);
		normal[1] = buffer_read(buffer, float);
		normal[2] = buffer_read(buffer, float);
		
		stl_vertex_t vertex1 = { .normal[0] = normal[0], .normal[1] = normal[1], .normal[2] = normal[2]};
		stl_vertex_t vertex2 = { .normal[0] = normal[0], .normal[1] = normal[1], .normal[2] = normal[2]};
		stl_vertex_t vertex3 = { .normal[0] = normal[0], .normal[1] = normal[1], .normal[2] = normal[2]};

		vertex1.position[0] = buffer_read(buffer, float);
		vertex1.position[1] = buffer_read(buffer, float);
		vertex1.position[2] = buffer_read(buffer, float);

		vertex2.position[0] = buffer_read(buffer, float);
		vertex2.position[1] = buffer_read(buffer, float);
		vertex2.position[2] = buffer_read(buffer, float);

		vertex3.position[0] = buffer_read(buffer, float);
		vertex3.position[1] = buffer_read(buffer, float);
		vertex3.position[2] = buffer_read(buffer, float);

		float color[3] = { 0 };
		__get_color_from_attribute(buffer_read(buffer, uint16_t), color);

		vertex1.color[0] = color[0];
		vertex1.color[1] = color[1];
		vertex1.color[2] = color[2];

		vertex2.color[0] = color[0];
		vertex2.color[1] = color[1];
		vertex2.color[2] = color[2];

		vertex3.color[0] = color[0];
		vertex3.color[1] = color[1];
		vertex3.color[2] = color[2];
		
		array_push(stl_model, vertex1);
		array_push(stl_model, vertex2);
		array_push(stl_model, vertex3);
	}
	
	buffer_destroy(buffer);

	return stl_model;
}

void stl_model_destroy(stl_model_t* stl_model)
{
	array_destroy(stl_model);
}

