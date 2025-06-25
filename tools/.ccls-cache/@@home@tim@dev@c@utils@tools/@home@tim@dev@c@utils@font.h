#pragma once
#include "gl_wrapper.h"
#include "allocator.h"

#include <stdint.h>
#include <stddef.h>

typedef struct
{
	allocator_t* allocator;
	texture_t* atlas;
	int32_t glyph_rects[95][4];
	int32_t line_height;
}font_t;

//TODO support stb_truetype.h SDF loading
font_t* font_load(allocator_t* allocator, const char* filepath, int32_t font_size);
font_t* font_load_from_memory(allocator_t* allocator, uint8_t* data, int32_t font_size);
void font_get_character_size(font_t* font, char character, int32_t out_rect[2]);
void font_get_string_size(font_t* font, const char* string, int32_t out_rect[2]);
void font_get_character_on_atlas(font_t* font, char character, int32_t out_rect[4]);
void font_destroy(font_t* font);
