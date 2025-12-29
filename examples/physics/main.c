#include "utils/logging.h"
#include "utils/renderer.h"
#include "utils/physics.h"
#include "utils/tools/memory_tracker.h"

void update_camera_controller(window_t* win, camera_t* camera){
	float speed = 0.01f;
	vec3_t movement = vec3(0, 0, 0);
	vec3_t right = vec3_normalize(vec3_cross(camera->direction, vec3(0, 1, 0)));
	if (key_down(win, KEY_W)) movement = vec3_add(movement, vec3_scale(camera->direction,  speed));
	if (key_down(win, KEY_S)) movement = vec3_add(movement, vec3_scale(camera->direction, -speed));
	if (key_down(win, KEY_D)) movement = vec3_add(movement, vec3_scale(right,  speed));
	if (key_down(win, KEY_A)) movement = vec3_add(movement, vec3_scale(right, -speed));
	if (key_down(win, KEY_SPACE))  movement.y += speed;
	if (key_down(win, KEY_LSHIFT)) movement.y -= speed;
	camera->position = vec3_add(camera->position, movement);
}

int main(){
	memory_tracker_init(false);
	window_t win = window_create_opengl("physics", 800, 600);
	camera_t camera = renderer_get_default_camera();
	mouse_set_relative_mode(&win, true);
	renderer_init(allocator_get_default(), &win, RENDERER_3D_SHAPES);
	
	// ---
	physics_world_t world = physics_world_create(allocator_get_default());
	physics_object_t ball = physics_get_default_sphere(vec3(0, 0, 0), 0.5f, 1);
	physics_object_t* obj = physics_object_add(&world, ball);
	obj->is_static = true;
	ball.center_of_mass.y = 0.9f;
	physics_object_add(&world, ball);

	while (window_open(&win)){
		window_update(&win);
		renderer_update_fps_camera(&camera);
		if (key_just_down(&win, KEY_ESCAPE)) window_close(&win);
		if (key_just_down(&win, KEY_Q)) physics_object_add(&world, ball);

		update_camera_controller(&win, &camera);

		renderer_draw_cylinder(vec3(0, 0, 0), vec3(1, 0, 0), 0.01f, color(1, 0, 0, 1));
		renderer_draw_cylinder(vec3(0, 0, 0), vec3(0, 1, 0), 0.01f, color(0, 1, 0, 1));
		renderer_draw_cylinder(vec3(0, 0, 0), vec3(0, 0, 1), 0.01f, color(0, 0, 1, 1));
		
		hashmap_foreach(&world.objects, obj_entry){
			physics_object_t* obj = (physics_object_t*)obj_entry->value_ptr;
			if(obj->collider_type == PHYSICS_SPHERE){
				renderer_draw_sphere(obj->center_of_mass, obj->as.sphere.radius, color(1, 0, 0, 0.5f));
			}
		}
		physics_world_step(&world, 1/60.0f/*dt later*/);
		size_t num_collisions;
		physics_collision_t* collisions = physics_get_current_collisions(&world, &num_collisions);
		for(size_t i = 0; i < num_collisions; i++){
			renderer_draw_sphere(collisions[i].contact_point, 0.05f, color(0, 1, 0, 0.5f));
		}
		
		
		renderer_render(vec2(window_get_size(&win).x, window_get_size(&win).y), color(1, 1, 1, 1), camera);
	}
	
	physics_object_remove(&world, obj);

	physics_world_destroy(&world);
	// ---
	
	renderer_deinit();
	window_destroy(&win);
	memory_tracker_deinit();
	return 0;
}
