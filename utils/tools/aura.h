#pragma once
#include "utils/types.h"

#define AURA_CORNER_VERTICES (16)
#define AURA_MAX_TEXTURES_PER_FRAME (64)

typedef struct{
	vec2_t position;
	color_t color;
	vec2_t uv;
}aura_vertex_t;

typedef struct{
	uint32_t width, height;
	void* sdl3_texture;//RGBA8888
}aura_texture_t;
typedef aura_texture_t aura_target_t;

typedef struct{
	void* sdl3_window;
	void* sdl3_renderer;
	aura_texture_t textures[AURA_MAX_TEXTURES_PER_FRAME];
	uint32_t num_textures;
}aura_context_t;

typedef enum{
	AURA_NEAREST,
	AURA_LINEAR,
	AURA_PIXELART
}aura_scale_mode_e;

typedef enum{
	AURA_NO_BLEND,
	AURA_BLEND,
	AURA_BLEND_PREMULTIPLIED,
	AURA_ADD,
	AURA_ADD_PREMULTIPLIED,
	AURA_MOD,
	AURA_MUL,
}aura_blend_mode_e;

// -- initialization
aura_context_t aura_init(void* sdl3_window);
void aura_deinit(aura_context_t* ctx);

// -- frame
void aura_render(aura_context_t* ctx);
void aura_clip(aura_context_t* ctx, rectangle_t rectangle);
void aura_clear(aura_context_t* ctx, color_t clear_color);

// -- texture loading
aura_texture_t* aura_add_texture(aura_context_t* ctx, uint32_t width, uint32_t height, void* pixels);//RGBA8888
aura_texture_t* aura_add_target(aura_context_t* ctx, uint32_t width, uint32_t height);//RGBA8888

void aura_set_texture_scale_mode(aura_texture_t* texture, aura_scale_mode_e scale_mode);
void aura_set_texture_blend_mode(aura_texture_t* texture, aura_blend_mode_e blend_mode);
void aura_set_target(aura_context_t* ctx, aura_target_t* target);

// -- drawing
void aura_debug_text(aura_context_t* ctx, const char* text, vec2_t position, color_t color);
void aura_debug_text_fmt(aura_context_t* ctx, vec2_t position, color_t color, const char* fmt, ...);
void aura_point(aura_context_t* ctx, vec2_t position, color_t color);
void aura_line(aura_context_t* ctx, vec2_t position_1, vec2_t position_2, color_t color);
void aura_rectangle(aura_context_t* ctx, rectangle_t rectangle, color_t color);
void aura_texture(aura_context_t* ctx, aura_texture_t* texture, rectangle_t dst);
void aura_texture_src(aura_context_t* ctx, aura_texture_t* texture, rectangle_t dst, rectangle_t src);
void aura_texture_sprite(aura_context_t* ctx, aura_texture_t* texture, rectangle_t dst, rectangle_t src, double degrees, vec2_t center, bool flip_vertical, bool flip_horizontal);
void aura_texture_grid(aura_context_t* ctx, aura_texture_t* texture, float left_width, float right_width, float top_height, float bottom_height, rectangle_t destination_rectangle);
void aura_geometry(aura_context_t* ctx, aura_texture_t* texture, aura_vertex_t* vertices, size_t num_vertices, uint32_t* indices, size_t num_indices);
void aura_rounded_rectangle(aura_context_t* ctx, rectangle_t rectangle, float radius, color_t color);
void aura_rounded_rectangle_corners(aura_context_t* ctx, rectangle_t rectangle, float radius_tl, float radius_tr, float radius_bl, float radius_br, color_t color);
