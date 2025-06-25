#pragma once
#include "color.h"
#include "font.h"

#include <cglm/cglm.h>
#include <stddef.h>

void gfx_clear(color_t clear_color);
void gfx_clear_depth();
void gfx_clear_stencil();

typedef struct
{
	float position[2];
	color_t color;
}gfx_primitive_vertex;

void gfx_draw_triangle(vec2 left, vec2 top, vec2 right, color_t color);//cw
void gfx_draw_triangles(gfx_primitive_vertex* vertices, size_t num_vertices);//ccw
void gfx_draw_circle(vec2 pos, float radius, color_t color);
void gfx_draw_rectangle_from_points(vec2 tl, vec2 tr, vec2 ll, vec2 lr, color_t color);//top, lower, right and left combinations
void gfx_draw_rectangle(vec2 pos, vec2 size, color_t color);
void gfx_draw_rectangle_rounded(vec2 pos, vec2 size, float corner_radius, color_t color);
void gfx_draw_line(vec2 pos1, vec2 pos2, float thickness, color_t color);
void gfx_draw_lines(vec2* positions, size_t num_positions, float thickness, color_t color);

void gfx_draw_texture_from_points(uint32_t gl_texture, vec3 tl, vec3 tr, vec3 ll, vec3 lr, color_t color, vec2 source_size, vec4 source_rectangle);//source is top-left oriented, source_rectangle to {-1, -1, -1, -1} to ignore
void gfx_draw_texture(uint32_t gl_texture, vec2 pos, vec2 size);
void gfx_draw_texture_sprite(uint32_t gl_texture, vec2 pos, vec2 size, color_t color, vec2 source_size, vec4 source_rectangle);

void gfx_draw_character(char character, font_t* font, vec3 tl, vec3 tr, vec3 ll, vec3 lr, color_t color);
void gfx_draw_string(const char* string, font_t* font, vec2 pos, color_t color);

void gfx_render(vec2 screen_size);
void gfx_destroy();
