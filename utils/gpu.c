#include "gpu.h"

#include "logging.h"

#define __DEFAULT_TEXTURE_FORMAT SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM

// GPU-CONTEXT
gpu_context_t gpu_context_create(allocator_t* allocator, window_t* window) {
	gpu_context_t ctx = {0};
	ctx.allocator = allocator;
	ctx.window = window;

	// setup gpu device
	ctx.device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
	WARNING("SDL_CreateGPUDevice debug=true");

	if (ctx.device == NULL) {
		ERROR("SDL_CreateGPUDevice failed: %s\n", SDL_GetError());
		return (gpu_context_t){0};
	}

	if (!SDL_ClaimWindowForGPUDevice(ctx.device, window->SDL3_window)) {
		ERROR("SDL_ClaimWindowForGPUDevice failed: %s\n", SDL_GetError());
		return (gpu_context_t){0};
	}

	SDL_SetGPUSwapchainParameters(ctx.device, ctx.window->SDL3_window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);


	//init default samplers
	SDL_GPUSamplerCreateInfo sampler_create_info = (SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_LINEAR,
		.mag_filter = SDL_GPU_FILTER_LINEAR,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
	};
	ctx.linear_sampler = SDL_CreateGPUSampler(ctx.device, &sampler_create_info);
	sampler_create_info.min_filter = SDL_GPU_FILTER_NEAREST;
	sampler_create_info.mag_filter = SDL_GPU_FILTER_NEAREST;
	sampler_create_info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
	ctx.nearest_sampler = SDL_CreateGPUSampler(ctx.device, &sampler_create_info);

	ctx.depth_texture = texture_create(&ctx, 10, 10, TEXTURE_FORMAT_DEPTH_16BIT, TEXTURE_FILTERING_NEAREST);

	return ctx;
}

void gpu_context_destroy(gpu_context_t* gpu_context) {
	texture_destroy(&gpu_context->depth_texture);
	SDL_ReleaseGPUSampler(gpu_context->device, gpu_context->linear_sampler);
	SDL_ReleaseGPUSampler(gpu_context->device, gpu_context->nearest_sampler);
	
	SDL_ReleaseWindowFromGPUDevice(gpu_context->device, gpu_context->window->SDL3_window);
	SDL_DestroyGPUDevice(gpu_context->device);
}

void gpu_begin(gpu_context_t* ctx, texture_t* target, color_t clear_color) {
	ctx->cmd_buf = SDL_AcquireGPUCommandBuffer(ctx->device);
	uint32_t current_w, current_h;
	if (target == NULL) {  // get window texture
		SDL_WaitAndAcquireGPUSwapchainTexture(ctx->cmd_buf, ctx->window->SDL3_window, &ctx->current_render_target, &current_w, &current_h);
		if (ctx->current_render_target == NULL) {  // window may be minimized
			SDL_SubmitGPUCommandBuffer(ctx->cmd_buf);
			return;
		}
	} else {
		ctx->current_render_target = target->sdl_texture;
		current_w = target->w;
		current_h = target->h;
	}

	// resize depth texture if size changed
	if (current_w != ctx->current_render_target_width || current_h != ctx->current_render_target_height){
		//SDL_SubmitGPUCommandBuffer(ctx->cmd_buf);
		WARNING("Window resized to %u, %u + memory leak", current_w, current_h);
		//texture_destroy(&ctx->depth_texture);//TODO: this line causes a segfault
		ctx->depth_texture = texture_create(ctx, current_w, current_h, TEXTURE_FORMAT_DEPTH_16BIT, TEXTURE_FILTERING_NEAREST);
		//ctx->cmd_buf = SDL_AcquireGPUCommandBuffer(ctx->device);
	}
	ctx->current_render_target_width = current_w;
	ctx->current_render_target_height = current_h;

	SDL_GPUColorTargetInfo color_target_info = {
		.clear_color = (SDL_FColor){clear_color.r, clear_color.g, clear_color.b, clear_color.a},
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_STORE,
		.texture = ctx->current_render_target,
	};

	SDL_GPUDepthStencilTargetInfo depth_stencil_target_info = {
		.texture = ctx->depth_texture.sdl_texture,
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_DONT_CARE,
		.clear_depth = 1,
	};

	ctx->render_pass = SDL_BeginGPURenderPass(ctx->cmd_buf, &color_target_info, 1, &depth_stencil_target_info);
	SDL_SetGPUViewport(ctx->render_pass, &(SDL_GPUViewport){.w = ctx->current_render_target_width, .h = ctx->current_render_target_height});
}

void gpu_vertex_uniform(gpu_context_t* ctx, uint32_t slot, const void* data, size_t size) {
	SDL_PushGPUVertexUniformData(ctx->cmd_buf, slot, data, size);
}

void gpu_fragment_uniform(gpu_context_t* ctx, uint32_t slot, const void* data, size_t size) {
	SDL_PushGPUFragmentUniformData(ctx->cmd_buf, slot, data, size);
}

void gpu_fragment_samplers(gpu_context_t* ctx, texture_t* textures, size_t num_textures) {
	SDL_GPUTextureSamplerBinding* texture_sampler_bindings = ctx->allocator->amalloc(num_textures * sizeof(SDL_GPUTextureSamplerBinding));
	for(size_t i = 0; i < num_textures; i++){
		texture_sampler_bindings[i].texture = textures[i].sdl_texture;
		if(textures[i].filtering == TEXTURE_FILTERING_NEAREST)
			texture_sampler_bindings[i].sampler = ctx->nearest_sampler;
		else
			texture_sampler_bindings[i].sampler = ctx->linear_sampler;
	}
	
	SDL_BindGPUFragmentSamplers(ctx->render_pass, 0, texture_sampler_bindings, num_textures);
	
	ctx->allocator->afree(texture_sampler_bindings);
}

void gpu_draw(gpu_context_t* ctx, pipeline_t* pipeline, mesh_t* mesh, size_t first_vertex, size_t num_vertices) {
	if (ctx->cmd_buf == NULL || ctx->current_render_target == NULL || ctx->render_pass == NULL) return;

	SDL_BindGPUGraphicsPipeline(ctx->render_pass, pipeline->sdl_pipeline);

	SDL_GPUBufferBinding buffer_bindings[1];
	buffer_bindings[0].buffer = mesh->vertex_buffer;
	buffer_bindings[0].offset = 0;

	SDL_BindGPUVertexBuffers(ctx->render_pass, 0, buffer_bindings, 1);
	SDL_DrawGPUPrimitives(ctx->render_pass, num_vertices, 1, first_vertex, 0);
}

void gpu_draw_indexed(gpu_context_t* ctx, pipeline_t* pipeline, mesh_t* mesh, size_t first_index, size_t num_indecies) {
	if (ctx->cmd_buf == NULL || ctx->current_render_target == NULL || ctx->render_pass == NULL) return;

	SDL_BindGPUGraphicsPipeline(ctx->render_pass, pipeline->sdl_pipeline);

	SDL_GPUBufferBinding buffer_bindings[1];
	buffer_bindings[0].buffer = mesh->vertex_buffer;
	buffer_bindings[0].offset = 0;

	SDL_BindGPUVertexBuffers(ctx->render_pass, 0, buffer_bindings, 1);

	SDL_GPUIndexElementSize index_size;
	if (mesh->index_size == sizeof(uint16_t))
		index_size = SDL_GPU_INDEXELEMENTSIZE_16BIT;
	else if (mesh->index_size == sizeof(uint32_t))
		index_size = SDL_GPU_INDEXELEMENTSIZE_32BIT;
	else {
		ERROR("wrong index size! Should be either 16bit or 32bit. Was set to %zubit", mesh->index_size * 8);
		index_size = SDL_GPU_INDEXELEMENTSIZE_32BIT;  // defaults to 32bit so no memory overlaps
	}

	SDL_BindGPUIndexBuffer(ctx->render_pass, &(SDL_GPUBufferBinding){.buffer = mesh->index_buffer, .offset = 0}, index_size);

	SDL_DrawGPUIndexedPrimitives(ctx->render_pass, num_indecies, 1, first_index, 0, 0);
}

void gpu_end(gpu_context_t* ctx) {
	if (ctx->cmd_buf == NULL || ctx->current_render_target == NULL || ctx->render_pass == NULL) return;
	SDL_EndGPURenderPass(ctx->render_pass);
	SDL_SubmitGPUCommandBuffer(ctx->cmd_buf);
	ctx->render_pass = NULL;
	ctx->cmd_buf = NULL;
}

SDL_GPUShader* __load_shader(SDL_GPUDevice* device, uint8_t* source_code, size_t code_size, SDL_GPUShaderStage stage, Uint32 sampler_count, Uint32 uniform_buffer_count, Uint32 storage_buffer_count, Uint32 storage_texture_count) {
	const char* entrypoint;
	SDL_GPUShaderFormat backend_formats = SDL_GetGPUShaderFormats(device);
	SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
	if (backend_formats & SDL_GPU_SHADERFORMAT_SPIRV) {
		format = SDL_GPU_SHADERFORMAT_SPIRV;
		entrypoint = "main";
	} else {
		ERROR("shaderformat SPIRV not supported!");
		return NULL;
	}

	if (source_code == NULL) {
		ERROR("source_code was NULL");
		return NULL;
	}

	SDL_GPUShaderCreateInfo shader_info = {
		.code = (const uint8_t*)source_code,
		.code_size = code_size,
		.entrypoint = entrypoint,
		.format = format,
		.stage = stage,
		.num_samplers = sampler_count,
		.num_uniform_buffers = uniform_buffer_count,
		.num_storage_buffers = storage_buffer_count,
		.num_storage_textures = storage_texture_count};

	SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shader_info);

	if (shader == NULL) {
		ERROR("SDL_CreateGPUShader failed: %s\n", SDL_GetError());
		return NULL;
	}
	return shader;
}

shader_t shader_create(gpu_context_t* ctx, uint8_t* vert_spirv_code, size_t vert_spirv_size, uint32_t vert_num_samplers, uint32_t vert_num_storage_textures, uint32_t vert_num_storage_buffers, uint32_t vert_num_uniform_buffers, uint8_t* frag_spirv_code, size_t frag_spirv_size, uint32_t frag_num_samplers, uint32_t frag_num_storage_textures, uint32_t frag_num_storage_buffers, uint32_t frag_num_uniform_buffers) {
	shader_t shader = {0};
	shader.ctx = ctx;

	shader.vert = __load_shader(shader.ctx->device, vert_spirv_code, vert_spirv_size, SDL_GPU_SHADERSTAGE_VERTEX, vert_num_samplers, vert_num_uniform_buffers, vert_num_storage_buffers, vert_num_storage_textures);
	shader.frag = __load_shader(shader.ctx->device, frag_spirv_code, frag_spirv_size, SDL_GPU_SHADERSTAGE_FRAGMENT, frag_num_samplers, frag_num_uniform_buffers, frag_num_storage_buffers, frag_num_storage_textures);

	if (shader.vert == NULL || shader.frag == NULL) {
		ERROR("Could not create shader!");
		return (shader_t){0};
	}

	return shader;
}

void shader_destroy(shader_t* shader) {
	SDL_ReleaseGPUShader(shader->ctx->device, shader->vert);
	SDL_ReleaseGPUShader(shader->ctx->device, shader->frag);
}

static SDL_GPUTextureFormat texture_format_enum_mapping[] = {
    [TEXTURE_FORMAT_GREY_8BIT]           = SDL_GPU_TEXTUREFORMAT_R8_UNORM,
    [TEXTURE_FORMAT_RGBA_8BIT]           = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,

    [TEXTURE_FORMAT_GREY_FLOAT32]        = SDL_GPU_TEXTUREFORMAT_R32_FLOAT,
    [TEXTURE_FORMAT_RGBA_FLOAT32]        = SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT,

    [TEXTURE_FORMAT_DEPTH_16BIT]         = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
    [TEXTURE_FORMAT_DEPTH_24BIT]         = SDL_GPU_TEXTUREFORMAT_D24_UNORM,
    [TEXTURE_FORMAT_DEPTH_FLOAT32]       = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
};

texture_t texture_create(gpu_context_t* ctx, uint32_t width, uint32_t height, texture_format_e format, texture_filtering_e filtering) {
	SDL_GPUTextureFormat sdl_format = texture_format_enum_mapping[format];

	SDL_GPUTextureUsageFlags custom_usage = 0;
	if((format == TEXTURE_FORMAT_DEPTH_16BIT || format == TEXTURE_FORMAT_DEPTH_24BIT) || format == TEXTURE_FORMAT_DEPTH_FLOAT32){
		custom_usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
	}
	else custom_usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;

	SDL_PropertiesID empty_props = SDL_CreateProperties();
	SDL_GPUTextureCreateInfo texture_create_info = {
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = sdl_format,
		.usage = custom_usage,
		.width = width,
		.height = height,
		.layer_count_or_depth = 1,
		.num_levels = 1,
		.props = empty_props,	 // docs say this needs to be 0, but then theres an error: "Parameter 'src' is invalid" since it tries to copy props parameter and it's 0/NULL. See source code SDL_gpu_vulkan.c and SDL_properties.c. Don't know what '1' does
	};

	SDL_GPUColorComponentFlags color_write_mask = 0;
	bool needs_blending = false;
	if(format == TEXTURE_FORMAT_RGBA_8BIT || format == TEXTURE_FORMAT_RGBA_FLOAT32){
		color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;
		needs_blending = true;
	}
		

	SDL_GPUColorTargetBlendState blend_state = {
		.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
		.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		.color_blend_op = SDL_GPU_BLENDOP_ADD,
		.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
		.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
		.color_write_mask = color_write_mask,
		.enable_blend = needs_blending,
		.enable_color_write_mask = false,
	};

	texture_t texture = {
		.ctx = ctx,
		.sdl_texture = SDL_CreateGPUTexture(ctx->device, &texture_create_info),
		.w = width,
		.h = height,
		.format = sdl_format,
		.blend_state = blend_state,
		.filtering = filtering,
	};
	SDL_DestroyProperties(empty_props);

	return texture;
}

void texture_destroy(texture_t* texture) {
	SDL_ReleaseGPUTexture(texture->ctx->device, texture->sdl_texture);
}

void texture_upload(texture_t* texture, void* data, size_t size){
	SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(texture->ctx->device);
	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer);

	//transfer_buffer
	SDL_GPUTransferBufferCreateInfo transfer_buffer_create_info = (SDL_GPUTransferBufferCreateInfo){
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = size,
	};
	SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(texture->ctx->device, &transfer_buffer_create_info );
	//mapping and copying
	void* mapped_data = SDL_MapGPUTransferBuffer(texture->ctx->device, transfer_buffer, false);
	memcpy(mapped_data, data, size);
	SDL_UnmapGPUTransferBuffer(texture->ctx->device, transfer_buffer);

	//source
	SDL_GPUTextureTransferInfo source = (SDL_GPUTextureTransferInfo){
		.transfer_buffer = transfer_buffer,
		//.offset = 0,
		//.pixels_per_row = texture->w,
		//.rows_per_layer = 1,
	};

	//destination
	SDL_GPUTextureRegion destination = (SDL_GPUTextureRegion){
		.texture = texture->sdl_texture,
		.mip_level = 0,
		.layer = 0,
		.x = 0,
		.y = 0,
		.z = 0,
		.w = texture->w,
		.h = texture->h,
		.d = 1,
	};

	//upload
	SDL_UploadToGPUTexture(copy_pass, &source, &destination, false/*cylce*/);
	SDL_ReleaseGPUTransferBuffer(texture->ctx->device, transfer_buffer);
	SDL_EndGPUCopyPass(copy_pass);
	SDL_SubmitGPUCommandBuffer(command_buffer);
}

pipeline_t pipeline_create(gpu_context_t* ctx, shader_t* shader, size_t vertex_size, attribute_e vertex_attribs[], size_t num_attribs) {
	// - vertex layout
	SDL_GPUVertexAttribute* sdl_attribs = ctx->allocator->amalloc(sizeof(SDL_GPUVertexAttribute) * num_attribs);
	size_t offset = 0;
	for (size_t i = 0; i < num_attribs; i++) {
		sdl_attribs[i] = (SDL_GPUVertexAttribute){
			.location = i,
			.buffer_slot = 0,
			.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
			.offset = offset,
		};

		switch ((vertex_attribs[i])) {
			case ATTRIBUTE_FLOAT1:
				offset += sizeof(float) * 1;
				sdl_attribs[i].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT;
				break;
			case ATTRIBUTE_FLOAT2:
				offset += sizeof(float) * 2;
				sdl_attribs[i].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
				break;
			case ATTRIBUTE_FLOAT3:
				offset += sizeof(float) * 3;
				sdl_attribs[i].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
				break;
			case ATTRIBUTE_FLOAT4:
				offset += sizeof(float) * 4;
				sdl_attribs[i].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
				break;
			default:
				UNREACHABLE();
				break;
		}
	}

	SDL_GPUVertexInputState input_state = {
		.vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{.slot = 0,
																		  .pitch = vertex_size,
																		  .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
																		  .instance_step_rate = 0}},
		.num_vertex_buffers = 1,
		.vertex_attributes = sdl_attribs,
		.num_vertex_attributes = num_attribs,
	};

	// - target
	SDL_GPUGraphicsPipelineTargetInfo target_info = {
		.color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
			.format = __DEFAULT_TEXTURE_FORMAT,
			.blend_state = (SDL_GPUColorTargetBlendState){
				.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
				.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
				.color_blend_op = SDL_GPU_BLENDOP_ADD,

				.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
				.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
				.alpha_blend_op = SDL_GPU_BLENDOP_ADD,

				.color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A,

				.enable_blend = true,
				.enable_color_write_mask = true,
			},
		}},
		.num_color_targets = 1,
		.has_depth_stencil_target = true,
		.depth_stencil_format = ctx->depth_texture.format,
	};

	SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {
		.vertex_shader = shader->vert,
		.fragment_shader = shader->frag,
		.vertex_input_state = input_state,
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.target_info = target_info,
		.depth_stencil_state = {
			.enable_depth_test = true,
			.enable_depth_write = true,
			.compare_op = SDL_GPU_COMPAREOP_LESS,
		},
		.rasterizer_state = (SDL_GPURasterizerState){.cull_mode = SDL_GPU_CULLMODE_BACK, .fill_mode = SDL_GPU_FILLMODE_FILL, .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE},
	};

	pipeline_t pipeline = {
		.ctx = ctx,
		.sdl_pipeline = SDL_CreateGPUGraphicsPipeline(ctx->device, &pipeline_info),
	};
	ctx->allocator->afree(sdl_attribs);

	return pipeline;
}

void pipeline_destroy(pipeline_t* pipeline) {
	SDL_ReleaseGPUGraphicsPipeline(pipeline->ctx->device, pipeline->sdl_pipeline);
}

mesh_t mesh_create(gpu_context_t* ctx, size_t vertex_size, size_t max_num_vertices, size_t index_size, size_t max_num_indices) {
	mesh_t mesh = {0};
	mesh.ctx = ctx;
	mesh.vertex_size = vertex_size;
	mesh.max_num_vertices = max_num_vertices;
	mesh.index_size = index_size;
	mesh.max_num_indices = max_num_indices;

	// create vertex/index buffers
	SDL_GPUBufferCreateInfo buffer_info = {0};

	buffer_info.size = vertex_size * max_num_vertices;
	buffer_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
	mesh.vertex_buffer = SDL_CreateGPUBuffer(ctx->device, &buffer_info);

	if (index_size != 0 && max_num_vertices > 0) {
		buffer_info.size = index_size * max_num_indices;
		buffer_info.usage = SDL_GPU_BUFFERUSAGE_INDEX;
		mesh.index_buffer = SDL_CreateGPUBuffer(ctx->device, &buffer_info);
	}

	// create transferbuffers
	SDL_GPUTransferBufferCreateInfo transfer_info = {0};
	transfer_info.size = vertex_size * max_num_vertices;
	transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	mesh.vertex_transfer_buffer = SDL_CreateGPUTransferBuffer(ctx->device, &transfer_info);

	if (index_size != 0 && max_num_vertices > 0) {
		transfer_info.size = index_size * max_num_indices;
		mesh.index_transfer_buffer = SDL_CreateGPUTransferBuffer(ctx->device, &transfer_info);
	}

	return mesh;
}

void mesh_destroy(mesh_t* mesh) {
	SDL_ReleaseGPUTransferBuffer(mesh->ctx->device, mesh->vertex_transfer_buffer);
	SDL_ReleaseGPUTransferBuffer(mesh->ctx->device, mesh->index_transfer_buffer);
	SDL_ReleaseGPUBuffer(mesh->ctx->device, mesh->vertex_buffer);
	SDL_ReleaseGPUBuffer(mesh->ctx->device, mesh->index_buffer);
}

// TODO find out what happens if not full capacity is uploaded. Is the rest zeros? Or is old vertex data rendered?
void mesh_upload(mesh_t* mesh, void* vertices, size_t num_vertices, void* indices, size_t num_indices) {
	// copying
	void* mapped_vertices = SDL_MapGPUTransferBuffer(mesh->ctx->device, mesh->vertex_transfer_buffer, false);
	memcpy(mapped_vertices, vertices, mesh->vertex_size * num_vertices);
	SDL_UnmapGPUTransferBuffer(mesh->ctx->device, mesh->vertex_transfer_buffer);

	if (indices != NULL && num_indices > 0) {
		void* mapped_indices = SDL_MapGPUTransferBuffer(mesh->ctx->device, mesh->index_transfer_buffer, false);
		memcpy(mapped_indices, indices, mesh->index_size * num_indices);
		SDL_UnmapGPUTransferBuffer(mesh->ctx->device, mesh->index_transfer_buffer);
	}

	// copy pass
	SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(mesh->ctx->device);
	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer);

	// vertex upload
	SDL_GPUTransferBufferLocation location = {0};
	location.transfer_buffer = mesh->vertex_transfer_buffer;
	location.offset = 0;
	SDL_GPUBufferRegion region = {0};
	region.buffer = mesh->vertex_buffer;
	region.size = mesh->vertex_size * num_vertices;
	region.offset = 0;
	SDL_UploadToGPUBuffer(copy_pass, &location, &region, true);

	// index upload
	if (indices != NULL && num_indices > 0) {
		location = (SDL_GPUTransferBufferLocation){0};
		location.transfer_buffer = mesh->index_transfer_buffer;
		location.offset = 0;
		region = (SDL_GPUBufferRegion){0};
		region.buffer = mesh->index_buffer;
		region.size = mesh->index_size * num_indices;
		region.offset = 0;
		SDL_UploadToGPUBuffer(copy_pass, &location, &region, true);
	}

	// end copy pass
	SDL_EndGPUCopyPass(copy_pass);
	SDL_SubmitGPUCommandBuffer(command_buffer);
}
