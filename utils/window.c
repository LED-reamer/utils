#include "window.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_time.h>
#include <SDL3/SDL_video.h>

#include "logging.h"

#define NUM_KEYBOARD_STATES SDL_SCANCODE_COUNT

//TODO store in static struct...
uint32_t num_windows = 0;
const bool* keyboard_state_current = NULL;
int32_t num_keyboard_states = 0;
float __mouse_x = 0, __mouse_y = 0, mouse_dx = 0, mouse_dy = 0, mouse_x_scroll = 0, mouse_y_scroll = 0;
SDL_Cursor* current_cursor = NULL;
SDL_WindowID window_to_close;
long double __previous_time = 0;//for delta time

// forward declaration
void __window_input_update();

window_t window_create(const char* title, uint32_t width, uint32_t height) {
	window_t window = (window_t){
		.is_open = true,
		.w = 1,
		.h = 1,
		.dark_theme = false,
	};

	if (num_windows == 0) {
		if (!SDL_Init(SDL_INIT_VIDEO)) {
			ERROR("Couldn't init SDL3");
			return (window_t){0};
		}
	}

	if (keyboard_state_current == NULL) {
		keyboard_state_current = SDL_GetKeyboardState(&num_keyboard_states);
	}

	// set props
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
	// SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, true);//not supported with gpu api
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);

	window.SDL3_window = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);
	if (window.SDL3_window == NULL) {
		ERROR("Couldn't create SDL3 window: %s", SDL_GetError());
		return (window_t){0};
	}

	if (SDL_GetSystemTheme() == SDL_SYSTEM_THEME_DARK) window.dark_theme = true;

	num_windows++;

	return window;
}

window_t window_create_opengl(const char* title, uint32_t width, uint32_t height) {
	window_t window = (window_t){
		.is_open = true,
		.w = 1,
		.h = 1,
		.dark_theme = false,
	};

	if (num_windows == 0) {
		if (!SDL_Init(SDL_INIT_VIDEO)) {
			ERROR("Couldn't init SDL3");
			return (window_t){0};
		}
	}

	if (keyboard_state_current == NULL) {
		keyboard_state_current = SDL_GetKeyboardState(&num_keyboard_states);
	}

	// set props
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
	//SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, true);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);

	window.SDL3_window = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);
	if (window.SDL3_window == NULL) {
		ERROR("Couldn't create SDL3 window: %s", SDL_GetError());
		return (window_t){0};
	}

	if (SDL_GetSystemTheme() == SDL_SYSTEM_THEME_DARK) window.dark_theme = true;

	num_windows++;

	return window;
}

window_t window_create_from_X11_handle(uint64_t x11_window) {
	window_t window = (window_t){
		.is_open = true,
		.w = 1,
		.h = 1,
		.dark_theme = false,
	};

	if (num_windows == 0) {
		if (!SDL_Init(SDL_INIT_VIDEO)) {
			ERROR("Couldn't init SDL3");
			return (window_t){0};
		}
	}

	if (keyboard_state_current == NULL) {
		keyboard_state_current = SDL_GetKeyboardState(&num_keyboard_states);
	}

	// set props
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, true);  // not supported with gpu api
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X11_WINDOW_NUMBER, x11_window);

	window.SDL3_window = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);
	if (window.SDL3_window == NULL) {
		ERROR("Couldn't create SDL3 window: %s", SDL_GetError());
		return (window_t){0};
	}

	if (SDL_GetSystemTheme() == SDL_SYSTEM_THEME_DARK) window.dark_theme = true;

	num_windows++;

	return window;
}

void window_destroy(window_t* window) {
	SDL_DestroyWindow(window->SDL3_window);
	*window = (window_t){0};

	num_windows--;
	if (num_windows == 0) {
		if (current_cursor != NULL)
			SDL_DestroyCursor(current_cursor);

		SDL_Quit();
	}
}

bool window_open(window_t* window) {
	return window->is_open;
}

void window_update(window_t* window) {
	if (window_to_close == SDL_GetWindowID(window->SDL3_window)) return window_close(window);

	long double now = window_get_time_s();
	window->delta_time = now - window->previous_time;
	window->previous_time = now;
	
	__window_input_update(window);
}

void window_close(window_t* window) {
	window->is_open = false;
	SDL_HideWindow(window->SDL3_window);
}

void window_set_title(window_t* window, const char* title) {
	SDL_SetWindowTitle(window->SDL3_window, title);
}

uvec2_t window_get_size(window_t* window) {
	int32_t width, height;
	SDL_GetWindowSize(window->SDL3_window, &width, &height);
	return uvec2(width, height);
}

void window_set_size(window_t* window, uint32_t width, uint32_t height) {
	SDL_SetWindowSize(window->SDL3_window, width, height);
}

uint32_t window_get_width(window_t* window) {
	return window_get_size(window).x;
}

uint32_t window_get_height(window_t* window) {
	return window_get_size(window).y;
}

ivec2_t window_get_position(window_t* window) {
	int32_t x, y;
	SDL_GetWindowSize(window->SDL3_window, &x, &y);
	return ivec2(x, y);
}

void window_set_position(window_t* window, int32_t x, int32_t y) {
	SDL_SetWindowSize(window->SDL3_window, x, y);
}

window_mode_e window_get_mode(window_t* window) {
	SDL_WindowFlags flags = SDL_GetWindowFlags(window->SDL3_window);

	if (flags & SDL_WINDOW_FULLSCREEN)
		return WINDOW_MODE_MAXIMIZED;
	else if (flags & SDL_WINDOW_MINIMIZED)
		return WINDOW_MODE_MINIMIZED;
	else
		return WINDOW_MODE_NORMAL;
}

void window_set_mode(window_t* window, window_mode_e mode) {
	switch (mode) {
		case WINDOW_MODE_MAXIMIZED:
			// SDL_MaximizeWindow(window->SDL3_handle);
			SDL_SetWindowFullscreen(window->SDL3_window, true);
			break;
		case WINDOW_MODE_MINIMIZED:
			SDL_MinimizeWindow(window->SDL3_window);
			break;
		case WINDOW_MODE_NORMAL:
		default:
			SDL_RestoreWindow(window->SDL3_window);
			break;
	}
}

bool window_is_focused(window_t* window) {
	SDL_WindowFlags flags = SDL_GetWindowFlags(window->SDL3_window);
	if (flags & SDL_WINDOW_INPUT_FOCUS /* || flags & SDL_WINDOW_MOUSE_FOCUS*/)
		return true;
	else
		return false;
}

void window_always_on_top(window_t* window, bool on_top) {
	SDL_SetWindowAlwaysOnTop(window->SDL3_window, on_top);
}

void window_set_icon(window_t* window, void* pixel_data, uint32_t width, uint32_t height) {
	SDL_Surface* icon = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_ARGB8888, pixel_data, width * 4);
	SDL_SetWindowIcon(window->SDL3_window, icon);
	SDL_DestroySurface(icon);
}

void window_set_framebuffer(window_t* window, uint32_t* pixels_rgba, uint32_t w, uint32_t h){
	SDL_Surface* src_surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA8888, pixels_rgba, w * 4);
	SDL_Surface* dest_surface = SDL_GetWindowSurface(window->SDL3_window);
	SDL_BlitSurface(src_surface, NULL, dest_surface, NULL);
	SDL_UpdateWindowSurface(window->SDL3_window);
	SDL_DestroySurface(src_surface);
}

long double window_get_delta_time(window_t* window){
	return window->delta_time;
}

int64_t window_get_time_ns() {
	SDL_Time ticks;
	SDL_GetCurrentTime(&ticks);
	return ticks;
}

long double window_get_time_s() {
	int64_t ticks = window_get_time_ns();
	return ticks / (long double)1000000000;
}

void window_text_input_begin(window_t* window, char* buffer, size_t size){
	if(window->text_input_mode) return;

	window->current_text_input_buffer_size = size;
	window->current_text_input_buffer = buffer;
	memset(window->current_text_input_buffer, 0, size);
	SDL_StartTextInput(window->SDL3_window);
	window->text_input_mode = true;
}

void window_text_input_end(window_t* window){
	if(!window->text_input_mode) return;
	
	SDL_StopTextInput(window->SDL3_window);
	window->current_text_input_buffer_size = 0;
	window->current_text_input_buffer = NULL;
	window->text_input_mode = false;
}

void __window_input_update(window_t* window) {
	if (num_keyboard_states != NUM_KEYBOARD_STATES) FATAL_ERROR("Wrong SDL3 SDL_SCANCODE_COUNT - outdated?");
	if (sizeof(window->keyboard_state_previous) != NUM_KEYBOARD_STATES) FATAL_ERROR("Wrong window->keyboard_state_previous static size");
	memcpy(window->keyboard_state_previous, keyboard_state_current, num_keyboard_states * sizeof(bool));
	window->mouse_state_previous = window->mouse_state_current;
	window->mouse_state_current = SDL_GetMouseState(&__mouse_x, &__mouse_y);
	SDL_GetRelativeMouseState(&mouse_dx, &mouse_dy);

	// reset
	mouse_x_scroll = 0;
	mouse_y_scroll = 0;

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_MOUSE_WHEEL) {
			if (e.wheel.x != 0)
				mouse_x_scroll += e.wheel.x;
			if (e.wheel.y != 0)
				mouse_y_scroll += e.wheel.y;
		}
		else if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
			window_to_close = e.window.windowID;
		}
		else if (window->text_input_mode && e.type == SDL_EVENT_TEXT_INPUT){
			if(window->current_text_input_buffer_size > strlen(window->current_text_input_buffer) + strlen(e.text.text)){
		    	strcat(window->current_text_input_buffer, e.text.text);
			}
		}
		else if (window->text_input_mode && (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_BACKSPACE)) {
		    size_t len = strlen(window->current_text_input_buffer);
		    if (len > 0) {
		        window->current_text_input_buffer[len - 1] = '\0';
		    }
		}
	}

	SDL_PumpEvents();
}

bool key_down(window_t* window, key_e key) {
	SDL_Scancode scancode = SDL_GetScancodeFromKey(key, NULL);
	if (keyboard_state_current[scancode] && window_is_focused(window))
		return true;
	return false;
}

bool key_just_down(window_t* window, key_e key) {
	SDL_Scancode scancode = SDL_GetScancodeFromKey(key, NULL);
	if (keyboard_state_current[scancode] && !window->keyboard_state_previous[scancode] && window_is_focused(window))
		return true;
	return false;
}

bool key_just_released(window_t* window, key_e key) {
	SDL_Scancode scancode = SDL_GetScancodeFromKey(key, NULL);
	if (!keyboard_state_current[scancode] && window->keyboard_state_previous[scancode] && window_is_focused(window))
		return true;
	return false;
}

bool mouse_button_down(window_t* window, mouse_button_e mouse_button) {
	if (window->mouse_state_current & SDL_BUTTON_MASK(mouse_button) && window_is_focused(window))
		return true;
	return false;
}

bool mouse_button_just_down(window_t* window, mouse_button_e mouse_button) {
	if ((window->mouse_state_current & SDL_BUTTON_MASK(mouse_button)) && !(window->mouse_state_previous & SDL_BUTTON_MASK(mouse_button)) && window_is_focused(window))
		return true;
	return false;
}

bool mouse_button_just_released(window_t* window, mouse_button_e mouse_button) {
	if (!(window->mouse_state_current & SDL_BUTTON_MASK(mouse_button)) && (window->mouse_state_previous & SDL_BUTTON_MASK(mouse_button)) && window_is_focused(window))
		return true;
	return false;
}

float mouse_get_scroll(window_t* window) {
	if (!window_is_focused(window)) return 0.0f;

	return mouse_y_scroll;
}

vec2_t mouse_get_position(window_t* window) {
	if (!window_is_focused(window)) {
		return vec2(0, 0);
	}
	return vec2(__mouse_x, __mouse_y);
}

vec2_t mouse_get_relative_position(window_t* window) {
	if (!window_is_focused(window)) {
		return vec2(0, 0);
	}
	return vec2(mouse_dx, mouse_dy);
}

void mouse_set_relative_mode(window_t* window, bool active){
	SDL_SetWindowRelativeMouseMode(window->SDL3_window, active);
}

void mouse_set_system_cursor(mouse_system_cursor_e cursor) {
	if (current_cursor != NULL)
		SDL_DestroyCursor(current_cursor);
	current_cursor = SDL_CreateSystemCursor((SDL_SystemCursor)cursor);

	SDL_SetCursor(current_cursor);
}

void mouse_hide_cursor(bool hidden) {
	if (hidden)
		SDL_HideCursor();
	else
		SDL_ShowCursor();
}

char* clipboard_get_string_allocated() {
	return SDL_GetClipboardText();
}

void clipboard_set_string(const char* string) {
	SDL_SetClipboardText(string);
}
