#include "utils/renderer.h"
#include "utils/logging.h"
#include "utils/window.h"
#include "utils/allocator.h"
#include "utils/types.h"
#include "utils/tools/memory_tracker.h"


int main(void) {
	memory_tracker_init(false);
	window_t win = window_create_opengl("renderer", 800, 600);
	renderer_init(allocator_get_default(), &win, RENDERER_2D_SHAPES | RENDERER_TEXT | RENDERER_3D_SHAPES);
	font_t font = font_create_from_file(allocator_get_default(), "font.ttf", 128.0f);
	renderer_set_font(&font);

	camera_t camera = renderer_get_default_camera();
	mouse_set_relative_mode(&win, true);
	
	while (window_open(&win)) {
		window_update(&win);

		renderer_update_fps_camera(&camera);

		const char* string = "Hello World from renderer";
		float w, h;
		font_string_size(&font, string, 128, &w, &h);
		renderer_draw_text(string, vec2(0, 0), 64, color(0, 0, 0, 1));

		renderer_draw_cylinder(vec3(0, 0, 0), vec3(1, 0, 0), .05, color(1, 0, 0, 1));
		renderer_draw_cylinder(vec3(0, 0, 0), vec3(0, 1, 0), .05, color(0, 1, 0, 1));
		renderer_draw_cylinder(vec3(0, 0, 0), vec3(0, 0, 1), .05, color(0, 0, 1, 1));

		renderer_draw_3d_rectangle_points(vec3(0, 0, 0), vec3(-3, 0, 0), vec3(-3, -3, -1), vec3(0, -3, -1), color(1, 0, 0, 1));


		renderer_render(vec2(window_get_size(&win).x, window_get_size(&win).y), color(1, 1, 1, 1), camera);

		if (key_just_down(&win, KEY_ESCAPE)) window_close(&win);
		float speed = 0.01f;
		vec3_t movement = vec3(0, 0, 0);
		vec3_t right = vec3_normalize(vec3_cross(camera.direction, vec3(0, 1, 0)));
		if (key_down(&win, KEY_W)) movement = vec3_add(movement, vec3_scale(camera.direction,  speed));
		if (key_down(&win, KEY_S)) movement = vec3_add(movement, vec3_scale(camera.direction, -speed));
		if (key_down(&win, KEY_D)) movement = vec3_add(movement, vec3_scale(right,  speed));
		if (key_down(&win, KEY_A)) movement = vec3_add(movement, vec3_scale(right, -speed));
		if (key_down(&win, KEY_SPACE))  movement.y += speed;
		if (key_down(&win, KEY_LSHIFT)) movement.y -= speed;
		camera.position = vec3_add(camera.position, movement);
	}

	font_destroy(&font);
	//TODO forgot to deinit renderer!
	renderer_deinit();
	window_destroy(&win);
	memory_tracker_deinit();
	return 0;
}
