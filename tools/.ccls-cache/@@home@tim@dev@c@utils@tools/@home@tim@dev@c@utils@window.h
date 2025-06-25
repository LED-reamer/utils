//link with -lGL -lGLEW -lglfw
//works on at least windows and linux

//includes:
//	logging.h
//	allocator.h

#pragma once
#include "allocator.h"

#include <GL/glew.h>
#include <stdint.h>


typedef struct
{
	allocator_t* allocator;
	void* glfw_handle;
} window_t;

typedef enum
{
	WINDOW_MODE_NORMAL,
	WINDOW_MODE_MAXIMIZED,
	WINDOW_MODE_MINIMIZED,
}window_mode_e;

window_t* window_create(allocator_t* allocator, const char* title, uint32_t width, uint32_t height);
void window_destroy(window_t* window);
int window_open(window_t* window);
void window_update(window_t* window);
void window_close(window_t* window);
void window_set_current(window_t* window);
void window_set_title(window_t* window, const char* title);
void window_get_size(window_t* window, uint32_t* width, uint32_t* height);
void window_set_size(window_t* window, uint32_t width, uint32_t height);
uint32_t window_get_width(window_t* window);
uint32_t window_get_height(window_t* window);
void window_get_position(window_t* window, int32_t* x, int32_t* y);
void window_set_position(window_t* window, int32_t x, int32_t y);
window_mode_e window_get_mode(window_t* window);//warning: WINDOW_MODE_MINIMIZED is not detected reliably!
void window_set_mode(window_t* window, window_mode_e mode);
int window_is_focused(window_t* window);
void window_set_vsync(window_t* window, int interval);//0 for benchmark, 1 no tearing
void window_get_attention(window_t* window);//warning: not always supported
void window_set_icon(window_t* window, void* pixel_data, uint32_t width, uint32_t height);

double window_get_time_seconds();
void window_set_time_seconds(double new_time);

void window_get_keyboard_states(window_t* window, void** current, void** previous);
void window_get_mouse_states(window_t* window, void** current, void** previous);
window_t* window_get_current();
