#include <stdlib.h>

#include "utils/logging.h"
#include "utils/window.h"

// this is just the basic structure of a single window application
// for all the functionality see utils/window.h

int main(void) {
	window_t win = window_create("window name", 800, 600);

	while (window_open(&win)) {
		window_update(&win);

		if (key_just_down(&win, KEY_ESCAPE)) window_close(&win);
	}

	window_destroy(&win);
	return 0;
}
