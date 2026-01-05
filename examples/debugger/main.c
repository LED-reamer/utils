#include "utils/tools/debugger.h"
#include "utils/window.h"

int main(void) {
	debugger_init();
	window_t window = window_create("main application", 1920/2, 1080/2);

	size_t current_size = 1, next_size = 1;
	void* data = malloc(current_size);

	while (window_open(&window)) {
		debugger_update();
		debugger_timestamp_begin("loop");
		window_update(&window);

		if (key_just_down(&window, KEY_ESCAPE)) window_close(&window);

		if (key_down(&window, KEY_UP)) next_size += 100;
		if (key_down(&window, KEY_DOWN) && next_size - 100 < next_size) next_size -= 100;

		if (current_size != next_size) {
			current_size = next_size;
			data = realloc(data, current_size);
		}
		debugger_timestamp_end("loop");
	}

	free(data);

	window_destroy(&window);
	debugger_deinit();
	return 0;
}
