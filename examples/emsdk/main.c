#include <stdlib.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "utils/logging.h"
#include "utils/window.h"

static window_t win;
static bool running = true;

void quit(){
	window_destroy(&win);
	#ifdef __EMSCRIPTEN__
	emscripten_cancel_main_loop();  /* this should "kill" the app. */
	#else
	exit(0);
	#endif
}

void main_loop(){
	window_update(&win);
	if (key_just_down(&win, KEY_ESCAPE)) {window_close(&win); running = false; }
	if (!running) quit();
}

int main() {
	printf("HELLO WORLD!\n");
	win = window_create("window name", 800, 600);

	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
	#else
	while (1) { main_loop(); }
	#endif
	return 0;
}
