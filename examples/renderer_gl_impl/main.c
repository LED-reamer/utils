#include "utils/renderer.h"
#include "utils/logging.h"
#include "utils/window.h"
#include "utils/allocator.h"
#include "utils/types.h"
#include "utils/tools/memory_tracker.h"


int main(){
	memory_tracker_init(false);
	window_t window = window_create_opengl("renderer - opengl implementation", 1920, 1080);
	renderer_init(allocator_get_default(), &window, 0);


	while(window_open(&window)){
		renderer_render(vec2(10, 10), color(0.2, 0.2, 0.2, 1), renderer_get_default_camera());
		window_update(&window);
		if(key_just_down(&window, KEY_ESCAPE)) window_close(&window);

	}
	
	renderer_deinit();
	window_destroy(&window);
	memory_tracker_deinit();
	return 0;
}
