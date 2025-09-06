#pragma once
#include "../window.h"
#include "assets.h"
#include "aura.h"
#include "ecs.h"


#include <stdbool.h>
#include "../allocator.h"
#include "../types.h"

typedef struct{
	allocator_t* allocator;
	window_t window;
	asset_manager_t asset_manager;
	ecs_t ecs;
	aura_context_t aura;

	r128 last_time;
	float dt;
	bool had_first_frame;

	// -- sprite renderer --
	vec2_t camera_target;
	hashmap_t atlas_images;//key: name_string_to_hash, value: aura_texture_t*
}engine_t;

typedef struct {
	allocator_t* allocator;
	const char* window_title;
	u32 window_width, window_height;
	u32 logical_width, logical_height;
	const char* archive_path;//either .zip or NULL
	const char* archive_pass;//may be NULL
}engine_args_t;
engine_args_t engine_args_standard();

engine_t* engine_create(engine_args_t args);
void engine_destroy(engine_t* engine);
bool engine_running(engine_t* engine);

// -- sprite engine --
void engine_set_camera_position(engine_t* engine, vec2_t target);
vec2_t engine_get_camera_position(engine_t* engine);
void engine_load_atlas(engine_t* engine, const char* filename, const char* name);// only RGBA8888
void engine_draw_sprite(engine_t* engine,
						const char* atlas_name, 
						rectangle_t dst, 
						rectangle_t src, 
						double degrees, 
						vec2_t center, 
						bool flip_vertical, 
						bool flip_horizontal,
						color_t tint);
