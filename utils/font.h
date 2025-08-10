#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "allocator.h"

typedef struct {
	uint32_t codepoint;
	
	// fontatlas source rectangle
	uint32_t src_x, src_y, src_width, src_height;
	
	// metrics (scaled to pixel)
	float x_offset, y_offset;	//scaled offsets from baseline (y up)
	float width, height;		//scaled size
	float advance;				//scaled distance to advance after drawing character
} glyph_t;

typedef struct {
	allocator_t* allocator;
	
	// (scaled to pixels)
	float font_size;
	float ascent;		// distance(positive) from baseline to top
	float descent;	  	// distance(positive) from baseline to bottom
	float line_height;  // offset between baselines

	float space_width;//TODO implement

	uint32_t num_glyphs;
	glyph_t* glyphs;

	// atlas bitmap
	size_t atlas_width;
	size_t atlas_height;
	uint8_t* atlas;		// size of bitmap is atlas_width * atlas_height

	// sdf variables
	float sdf_thickness; //which values should still be rendered [0, 1]
	float sdf_smoothing; //sdf smoothing
} font_t;

font_t font_create_from_file(allocator_t* allocator, const char* filename, float pixel_size);
font_t font_create_from_memory(allocator_t* allocator, void* buffer, size_t buffer_size, float pixel_size);
void font_destroy(font_t* font);

// returns NULL if codepoint is not in font
glyph_t* font_get_glyph(font_t* font, uint32_t codepoint);

bool font_get_glyph_uv(font_t* font, glyph_t* glyph, float uv_rect[4]);

// returns false if codepoint is not in font
bool font_contains_codepoint(font_t* font, uint32_t codepoint);
bool font_get_codepoint_source_rectangle(font_t* font, uint32_t codepoint, uint32_t src_rect[4]);
bool font_get_codepoint_uv(font_t* font, uint32_t codepoint, float uv_rect[4]);
void font_codepoint_size(font_t* font, uint32_t codepoint, float pixel_height, float* width, float* height);
void font_string_size(font_t* font, const char* string, float pixel_height, float* width, float* height);
