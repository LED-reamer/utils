#include <stdlib.h>

#include "utils/allocator.h"
#include "utils/renderer.h"
#include "utils/tools/memory_tracker.h"
#include "utils/window.h"
#include "utils/logging.h"

#include <SDL3/SDL.h>

int main(void) {
	memory_tracker_init(false);
	window_t win = window_create("renderer", 800, 600);
	renderer_init(allocator_get_default(), &win, RENDERER_2D_SHAPES | RENDERER_TEXT | RENDERER_3D_SHAPES);

	font_t font = font_create_from_file(allocator_get_default(), "font.ttf", 128.0f);
	renderer_set_font(&font);

	camera_t camera = renderer_get_default_camera();

	while (window_open(&win)) {
		if(strlen(SDL_GetError()) != 0) ERROR("sdl: %s", SDL_GetError());
	
		window_update(&win);

		renderer_draw_line(vec2(0, 0), mouse_get_position(&win), 5, color(0, 0, 0, 1));
		const char* string = "Hello World from renderer";
		float w, h;
		font_string_size(&font, string, 128, &w, &h);
		renderer_draw_rectangle_size(vec2(mouse_get_position(&win).x, mouse_get_position(&win).y), vec2(w, h), color(1, 0, 0, .2));
		renderer_draw_text(string, vec2(0, 0), 64, color(0, 0, 0, 1));

		renderer_draw_cylinder(vec3(0, 0, 0), vec3(1, 0, 0), .1, color(1, 0, 0, 1));
		renderer_draw_cylinder(vec3(0, 0, 0), vec3(0, 1, 0), .1, color(0, 1, 0, 1));
		renderer_draw_cylinder(vec3(0, 0, 0), vec3(0, 0, 1), .1, color(0, 0, 1, 1));


		//TODO: depth testing is still wrong
		renderer_draw_cylinder(vec3(-1, 0, 2), vec3(1, 0, 2), .1, color(.5, .5, .5, 1));
		renderer_draw_cylinder(vec3(0, 1, 2), vec3(0, -1, 2), .1, color(.2, .2, .2, 1));

		renderer_render(vec2(window_get_size(&win).x, window_get_size(&win).y), color(1, 1, 1, 1), camera);

		//camera.direction = vec3(0, 0, 1);

		if (key_just_down(&win, KEY_ESCAPE)) window_close(&win);
		if (key_down(&win, KEY_A)) camera.position.x += .01;
		if (key_down(&win, KEY_D)) camera.position.x += -.01;
		if (key_down(&win, KEY_W)) camera.position.z += .01;
		if (key_down(&win, KEY_S)) camera.position.z += -.01;
		if (key_down(&win, KEY_SPACE)) camera.position.y += .01;
		if (key_down(&win, KEY_LSHIFT)) camera.position.y += -.01;
	}

	font_destroy(&font);
	window_destroy(&win);
	if(strlen(SDL_GetError()) != 0) ERROR("sdl: %s", SDL_GetError());
	memory_tracker_deinit();
	return 0;
}
