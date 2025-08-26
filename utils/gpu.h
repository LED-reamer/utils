#pragma once
#include <SDL3/SDL_gpu.h>

#include "allocator.h"
#include "types.h"
#include "window.h"

// -= FORWARD DECLARATIONS =-
typedef struct gpu_context_t gpu_context_t;
typedef struct shader_t shader_t;
typedef struct pipeline_t pipeline_t;
typedef struct texture_t texture_t;
typedef struct mesh_t mesh_t;

// -= CONTEXT =-

gpu_context_t gpu_context_create(allocator_t* allocator, window_t* window);
void gpu_context_destroy(gpu_context_t* gpu_context);

// -= RENDER_PASS =-

void gpu_begin(gpu_context_t* ctx, texture_t* target /*NULL for window texture*/, color_t clear_color);
void gpu_vertex_uniform(gpu_context_t* ctx, uint32_t slot, const void* data, size_t size);
void gpu_fragment_uniform(gpu_context_t* ctx, uint32_t slot, const void* data, size_t size);
void gpu_fragment_samplers(gpu_context_t* ctx, texture_t* textures, size_t num_textures);
void gpu_draw(gpu_context_t* ctx, pipeline_t* pipeline, mesh_t* mesh, size_t first_vertex, size_t num_vertices);
void gpu_draw_indexed(gpu_context_t* ctx, pipeline_t* pipeline, mesh_t* mesh, size_t first_index, size_t num_indecies);
void gpu_end(gpu_context_t* ctx);

// -= SHADER =-

shader_t shader_create(gpu_context_t* ctx, uint8_t* vert_spirv_code, size_t vert_spirv_size, uint32_t vert_num_samplers, uint32_t vert_num_storage_textures, uint32_t vert_num_storage_buffers, uint32_t vert_num_uniform_buffers, uint8_t* frag_spirv_code, size_t frag_spirv_size, uint32_t frag_num_samplers, uint32_t frag_num_storage_textures, uint32_t frag_num_storage_buffers, uint32_t frag_num_uniform_buffers);
void shader_destroy(shader_t* shader);

// -= PIPELINE =-

typedef enum {
	ATTRIBUTE_FLOAT1,
	ATTRIBUTE_FLOAT2,
	ATTRIBUTE_FLOAT3,
	ATTRIBUTE_FLOAT4,
} attribute_e;

pipeline_t pipeline_create(gpu_context_t* ctx, shader_t* shader, size_t vertex_size, attribute_e vertex_attribs[], size_t num_attribs);
void pipeline_destroy(pipeline_t* pipeline);

// -= TEXTURE =-

typedef enum {
	TEXTURE_FORMAT_GREY_8BIT,
	TEXTURE_FORMAT_RGBA_8BIT,
	
	TEXTURE_FORMAT_GREY_FLOAT32,
	TEXTURE_FORMAT_RGBA_FLOAT32,

	TEXTURE_FORMAT_DEPTH_16BIT,
	TEXTURE_FORMAT_DEPTH_24BIT,
	TEXTURE_FORMAT_DEPTH_FLOAT32,
} texture_format_e;

typedef enum {
	TEXTURE_FILTERING_NEAREST,
	TEXTURE_FILTERING_LINEAR,
} texture_filtering_e;

texture_t texture_create(gpu_context_t* ctx, uint32_t width, uint32_t height, texture_format_e format, texture_filtering_e filtering);
void texture_destroy(texture_t* texture);
void texture_upload(texture_t* texture, void* data, size_t size);

// -= MESH =-

mesh_t mesh_create(gpu_context_t* ctx, size_t vertex_size, size_t max_num_vertices, size_t index_size, size_t max_num_indices);
void mesh_destroy(mesh_t* mesh);
void mesh_upload(mesh_t* mesh, void* vertices, size_t num_vertices, void* indices, size_t num_indices);

// -= STRUCT IMPLEMENTATIONS =-

struct texture_t{
	gpu_context_t* ctx;
	SDL_GPUTexture* sdl_texture;

	uint32_t w, h;

	SDL_GPUTextureFormat format;
	SDL_GPUColorTargetBlendState blend_state;
	texture_filtering_e filtering;
};

struct gpu_context_t{
	allocator_t* allocator;
	window_t* window;
	SDL_GPUDevice* device;
	SDL_GPUCommandBuffer* cmd_buf;
	SDL_GPURenderPass* render_pass;
	SDL_GPUTexture* current_render_target;
	texture_t depth_texture;
	uint32_t current_render_target_width, current_render_target_height;

	//both are repeating
	SDL_GPUSampler* linear_sampler;
	SDL_GPUSampler* nearest_sampler;
};

struct shader_t {
	gpu_context_t* ctx;
	SDL_GPUShader* vert;
	SDL_GPUShader* frag;
};

struct pipeline_t {
	gpu_context_t* ctx;
	SDL_GPUGraphicsPipeline* sdl_pipeline;
};

struct mesh_t {
	gpu_context_t* ctx;
	SDL_GPUBuffer* vertex_buffer;
	SDL_GPUBuffer* index_buffer;
	size_t vertex_size;
	size_t max_num_vertices;
	size_t index_size;
	size_t max_num_indices;

	SDL_GPUTransferBuffer* vertex_transfer_buffer;
	SDL_GPUTransferBuffer* index_transfer_buffer;
};
