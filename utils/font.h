#pragma once
#include "allocator.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct{
	uint32_t codepoint;
	//fontatlas source rectangle
	uint32_t src_x, src_y, src_width, src_height;

	//metrics (in pixel)
	int32_t advance_width;//draw character. move to right by advance pixels. repeat.
	int32_t left_space;//offset of where to draw
}glyph_t;

typedef struct{
	allocator_t* allocator;
	//(in pixels)
	float font_size;
	int32_t ascent;//offset baseline to top
	int32_t descent;//offset baseline to bottom
	int32_t line_height;//offset between baselines

	uint32_t num_glyphs;
	glyph_t* glyphs;

	size_t atlas_width;
	size_t atlas_height;
	uint8_t* atlas;//size of bitmap is atlas_width * atlas_height
}font_t;


font_t font_create_from_file(allocator_t* allocator, const char* filename, float pixel_size);
font_t font_create_from_memory(allocator_t* allocator, void* buffer, size_t buffer_size, float pixel_size);
void font_destroy(font_t* font);

//returns NULL if codepoint is not in font
glyph_t* font_get_glyph(font_t* font, uint32_t codepoint);

//returns false if codepoint is not in font
bool font_contains_codepoint(font_t* font, uint32_t codepoint);
bool font_get_codepoint_source_rectangle(font_t* font, uint32_t codepoint, uint32_t src_rect[4]);
bool font_get_codepoint_uv(font_t* font, uint32_t codepoint, uint32_t uv_rect[4]);
void font_codepoint_size(font_t* font, uint32_t codepoint, uint32_t* width, uint32_t* height);
void font_string_size(font_t* font, const char* string, uint32_t* width, uint32_t* height);
