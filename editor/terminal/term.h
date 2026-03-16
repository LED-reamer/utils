#pragma once
#include <stddef.h>

void term_init();
void term_quit();
void term_trigger_refresh();
void term_update();

typedef void* term_window_t;

term_window_t term_window_open(size_t x, size_t y, size_t w, size_t h);
void term_window_close(term_window_t window);//autocloses with term_quit
void term_resize_window(term_window_t* window, size_t x, size_t y, size_t w, size_t h);
term_window_t* term_get_windows(size_t* count);

typedef struct{
	int key;
}term_event_t;

term_event_t term_next_event();
