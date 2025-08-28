#include "utils/tools/debugger.h"
#include "utils/tools/engine.h"
#include "utils/random.h"
#include "utils/logging.h"

typedef struct{
	vec2_t position;
}sprite_t;

void ecs_sprite_update(ecs_t* ecs, entity_t e, hashmap_t* other, void* userdata){
	(void)other;
	sprite_t* sprite = ECS_ENTITY_GET_COMPONENT(ecs, e, sprite_t);
	sprite->position.x += random_float(-5, 5);
	sprite->position.y += random_float(-5, 5);

	engine_t* engine = (engine_t*)userdata;

	if(key_down(&engine->window, KEY_DOWN)){
		sprite->position.y += 10;
	}
}

void ecs_sprite_draw(ecs_t* ecs, entity_t e, hashmap_t* other, void* userdata){
	(void)other;
	engine_t* engine = (engine_t*)userdata;
	sprite_t* sprite = ECS_ENTITY_GET_COMPONENT(ecs, e, sprite_t);
	engine_draw_sprite(engine, "spritesheet", rectangle(sprite->position.x, sprite->position.y, 64, 64), rectangle(0, 0, 16, 16), 0, vec2(32, 32), false, false, color(1, 1, 1, 1));
}

int main(){
	debugger_init();
	engine_args_t args = engine_args_standard();
	args.logical_width = 640;
	args.logical_height = 360;
	engine_t* engine = engine_create(args);

	ecs_set_userdata(&engine->ecs, engine);

	ECS_REGISTER_COMPONENT(&engine->ecs, sprite_t);
	ECS_REGISTER_SYSTEM(&engine->ecs, ecs_sprite_draw, SYSTEM_ON_FRAME, sprite_t);
	ECS_REGISTER_SYSTEM(&engine->ecs, ecs_sprite_update, SYSTEM_ON_UPDATE, sprite_t);

	engine_load_atlas(engine, "res/1-bit_gamepack_colored.png", "spritesheet");

	while(engine_running(engine)){
		debugger_update();
		if(key_just_down(&engine->window, KEY_ESCAPE)) window_close(&engine->window);
		aura_clear(&engine->aura, color(1, 1, 1, 1));
		engine_draw_sprite(engine, "spritesheet", rectangle(0, 0, 64, 64), rectangle(0, 0, 16, 16), 0, vec2(32, 32), false, false, color(1, 1, 1, 1));

		if(key_down(&engine->window, KEY_SPACE)){
			entity_t e = ECS_ENTITY_ADD(&engine->ecs);
			ECS_ENTITY_ADD_COMPONENT(&engine->ecs, e, sprite_t);
		}
		
		if(key_down(&engine->window, KEY_A)){
			vec2_t cam = engine_get_camera_position(engine);
			cam.x += 1;
			engine_set_camera_position(engine, cam);
		}
		if(key_down(&engine->window, KEY_D)){
			vec2_t cam = engine_get_camera_position(engine);
			cam.x -= 1;
			engine_set_camera_position(engine, cam);
		}
		if(key_down(&engine->window, KEY_W)){
			vec2_t cam = engine_get_camera_position(engine);
			cam.y += 1;
			engine_set_camera_position(engine, cam);
		}
		if(key_down(&engine->window, KEY_S)){
			vec2_t cam = engine_get_camera_position(engine);
			cam.y -= 1;
			engine_set_camera_position(engine, cam);
		}
	}

	engine_destroy(engine);
	debugger_deinit();
	return 0;
}
