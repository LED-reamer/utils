#pragma once
#include "allocator.h"
#include "types.h"

typedef uint32_t shader_t;
typedef struct mesh_t mesh_t;
typedef struct texture_t texture_t;
typedef struct rendertarget_t rendertarget_t;


// -=INITIALISATION=-

void gl_init();
void gl_deinit();

// -=RENDER STATE=-

void gl_state_reset();
void gl_set_depth_test(bool active);

// -=CLEARING=-

void gl_clear(color_t color);//clear color/depth/stencil
void gl_clear_color(color_t color);
void gl_clear_depth();
void gl_clear_stencil();

// -=SHADER=-

shader_t shader_create(allocator_t* allocator, const char* vertex, const char* fragment, const char* geometry, const char* tess_control, const char* tess_eval, const char* compute);
void shader_destroy(shader_t shader);
void shader_bind(shader_t shader);
void shader_uniform_float32(shader_t shader, const char* uniform_name, float value);
void shader_uniform_int32(shader_t shader, const char* uniform_name, int32_t value);
void shader_uniform_uint32(shader_t shader, const char* uniform_name, uint32_t value);
void shader_uniform_vec2(shader_t shader, const char* uniform_name, vec2_t value);
void shader_uniform_vec3(shader_t shader, const char* uniform_name, vec3_t value);
void shader_uniform_vec4(shader_t shader, const char* uniform_name, vec4_t value);
void shader_uniform_mat4x4(shader_t shader, const char* uniform_name, mat4x4_t value);
void shader_uniform_float32_array(shader_t shader, const char* uniform_name, float* data_ptr, size_t count);
void shader_uniform_int32_array(shader_t shader, const char* uniform_name, int32_t* data_ptr, size_t count);

// -=MESH=-

typedef enum vertex_attribute_e vertex_attribute_e;
typedef enum draw_mode_e draw_mode_e;
mesh_t* mesh_create(allocator_t* allocator);
void mesh_destroy(mesh_t* mesh);
void mesh_set_attributes(mesh_t* mesh, vertex_attribute_e* vertex_attributes, size_t num_attributes);
void mesh_set_data_vertices(mesh_t* mesh, void* vertices, size_t num_vertices);
void mesh_set_data_indices(mesh_t* mesh, uint32_t* indices, size_t num_indices);
void mesh_draw(mesh_t* mesh, bool indexed, draw_mode_e draw_mode, bool instanced);

// -=TEXTURE=-

typedef enum texture_channels_e texture_channels_e;
texture_channels_e texture_get_channels_from_num(uint8_t num_channels);
typedef enum texture_filtering_e texture_filtering_e;
typedef enum texture_wrapping_e texture_wrapping_e;
texture_t* texture_create(allocator_t* allocator, uint32_t width, uint32_t height, texture_channels_e channels, uint8_t bits_per_channel);
void texture_destroy(texture_t* texture);
void texture_bind(texture_t* texture, uint32_t slot);
void texture_resize(texture_t* texture, uint32_t width, uint32_t height);
void texture_set_pixels(texture_t* texture, uint32_t width, uint32_t height, void* pixels);
void texture_get_pixels(texture_t* texture, void* pixels);
vec2_t texture_get_size(texture_t* texture);
uint32_t texture_get_width(texture_t* texture);
uint32_t texture_get_height(texture_t* texture);
void texture_filtering(texture_t* texture, texture_filtering_e filtering_magnifying, texture_filtering_e filtering_minifying);
void texture_wrapping(texture_t* texture, texture_wrapping_e wrapping_x, texture_wrapping_e wrapping_y);
uint32_t texture_get_num_channels(texture_t* texture);
uint8_t texture_get_bits_per_channel(texture_t* texture);

// -=RENDERTARGET=-

rendertarget_t* rendertarget_create(allocator_t* allocator, uint32_t width, uint32_t height, texture_channels_e channels, uint8_t bits_per_channel);
void rendertarget_destroy(rendertarget_t* rendertarget);
void rendertarget_bind(rendertarget_t* rendertarget);
void rendertarget_unbind(vec2_t new_viewport);
texture_t* rendertarget_get_texture(rendertarget_t* rendertarget);

// -=ENUMS=-

typedef enum vertex_attribute_e {
	VERTEX_ATTRIB_FLOAT1,
	VERTEX_ATTRIB_FLOAT2,
	VERTEX_ATTRIB_FLOAT3,
	VERTEX_ATTRIB_FLOAT4,
	VERTEX_ATTRIB_INT,
	VERTEX_ATTRIB_UINT,
	VERTEX_ATTRIB_FLOAT1_NORM,
	VERTEX_ATTRIB_FLOAT2_NORM,
	VERTEX_ATTRIB_FLOAT3_NORM,
	VERTEX_ATTRIB_FLOAT4_NORM,
	VERTEX_ATTRIB_INT_NORM,
	VERTEX_ATTRIB_UINT_NORM,
}vertex_attribute_e;

typedef enum draw_mode_e {
	DRAW_MODE_POINTS,
	DRAW_MODE_LINES,
	DRAW_MODE_LINE_STRIP,
	DRAW_MODE_LINE_LOOP,
	DRAW_MODE_TRIANGLES,
	DRAW_MODE_TRIANGLE_STRIP,
	DRAW_MODE_TRIANGLE_FAN,
}draw_mode_e;

typedef enum texture_channels_e {
	TEXTURE_CHANNELS_R,
	TEXTURE_CHANNELS_RG,
	TEXTURE_CHANNELS_RGB,
	TEXTURE_CHANNELS_RGBA,

	TEXTURE_CHANNELS_R8,
	TEXTURE_CHANNELS_RG8,
	TEXTURE_CHANNELS_RGB8,
	TEXTURE_CHANNELS_RGBA8,

	TEXTURE_CHANNELS_R16,
	TEXTURE_CHANNELS_RG16,
	TEXTURE_CHANNELS_RGB16,
	TEXTURE_CHANNELS_RGBA16,
}texture_channels_e;

typedef enum texture_filtering_e {
	TEXTURE_FILTERING_NEAREST,
	TEXTURE_FILTERING_LINEAR,
	TEXTURE_FILTERING_NEAREST_MIPMAP_NEAREST,
	TEXTURE_FILTERING_LINEAR_MIPMAP_NEAREST,
	TEXTURE_FILTERING_NEAREST_MIPMAP_LINEAR,
	TEXTURE_FILTERING_LINEAR_MIPMAP_LINEAR,
}texture_filtering_e;

typedef enum texture_wrapping_e {
	TEXTURE_WRAPPING_CLAMP_TO_EDGE,
	TEXTURE_WRAPPING_CLAMP_TO_BORDER,
	TEXTURE_WRAPPING_REPEAT,
	TEXTURE_WRAPPING_MIRRORED_REPEAT,
}texture_wrapping_e;
