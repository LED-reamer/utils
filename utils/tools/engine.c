#include "engine.h"
#include "../logging.h"
#include "../random.h"
#include "../hashmap.h"
#include "../image.h"

engine_args_t engine_args_standard(){
	engine_args_t args = {
		.allocator = allocator_get_default(),
		.window_title = "window",
		.window_width = 1920,
		.window_height = 1080,
		.logical_width = 1920,
		.logical_height = 1080,
		.archive_path = NULL,
		.archive_pass = NULL,
	};

	return args;
}

engine_t* engine_create(engine_args_t args){
	engine_t* engine = args.allocator->amalloc(sizeof(engine_t));
	engine->allocator = args.allocator;
	engine->window = window_create(args.window_title, args.window_width, args.window_height);
	engine->asset_manager = asset_manager_create(engine->allocator, args.archive_path, args.archive_pass);
	engine->ecs = ecs_create(engine->allocator);
	engine->aura = aura_init(engine->window.SDL3_window);
	engine->camera_target = vec2(0, 0);
	engine->atlas_images = hashmap_create(engine->allocator);
	engine->last_time = window_get_time_s();
	aura_set_logical_presentation(&engine->aura, args.logical_width, args.logical_height, AURA_LETTERBOX);
	
	return engine;
}

void engine_destroy(engine_t* engine){
	ecs_shutdown(&engine->ecs);
	hashmap_destroy(&engine->atlas_images);
	ecs_destroy(&engine->ecs);
	aura_deinit(&engine->aura);
	window_destroy(&engine->window);
	asset_manager_destroy(&engine->asset_manager);
	engine->allocator->afree(engine);
}

bool engine_running(engine_t* engine){
	if(!engine->had_first_frame){
		ecs_start(&engine->ecs);
		engine->had_first_frame = true;
	}

	r128 now = window_get_time_s();
	engine->dt = (float)(now - engine->last_time);
	engine->last_time = now;
	
	ecs_update(&engine->ecs);
	ecs_frame(&engine->ecs);
	aura_render(&engine->aura);
	window_update(&engine->window);

	if(window_open(&engine->window))
		return true;
	return false;
}

void engine_set_camera_position(engine_t* engine, vec2_t target){
	engine->camera_target = target;
}

vec2_t engine_get_camera_position(engine_t* engine){
	return engine->camera_target;
}

uint64_t __name_to_key(const char* name){
	return random_hash_from_string(name);
}

void engine_load_atlas(engine_t* engine, const char* filename, const char* name){
	uint64_t key = __name_to_key(name);
	if (hashmap_contains_key(&engine->atlas_images, key)){
		ERROR("sprite_engine: \"%s\" already loaded", name);
		return;
	}

	buffer_t image_buffer = buffer_create(engine->allocator);
	asset_manager_load_file(&engine->asset_manager, &image_buffer, filename);
	image_t atlas = image_create_from_memory(engine->allocator, image_buffer.data, image_buffer.size, false, IMAGE_RGBA, IMAGE_8BIT);
	buffer_destroy(&image_buffer);
	
	image_reverse_channels(&atlas);
	aura_texture_t* texture = aura_add_texture(&engine->aura, atlas.width, atlas.height, atlas.pixels);
	aura_set_texture_scale_mode(texture, AURA_NEAREST);
	aura_set_texture_blend_mode(texture, AURA_BLEND);
	image_destroy(&atlas);

	hashmap_set(&engine->atlas_images, key, texture);
}

void engine_draw_sprite(engine_t* engine, const char* atlas_name, rectangle_t dst, rectangle_t src, double degrees, vec2_t center, bool flip_vertical, bool flip_horizontal, color_t tint){
	uint64_t key = __name_to_key(atlas_name);
	aura_texture_t* texture = (aura_texture_t*)hashmap_get(&engine->atlas_images, key);

	if(texture == NULL){
		ERROR("sprite_engine: \"%s\" texture not loaded", atlas_name);
		return;
	}

	//move by camera
	dst.x -= engine->camera_target.x - aura_get_logical_size(&engine->aura).x/2;
	dst.y -= engine->camera_target.y + aura_get_logical_size(&engine->aura).y/2;

	//sprite's coordinate system is y up
	dst.y *= -1;

	//prevents nothing
	//dst.x = (int32_t)dst.x;
	//dst.y = (int32_t)dst.y;

	aura_texture_sprite(&engine->aura, texture, dst, src, degrees, center, flip_vertical, flip_horizontal, tint);
}

vec2_t engine_get_mouse_sprite(engine_t* engine){
	vec2_t pos = aura_mouse_render_position(&engine->aura);

	pos.x += engine->camera_target.x - aura_get_logical_size(&engine->aura).x/2;
	pos.y += engine->camera_target.y + aura_get_logical_size(&engine->aura).y/2;
	pos.y *= -1;

	return pos;
}
