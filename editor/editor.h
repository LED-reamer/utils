#pragma once
#include "hstd/array.h"

void init();
void quit();
void loop();

MAKE_ARRAY(windows_t, window_t);
typedef enum{ LAYOUT_VERTICAL, LAYOUT_HORIZONTAL }layout_e;
typedef struct{
	void* term_win;

	layout_e layout;
	window_t* parent;
	windows_t children;
}window_t;
