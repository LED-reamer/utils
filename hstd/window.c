#include "window.h"
#include "logger.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_time.h>
#include <SDL3/SDL_clipboard.h>

static SDL_Cursor* current_cursor = NULL;

b8 __try_init_SDL() {
	if(SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO){
		if (!SDL_Init(SDL_INIT_VIDEO))
			return false;
	}
	return true;
}

window_t window_create(cstring title, u32 width, u32 height){
	window_t window = { 0 };
	if(!__try_init_SDL()){
		logger.err("Couldn't init SDL");
		return window;
	}
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);
	window.handle = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);

	if(window.handle == NULL) {
		logger.err("Couldn't init create sdl window");
		return (window_t){0};
	}

	window.is_open = true;
	return window;
}

window_t window_create_for_opengl(cstring title, u32 width, u32 height){
	window_t window = { 0 };
	if(!__try_init_SDL()){
		logger.err("Couldn't init SDL");
		return window;
	}
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
	window.handle = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);

	if(window.handle == NULL) {
		logger.err("Couldn't init create sdl window for opengl");
		return (window_t){0};
	}

	window.is_open = true;
	return window;
}

window_t window_create_from_X11(u64 x11_handle){
	window_t window = { 0 };
	if(!__try_init_SDL()){
		logger.err("Couldn't init SDL");
		return window;
	}
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X11_WINDOW_NUMBER, x11_handle);
	window.handle = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);

	if(window.handle == NULL) {
		logger.err("Couldn't init create sdl window from x11_handle");
		return (window_t){0};
	}

	window.is_open = true;
	return window;
}

void window_destroy(window_t* window){
	SDL_DestroyWindow(window->handle);
	*window = (window_t){0};
	//we don't SDL_Quit here because we don't know if the users still need it!
}

b8 window_open(window_t* window) {
	return window->is_open;
}

void window_update(window_t* window){
	f128 now = window_time_s();
	window->dt = now - window->last_time;
	window->last_time = now;

	//update keyboard state
	s32 num_keys = 0;
	const b8* current_keyboard_state = (const b8*)SDL_GetKeyboardState(&num_keys);
	if(num_keys != (sizeof(window->keyboard_state) / sizeof(window->keyboard_state[0]))) {
		logger.err("num_keys changed. Old scancodes!");
		return window_close(window);
	}
	memcpy(window->keyboard_state_previous, window->keyboard_state, sizeof(window->keyboard_state) / sizeof(window->keyboard_state[0]));
	memcpy(window->keyboard_state, current_keyboard_state, sizeof(window->keyboard_state) / sizeof(window->keyboard_state[0]));
	//update mouse state
	window->mouse_state_previous = window->mouse_state;
	window->mouse_state = SDL_GetMouseState(NULL, NULL);
	
	window->scroll_delta = VEC2F(0, 0);
	
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_MOUSE_WHEEL) {
			window->scroll_delta.x += e.wheel.x;
			window->scroll_delta.y += e.wheel.y;
		}
		else if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
			if(e.window.windowID == SDL_GetWindowID(window->handle)) window_close(window);
		}
		else if(window->text_input_mode){
			if(e.type == SDL_EVENT_TEXT_INPUT){
				if(window->text_input_size > strlen(window->text_input_buffer) + strlen(e.text.text)){
					strcat(window->text_input_buffer, e.text.text);
				}
			}
			if(e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_BACKSPACE){
				size_t len = strlen(window->text_input_buffer);
				if (len > 0) {
				    window->text_input_buffer[len - 1] = '\0';
				}
			}
		}
	}
	
	SDL_PumpEvents();
}

void window_close(window_t* window) {
	window->is_open = false;
	SDL_HideWindow(window->handle);
}

b8 window_is_focused(window_t* window){
	SDL_WindowFlags flags = SDL_GetWindowFlags(window->handle);
	if (flags & SDL_WINDOW_INPUT_FOCUS)
		return true;
	return false;
}

void window_set_title(window_t* window, cstring title) {
	SDL_SetWindowTitle(window->handle, title);
}

void window_set_icon(window_t* window, void* rgba, u32 width, u32 height){
	SDL_Surface* icon = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA8888/*SDL_PIXELFORMAT_ARGB8888*/, rgba, width * 4);
	SDL_SetWindowIcon(window->handle, icon);
	SDL_DestroySurface(icon);
}

void window_set_framebuffer(window_t* window, u32* rgba, u32 w, u32 h){
	SDL_Surface* src_surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA8888, rgba, w * 4);
	SDL_Surface* dest_surface = SDL_GetWindowSurface(window->handle);
	SDL_BlitSurface(src_surface, NULL, dest_surface, NULL);
	SDL_UpdateWindowSurface(window->handle);
	SDL_DestroySurface(src_surface);
}

s64 window_time_ns(){
	SDL_Time ticks;
	SDL_GetCurrentTime(&ticks);
	return ticks;
}

f128 window_time_s(){
	s64 ticks = window_time_ns();
	return ticks / (long double)1000000000;
}

void window_text_input_begin(window_t* window, char* buffer, u64 size){
	if(window->text_input_mode) return;

	window->text_input_size = size;
	window->text_input_buffer = buffer;
	memset(window->text_input_buffer, 0, size);
	SDL_StartTextInput(window->handle);
	window->text_input_mode = true;
}

void window_text_input_end(window_t* window){
	if(!window->text_input_mode) return;
		
	SDL_StopTextInput(window->handle);
	window->text_input_size = 0;
	window->text_input_buffer = NULL;
	window->text_input_mode = false;
}

b8 key_down(window_t* window, key_e key){
	SDL_Scancode scancode = SDL_GetScancodeFromKey(key, NULL);
	if (window->keyboard_state[scancode] && window_is_focused(window))
		return true;
	return false;
}

b8 key_just_down(window_t* window, key_e key){
	SDL_Scancode scancode = SDL_GetScancodeFromKey(key, NULL);
	if (window->keyboard_state[scancode] && !window->keyboard_state_previous[scancode] && window_is_focused(window))
		return true;
	return false;
}

b8 key_just_released(window_t* window, key_e key){
	SDL_Scancode scancode = SDL_GetScancodeFromKey(key, NULL);
	if (!window->keyboard_state[scancode] && window->keyboard_state_previous[scancode] && window_is_focused(window))
		return true;
	return false;
}

b8 mouse_button_down(window_t* window, mouse_button_e mouse_button){
	if (window->mouse_state & SDL_BUTTON_MASK(mouse_button) && window_is_focused(window))
		return true;
	return false;
}

b8 mouse_button_just_down(window_t* window, mouse_button_e mouse_button){
	if ((window->mouse_state & SDL_BUTTON_MASK(mouse_button)) && !(window->mouse_state_previous & SDL_BUTTON_MASK(mouse_button)) && window_is_focused(window))
		return true;
	return false;
}

b8 mouse_button_just_released(window_t* window, mouse_button_e mouse_button){
	if (!(window->mouse_state & SDL_BUTTON_MASK(mouse_button)) && (window->mouse_state_previous & SDL_BUTTON_MASK(mouse_button)) && window_is_focused(window))
		return true;
	return false;
}

f64 mouse_get_scroll(window_t* window){
	return window->scroll_delta.y;
}

vec2f mouse_get_position(window_t* window){
	vec2f ret = VEC2F(0, 0);
	if (!window_is_focused(window)) {
		return ret;
	}
	SDL_GetMouseState(&ret.x, &ret.y);
	return ret;
}

vec2f mouse_get_relative_position(window_t* window){
	vec2f ret = VEC2F(0, 0);
	if (!window_is_focused(window)) {
		return ret;
	}
	SDL_GetRelativeMouseState(&ret.x, &ret.y);
	return ret;
}

void mouse_set_relative_mode(window_t* window, b8 active){
	SDL_SetWindowRelativeMouseMode(window->handle, active);
}

void mouse_set_system_cursor(mouse_system_cursor_e cursor){
	if (current_cursor != NULL)
		SDL_DestroyCursor(current_cursor);
	current_cursor = SDL_CreateSystemCursor((SDL_SystemCursor)cursor);

	SDL_SetCursor(current_cursor);
}

void mouse_hide_cursor(b8 hidden){
	if (hidden)
		SDL_HideCursor();
	else
		SDL_ShowCursor();
}

char* clipboard_get_string_allocated(){
	return SDL_GetClipboardText();
}
void clipboard_set_string(cstring string){
	SDL_SetClipboardText(string);
}
