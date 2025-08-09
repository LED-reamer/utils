#pragma once
#include "allocator.h"
#include "font.h"
#include "types.h"
#include "window.h"

// 2D COORDINATE SYSTEM (units in pixels):
//	(0,0) ───────► x
//	   │
//	   ▼
//	   y

// 3D COORDINATE SYSTEM (left handed, units in meters):
//      y
//      ▲  z
//      │ ╱
//      │╱
//	(0,0,0) ─────► x

// TODO once arena and mesh are stack variables remove induvidual initialising and just do always all (since no heap allocations needed when not used -> arena)
typedef enum {
	RENDERER_2D_SHAPES = 1,
	RENDERER_3D_SHAPES = 2,
	RENDERER_TEXTURES = 4,
	RENDERER_TEXT = 8,
} renderer_e;

typedef struct {
	vec3_t position;
	vec3_t direction;
	vec3_t up_vector;
	float fov;
	float z_near;
	float z_far;
} camera_t;

camera_t renderer_get_default_camera();

void renderer_init(allocator_t* allocator, window_t* window, renderer_e renderer_flags);
void renderer_deinit();

void renderer_render(vec2_t screen_size, color_t clear_color, camera_t camera);

// -=2D-SHAPE-RENDERER=-
typedef struct {
	vec2_t position;
	color_t color;
} vertex_2d_shapes_t;

void renderer_draw_2d_shape_mesh(vertex_2d_shapes_t* vertices_ccw, size_t num_vertices);
void renderer_draw_triangle(vec2_t left, vec2_t top, vec2_t right, color_t color);
void renderer_draw_triangles(vec2_t* positions_ccw, size_t num_positions, color_t color);
void renderer_draw_rectangle_points(vec2_t top_left, vec2_t top_right, vec2_t bottom_right, vec2_t bottom_left, color_t color);
void renderer_draw_rectangle_size(vec2_t top_left, vec2_t size, color_t color);
void renderer_draw_rectangle_center(vec2_t center, vec2_t size, color_t color);
void renderer_draw_line(vec2_t pos1, vec2_t pos2, float thickness, color_t color);
void renderer_draw_circle(vec2_t position, float radius, color_t color);

// -=3D-SHAPE-RENDERER=-
typedef struct {
	vec3_t position;
	vec3_t normal;
	color_t color;
} vertex_3d_shapes_t;

void renderer_draw_3d_shape_mesh(vertex_3d_shapes_t* vertices_ccw, size_t num_vertices);
void renderer_draw_3d_triangle(vec3_t left, vec3_t top, vec3_t right, color_t color);
void renderer_draw_cylinder(vec3_t pos1, vec3_t pos2, float radius, color_t color);
// void renderer_draw_rectangle_points(vec2_t top_left, vec2_t top_right, vec2_t bottom_right, vec2_t bottom_left, color_t color);
// void renderer_draw_rectangle_size(vec2_t top_left, vec2_t size, color_t color);
// void renderer_draw_rectangle_center(vec2_t center, vec2_t size, color_t color);
// void renderer_draw_line(vec2_t pos1, vec2_t pos2, float thickness, color_t color);
// void renderer_draw_circle(vec2_t position, float radius, color_t color);

// -=TEXT-RENDERER=-
void renderer_set_font(font_t* font);
void renderer_draw_text(const char* string, vec2_t pos/*, TODO add float line_height or size to downsize for better quality*/, color_t color);
