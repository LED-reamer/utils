#pragma once
#include "buffer.h"
#include "hstd/array.h"

void init();
void quit();
void loop();

void editor_open_file(path_t path);

typedef struct window_t{
	void* term_win;
	buffer_t buffer;
	size_t scroll;//buffer-line index to be drawn in the first window-line
}window_t;
