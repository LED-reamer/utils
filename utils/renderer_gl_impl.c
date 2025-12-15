#include "renderer.h"
#include "allocator.h"
#include "SDL3/SDL_video.h"
#include "opengl.h"

typedef struct{
	allocator_t* allocator;
	SDL_GLContext gl;
	window_t* window;
}gl_context_t;

static gl_context_t ctx = {0};

static camera_t default_camera = {
	.position = vec3(0, 0, -5),
	.direction = vec3(0, 0, 1),
	.up_vector = vec3(0, 1, 0),
	.fov = 60,
	.z_near = 0.01f,
	.z_far = 1000.0f,
};

camera_t renderer_get_default_camera() {
	return default_camera;
}

void renderer_init(allocator_t* allocator, window_t* window, renderer_e renderer_flags){
	ctx = (gl_context_t){0};
	ctx.allocator = allocator;
	ctx.gl = SDL_GL_CreateContext(window->SDL3_window);
	ctx.window = window;
}

void renderer_deinit(){
	SDL_GL_DestroyContext(ctx.gl);
	ctx = (gl_context_t){0};
}

void renderer_render(vec2_t screen_size, color_t clear_color, camera_t camera){
	gl_clear_color(clear_color);
	SDL_GL_SwapWindow(ctx.window->SDL3_window);
}

