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
	renderer_init(allocator_get_default(), &win, RENDERER_2D_SHAPES | RENDERER_TEXT);

	font_t font = font_create_from_file(allocator_get_default(), "simple_font.ttf", 128.0f);
	//font_t font = font_create_from_file(allocator_get_default(), "second_font.ttf", 64.0f);
	renderer_set_font(&font);

	while (window_open(&win)) {

		if(strlen(SDL_GetError()) != 0) ERROR("sdl: %s", SDL_GetError());
	
		window_update(&win);

		renderer_draw_line(vec2(0, 0), mouse_get_position(&win), 5, color(0, 0, 0, 1));
		renderer_draw_text("Hellnpo World", mouse_get_position(&win), color(1, 0, 0, 1));

		renderer_render(vec2(window_get_size(&win).x, window_get_size(&win).y), color(1, 1, 1, 1), renderer_get_default_camera());

		if (key_just_down(&win, KEY_ESCAPE)) window_close(&win);
	}

	font_destroy(&font);
	window_destroy(&win);
	memory_tracker_deinit();
	return 0;
}
