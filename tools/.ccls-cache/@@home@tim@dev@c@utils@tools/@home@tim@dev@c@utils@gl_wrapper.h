//link with -lcglm -lGLEW -lGL -lm
//works at least on linux, *should* work on windows too

//includes:
//	logging.h
//	allocator.h

#pragma once
#include "allocator.h"
#include <GL/glew.h>
#include <cglm/cglm.h>
#include <stdint.h>

typedef uint32_t shader_t;

shader_t shader_create(allocator_t* allocator, const char* vertex, const char* fragment, const char* geometry, const char* tess_control, const char* tess_eval, const char* compute);
void shader_destroy(shader_t shader);
void shader_bind(shader_t shader);

void shader_uniform_float32(shader_t shader, const char* uniform_name, float value);
void shader_uniform_int32(shader_t shader, const char* uniform_name, int32_t value);
void shader_uniform_uint32(shader_t shader, const char* uniform_name, uint32_t value);
void shader_uniform_vec2(shader_t shader, const char* uniform_name, vec2 value);
void shader_uniform_vec3(shader_t shader, const char* uniform_name, vec3 value);
void shader_uniform_vec4(shader_t shader, const char* uniform_name, vec4 value);
void shader_uniform_mat4(shader_t shader, const char* uniform_name, mat4 value);
void shader_uniform_float32_array(shader_t shader, const char* uniform_name, float* data_ptr, size_t count);
void shader_uniform_int32_array(shader_t shader, const char* uniform_name, int32_t* data_ptr, size_t count);



typedef struct
{
	allocator_t* allocator;
	uint32_t gl_vertex_array;
	uint32_t gl_vertex_buffer;
	uint32_t gl_index_buffer;
		
	size_t vertex_size;
	uint32_t num_vertices;
	uint32_t num_indices;
}mesh_t;

typedef enum
{
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

typedef enum
{
	DRAW_MODE_POINTS = GL_POINTS,
	DRAW_MODE_LINES = GL_LINES,
	DRAW_MODE_LINE_STRIP = GL_LINE_STRIP,
	DRAW_MODE_LINE_LOOP = GL_LINE_LOOP,
	DRAW_MODE_TRIANGLES = GL_TRIANGLES,
	DRAW_MODE_TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
	DRAW_MODE_TRIANGLE_FAN = GL_TRIANGLE_FAN,
}draw_mode_e;//TODO currently not all. see changes in 3.3 and 4.5 for example

mesh_t* mesh_create(allocator_t* allocator);
void mesh_destroy(mesh_t* mesh);
void mesh_set_attributes(mesh_t* mesh, vertex_attribute_e* vertex_attributes, size_t num_attributes);
void mesh_set_data_vertices(mesh_t* mesh, void* vertices, size_t num_vertices);
void mesh_set_data_indices(mesh_t* mesh, uint32_t* indices, size_t num_indices);
void mesh_draw(mesh_t* mesh, uint8_t indexed, draw_mode_e draw_mode, uint8_t instanced);



typedef enum
{
	TEXTURE_CHANNELS_R = GL_RED,
	TEXTURE_CHANNELS_RG = GL_RG,
	TEXTURE_CHANNELS_RGB = GL_RGB,
	TEXTURE_CHANNELS_RGBA = GL_RGBA,
	
	TEXTURE_CHANNELS_R8 = GL_R8,
	TEXTURE_CHANNELS_RG8 = GL_RG8,
	TEXTURE_CHANNELS_RGB8 = GL_RGB8,
	TEXTURE_CHANNELS_RGBA8 = GL_RGBA8,

	TEXTURE_CHANNELS_R16 = GL_R16,
	TEXTURE_CHANNELS_RG16 = GL_RG16,
	TEXTURE_CHANNELS_RGB16 = GL_RGB16,
	TEXTURE_CHANNELS_RGBA16 = GL_RGBA16,
}texture_channels_e;
texture_channels_e texture_get_channels_from_num(uint8_t num_channels);

typedef enum
{
	TEXTURE_FILTERING_NEAREST = GL_NEAREST,
	TEXTURE_FILTERING_LINEAR = GL_LINEAR,
	TEXTURE_FILTERING_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	TEXTURE_FILTERING_LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
	TEXTURE_FILTERING_NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
	TEXTURE_FILTERING_LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
}texture_filtering_e;

typedef enum
{
	TEXTURE_WRAPPING_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
	TEXTURE_WRAPPING_CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
	TEXTURE_WRAPPING_REPEAT = GL_REPEAT,
	TEXTURE_WRAPPING_MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
}texture_wrapping_e;

typedef struct
{
	allocator_t* allocator;
	uint32_t gl_texture;
	uint32_t width, height;
	texture_channels_e channels;
	uint8_t bits_per_channel;
}texture_t;

texture_t* texture_create(allocator_t* allocator, uint32_t width, uint32_t height, texture_channels_e channels, uint8_t bits_per_channel);
void texture_destroy(texture_t* texture);
void texture_bind(texture_t* texture, uint32_t slot);
void texture_resize(texture_t* texture, uint32_t width, uint32_t height);
void texture_set_pixels(texture_t* texture, uint32_t width, uint32_t height, void* pixels);
void texture_get_pixels(texture_t* texture, void* pixels);
void texture_filtering(texture_t* texture, texture_filtering_e filtering_magnifying, texture_filtering_e filtering_minifying);
void texture_wrapping(texture_t* texture, texture_wrapping_e wrapping_x, texture_wrapping_e wrapping_y);
uint32_t texture_get_num_channels(texture_t* texture);

typedef struct
{
	allocator_t* allocator;
	uint32_t gl_framebuffer;
	texture_t* texture;
}render_target_t;

render_target_t* render_target_create(allocator_t* allocator, uint32_t width, uint32_t height, texture_channels_e channels, uint8_t bits_per_channel);
void render_target_destroy(render_target_t* render_target);
void render_target_bind(render_target_t* render_target);
void render_target_unbind(vec2 new_viewport);
texture_t* render_target_get_texture(render_target_t* render_target);


//see https://docs.gl/gl4/glEnable and https://docs.gl/gl4/glGet
typedef enum
{
	GL_CAP_BLEND = GL_BLEND,
	//GL_CAP_CLIP_DISTANCEi = GL_CLIP_DISTANCEi, //i one for each number but what is max??
	GL_CAP_COLOR_LOGIC_OP = GL_COLOR_LOGIC_OP,
	GL_CAP_CULL_FACE = GL_CULL_FACE,
	GL_CAP_DEBUG_OUTPUT = GL_DEBUG_OUTPUT, //GL 4.3
	GL_CAP_DEBUG_OUTPUT_SYNCHRONOUS = GL_DEBUG_OUTPUT_SYNCHRONOUS, //GL 4.3
	GL_CAP_DEPTH_CLAMP = GL_DEPTH_CLAMP,
	GL_CAP_DEPTH_TEST = GL_DEPTH_TEST,
	GL_CAP_DITHER = GL_DITHER,
	GL_CAP_FRAMEBUFFER_SRGB = GL_FRAMEBUFFER_SRGB,
	GL_CAP_LINE_SMOOTH = GL_LINE_SMOOTH,
	GL_CAP_MULTISAMPLE = GL_MULTISAMPLE,
	GL_CAP_POLYGON_OFFSET_FILL = GL_POLYGON_OFFSET_FILL,
	GL_CAP_POLYGON_OFFSET_LINE = GL_POLYGON_OFFSET_LINE,
	GL_CAP_POLYGON_OFFSET_POINT = GL_POLYGON_OFFSET_POINT,
	GL_CAP_POLYGON_SMOOTH = GL_POLYGON_SMOOTH,
	GL_CAP_PRIMITIVE_RESTART = GL_PRIMITIVE_RESTART, //GL 3.1
	GL_CAP_PRIMITIVE_RESTART_FIXED_INDEX = GL_PRIMITIVE_RESTART_FIXED_INDEX, //GL 3.3
	GL_CAP_RASTERIZER_DISCARD = GL_RASTERIZER_DISCARD,
	GL_CAP_SAMPLE_ALPHA_TO_COVERAGE = GL_SAMPLE_ALPHA_TO_COVERAGE,
	GL_CAP_SAMPLE_ALPHA_TO_ONE = GL_SAMPLE_ALPHA_TO_ONE,
	GL_CAP_SAMPLE_COVERAGE = GL_SAMPLE_COVERAGE,
	GL_CAP_SAMPLE_SHADING = GL_SAMPLE_SHADING,
	GL_CAP_SAMPLE_MASK = GL_SAMPLE_MASK,
	GL_CAP_SCISSOR_TEST = GL_SCISSOR_TEST,
	GL_CAP_STENCIL_TEST = GL_STENCIL_TEST,
	GL_CAP_TEXTURE_CUBE_MAP_SEAMLESS = GL_TEXTURE_CUBE_MAP_SEAMLESS, //GL 3.2
	GL_CAP_PROGRAM_POINT_SIZE = GL_PROGRAM_POINT_SIZE,

	__GL_CAP_COUNT
	
}gl_cap_e;

typedef enum
{
	GL_BLEND_EQ_FUNC_ADD = GL_FUNC_ADD, 
	GL_BLEND_EQ_FUNC_SUBTRACT = GL_FUNC_SUBTRACT, 
	GL_BLEND_EQ_FUNC_REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT, 
	GL_BLEND_EQ_MIN = GL_MIN, 
	GL_BLEND_EQ_MAX = GL_MAX,
}gl_blend_equation_e;

typedef enum
{
	GL_BLEND_FUNC_ZERO						= GL_ZERO,
	GL_BLEND_FUNC_ONE						= GL_ONE,
	GL_BLEND_FUNC_SRC_COLOR					= GL_SRC_COLOR,
	GL_BLEND_FUNC_ONE_MINUS_SRC_COLOR		= GL_ONE_MINUS_SRC_COLOR,
	GL_BLEND_FUNC_DST_COLOR					= GL_DST_COLOR,
	GL_BLEND_FUNC_ONE_MINUS_DST_COLOR		= GL_ONE_MINUS_DST_COLOR,
	GL_BLEND_FUNC_SRC_ALPHA					= GL_SRC_ALPHA,
	GL_BLEND_FUNC_ONE_MINUS_SRC_ALPHA		= GL_ONE_MINUS_SRC_ALPHA,
	GL_BLEND_FUNC_DST_ALPHA					= GL_DST_ALPHA,
	GL_BLEND_FUNC_ONE_MINUS_DST_ALPHA		= GL_ONE_MINUS_DST_ALPHA,
	GL_BLEND_FUNC_CONSTANT_COLOR			= GL_CONSTANT_COLOR,
	GL_BLEND_FUNC_ONE_MINUS_CONSTANT_COLOR	= GL_ONE_MINUS_CONSTANT_COLOR,
	GL_BLEND_FUNC_CONSTANT_ALPHA			= GL_CONSTANT_ALPHA,
	GL_BLEND_FUNC_ONE_MINUS_CONSTANT_ALPHA	= GL_ONE_MINUS_CONSTANT_ALPHA,
	GL_BLEND_FUNC_SRC_ALPHA_SATURATE		= GL_SRC_ALPHA_SATURATE,
	GL_BLEND_FUNC_SRC1_COLOR				= GL_SRC1_COLOR,
	GL_BLEND_FUNC_ONE_MINUS_SRC1_COLOR		= GL_ONE_MINUS_SRC_COLOR,
	GL_BLEND_FUNC_SRC1_ALPHA				= GL_SRC1_ALPHA,
	GL_BLEND_FUNC_ONE_MINUS_SRC1_ALPHA		= GL_ONE_MINUS_SRC_ALPHA,
}gl_blend_func_e;

typedef enum
{
	GL_CULL_FACE_FRONT = GL_FRONT,
	GL_CULL_FACE_BACK = GL_BACK,
	GL_CULL_FACE_FRONT_AND_BACK = GL_FRONT_AND_BACK,
}gl_cull_face_e;

typedef enum
{
	GL_DEPTH_FUNC_NEVER			= GL_NEVER,
	GL_DEPTH_FUNC_LESS			= GL_LESS,
	GL_DEPTH_FUNC_EQUAL			= GL_EQUAL,
	GL_DEPTH_FUNC_LEQUAL		= GL_LEQUAL,
	GL_DEPTH_FUNC_GREATER		= GL_GREATER,
	GL_DEPTH_FUNC_NOTEQUAL		= GL_NOTEQUAL,
	GL_DEPTH_FUNC_GEQUAL		= GL_GEQUAL,
	GL_DEPTH_FUNC_ALWAYS		= GL_ALWAYS,
}gl_depth_func_e;

typedef enum
{
	GL_HINT_LINE_SMOOTH = GL_LINE_SMOOTH_HINT,
	GL_HINT_POLYGON_SMOOTH = GL_POLYGON_SMOOTH_HINT,
	GL_HINT_TEXTURE_COMPRESSION = GL_TEXTURE_COMPRESSION_HINT,
	GL_HINT_FRAGMENT_SHADER_DERIVATIVE = GL_FRAGMENT_SHADER_DERIVATIVE_HINT,

	__GL_HINT_COUNT
}gl_hint_e;

typedef enum
{
	GL_LOGIC_OP_CLEAR = GL_CLEAR,
	GL_LOGIC_OP_SET = GL_SET,
	GL_LOGIC_OP_COPY = GL_COPY,
	GL_LOGIC_OP_COPY_INVERTED = GL_COPY_INVERTED,
	GL_LOGIC_OP_NOOP = GL_NOOP,
	GL_LOGIC_OP_INVERT = GL_INVERT,
	GL_LOGIC_OP_AND = GL_AND,
	GL_LOGIC_OP_NAND = GL_NAND,
	GL_LOGIC_OP_OR = GL_OR,
	GL_LOGIC_OP_NOR = GL_NOR,
	GL_LOGIC_OP_XOR = GL_XOR,
	GL_LOGIC_OP_EQUIV = GL_EQUIV,
	GL_LOGIC_OP_AND_REVERSE = GL_AND_REVERSE,
	GL_LOGIC_OP_AND_INVERTED = GL_AND_INVERTED,
	GL_LOGIC_OP_OR_REVERSE = GL_OR_REVERSE,
	GL_LOGIC_OP_OR_INVERTED = GL_OR_INVERTED,
}gl_logic_op_e;

typedef enum
{
	GL_POLYGON_MODE_POINT = GL_POINT,
	GL_POLYGON_MODE_LINE = GL_LINE,
	GL_POLYGON_MODE_FILL = GL_FILL,
}gl_polygon_mode_e;

typedef enum
{
	GL_STENCIL_FUNC_NEVER		= GL_NEVER,
	GL_STENCIL_FUNC_LESS		= GL_LESS,
	GL_STENCIL_FUNC_EQUAL		= GL_EQUAL,
	GL_STENCIL_FUNC_LEQUAL		= GL_LEQUAL,
	GL_STENCIL_FUNC_GREATER		= GL_GREATER,
	GL_STENCIL_FUNC_NOTEQUAL	= GL_NOTEQUAL,
	GL_STENCIL_FUNC_GEQUAL		= GL_GEQUAL,
	GL_STENCIL_FUNC_ALWAYS		= GL_ALWAYS,
}gl_stencil_func_e;

typedef enum
{
	GL_STENCIL_OP_KEEP = GL_KEEP,
	GL_STENCIL_OP_ZERO = GL_ZERO,
	GL_STENCIL_OP_REPLACE = GL_REPLACE,
	GL_STENCIL_OP_INCR = GL_INCR,
	GL_STENCIL_OP_INCR_WRAP = GL_INCR_WRAP,
	GL_STENCIL_OP_DECR = GL_DECR,
	GL_STENCIL_OP_DECR_WRAP = GL_DECR_WRAP,
	GL_STENCIL_OP_INVERT = GL_INVERT,
}gl_stencil_op_e;

typedef struct
{
	//caps
	uint8_t caps[__GL_CAP_COUNT];

	//blending
	vec4 blend_color;
	gl_blend_equation_e blend_equation_rgb;
	gl_blend_equation_e blend_equation_alpha;
	gl_blend_func_e blend_func_rgb_src;
	gl_blend_func_e blend_func_rgb_dst;
	gl_blend_func_e blend_func_alpha_src;
	gl_blend_func_e blend_func_alpha_dst;

	uint8_t clamp_read_color;
	
	//no clip control since its only GL 4.5
	
	uint8_t color_mask[4];//bool for each channel rgba
	gl_cull_face_e cull_face;
	gl_depth_func_e depth_func;
	uint8_t depth_mask;
	float depth_range[2];// [0] -> near, [1] -> far
	uint8_t front_face_ccw;
	uint8_t hints[__GL_HINT_COUNT];//0 -> fastest, 1 nicest, 2 don't care
	float line_width;
	gl_logic_op_e logic_op;
	
	//no pixel store since -> add only when needed

	float point_parameter_fade_threshold_size;
	uint8_t point_parameter_sprite_coord_origin; //0 -> top left, 1 bottom left
	float point_size;

	gl_polygon_mode_e polygon_mode;

	float polygon_offset_factor;
	float polygon_offset_units;

	float sample_coverage_value;
	uint8_t sample_coverage_invert;

	int32_t scissor_x;
	int32_t scissor_y;
	size_t scissor_width;
	size_t scissor_height;

	gl_stencil_func_e stencil_func;
	int32_t stencil_func_ref;
	uint32_t stencil_func_mask;
	uint32_t stencil_mask;
	gl_stencil_op_e stencil_op_sfail;
	gl_stencil_op_e stencil_op_dpail;
	gl_stencil_op_e stencil_op_dppass;

	int32_t viewport_x;
	int32_t viewport_y;
	size_t viewport_width;
	size_t viewport_height;
}gl_render_state_t;

gl_render_state_t* gl_state();
void gl_render_state_reset();
void gl_render_state_apply();
