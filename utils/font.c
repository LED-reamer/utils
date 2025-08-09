#include "font.h"

#include "buffer.h"
#include "logging.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "3rd-party/stb_truetype.h"

typedef struct {
	void* data;	 // sdf bitmap
	int32_t xoff, yoff;
} internal_sdf_bitmap_t;

font_t font_create_from_file(allocator_t* allocator, const char* filename, float pixel_size) {
	buffer_t file_buffer = buffer_create(allocator);
	buffer_load_data_from_file(&file_buffer, filename);
	font_t font = font_create_from_memory(allocator, file_buffer.data, file_buffer.size, pixel_size);
	buffer_destroy(&file_buffer);

	if (font.glyphs == NULL || font.atlas == NULL) {
		ERROR("Could not load font \"%s\"", filename);
		font_destroy(&font);
		return (font_t){0};
	}

	return font;
}

font_t font_create_from_memory(allocator_t* allocator, void* buffer, size_t buffer_size, float pixel_size) {
	(void)buffer_size;	// unused

	font_t font = {
		.allocator = allocator,
		.font_size = pixel_size,
	};

	stbtt_fontinfo info = {0};
	if (!stbtt_InitFont(&info, buffer, 0)) {
		ERROR("Could not load font");
		return (font_t){0};
	}

	float font_to_pixel_scale = stbtt_ScaleForPixelHeight(&info, pixel_size);
	int32_t ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);
	font.ascent = ascent * font_to_pixel_scale;
	font.descent = descent * font_to_pixel_scale;
	font.line_height = (ascent - descent + line_gap) * font_to_pixel_scale;

	uint32_t max_glyph_size = 0;
	uint32_t glyphs_index = 0;
	internal_sdf_bitmap_t* sdf_bitmap_array = NULL;
	LOG("time before load (multithread this) -> first loop to get num_glyphs -> seconds threaded");
	//TODO multithread this loop
	for (uint32_t i = 0; i < (uint32_t)info.numGlyphs; i++) {
		int32_t id = stbtt_FindGlyphIndex(&info, i);
		if (id == 0)
			continue;

		int32_t width, height, xoff, yoff;
		void* data = stbtt_GetGlyphSDF(&info, font_to_pixel_scale, id, /*padding*/ 5, /*onedge_value*/ 180, /*pixel_dist_scale*/ 36.0f, &width, &height, &xoff, &yoff);
		if (data == NULL) {
			continue;
		}
		font.num_glyphs++;
		font.glyphs = font.allocator->arealloc(font.glyphs, font.num_glyphs * sizeof(glyph_t));
		sdf_bitmap_array = font.allocator->arealloc(sdf_bitmap_array, font.num_glyphs * sizeof(internal_sdf_bitmap_t));
		font.glyphs[glyphs_index].codepoint = i;
		sdf_bitmap_array[glyphs_index].data = data;
		sdf_bitmap_array[glyphs_index].xoff = roundf(xoff * font_to_pixel_scale);
		sdf_bitmap_array[glyphs_index].yoff = roundf(yoff * font_to_pixel_scale);

		int32_t glyph_box[4];//(x0, y0), (x1, y1) corner coordinates
		stbtt_GetGlyphBox(&info, id, &glyph_box[0], &glyph_box[1], &glyph_box[2], &glyph_box[3]);
		font.glyphs[glyphs_index].height = (glyph_box[3] - glyph_box[1]) * font_to_pixel_scale;
		font.glyphs[glyphs_index].top_space = font_to_pixel_scale * ascent - font.glyphs[glyphs_index].height;

		int32_t advance_width, left_size_bearing;
		stbtt_GetGlyphHMetrics(&info, id, &advance_width, &left_size_bearing);

		font.glyphs[glyphs_index].advance_width = advance_width * font_to_pixel_scale;
		font.glyphs[glyphs_index].width = (glyph_box[2] - glyph_box[0]) * font_to_pixel_scale;
		font.glyphs[glyphs_index].left_space = left_size_bearing * font_to_pixel_scale;

		font.glyphs[glyphs_index].src_width = width;
		font.glyphs[glyphs_index].src_height = height;

		if (max_glyph_size < (uint32_t)width) max_glyph_size = (uint32_t)width;
		if (max_glyph_size < (uint32_t)height) max_glyph_size = (uint32_t)height;

		glyphs_index++;
	}
	LOG("time after load");

	uint32_t characters_per_row = ((uint32_t)sqrt(font.num_glyphs)) + 1;
	uint32_t texture_size = characters_per_row * max_glyph_size;
	font.atlas_width = texture_size;
	font.atlas_height = texture_size;
	font.atlas = font.allocator->amalloc(font.atlas_width * font.atlas_height);
	memset(font.atlas, 0, font.atlas_width * font.atlas_height);

	uint32_t col = 0, row = 0;
	for (uint32_t i = 0; i < (uint32_t)font.num_glyphs; i++) {
		if (sdf_bitmap_array[i].data == NULL) {
			ERROR("SDF buffer was NULL");
			continue;
		}
		//is this correct?
		uint32_t bitmap_x = col * max_glyph_size;// - sdf_bitmap_array[i].xoff;
		uint32_t bitmap_y = row * max_glyph_size;// - sdf_bitmap_array[i].yoff;

		uint32_t glyph_width = font.glyphs[i].src_width;
		uint32_t glyph_height = font.glyphs[i].src_height;

		for (uint32_t row = 0; row < glyph_height; ++row) {
			memcpy(font.atlas + (bitmap_y + row) * texture_size + bitmap_x,
				   sdf_bitmap_array[i].data + row * glyph_width,
				   glyph_width);
		}

		stbtt_FreeSDF(sdf_bitmap_array[i].data, NULL);

		font.glyphs[i].src_x = bitmap_x;
		font.glyphs[i].src_y = bitmap_y;

		col++;
		if (col >= characters_per_row) {
			col = 0;
			row++;
		}
	}

	font.allocator->afree(sdf_bitmap_array);
	return font;
}

void font_destroy(font_t* font) {
	if (font->glyphs)
		font->allocator->afree(font->glyphs);
	if (font->atlas)
		font->allocator->afree(font->atlas);
	*font = (font_t){0};
}

glyph_t* font_get_glyph(font_t* font, uint32_t codepoint) {
	// TODO improve search algo ...
	// TODO add caching
	for (uint32_t i = 0; i < font->num_glyphs; i++) {
		if (font->glyphs[i].codepoint == codepoint)
			return &font->glyphs[i];
	}

	return NULL;
}

bool font_get_glyph_uv(font_t* font, glyph_t* glyph, float uv_rect[4]){
	if (!glyph) return false;
	
	uv_rect[0] = (float)glyph->src_x / font->atlas_width;
	uv_rect[1] = (float)glyph->src_y / font->atlas_height;
	uv_rect[2] = (float)(glyph->src_x + glyph->src_width) / font->atlas_width;
	uv_rect[3] = (float)(glyph->src_y + glyph->src_height) / font->atlas_height;
	
	return true;
}

bool font_contains_codepoint(font_t* font, uint32_t codepoint) {
	glyph_t* glyph = font_get_glyph(font, codepoint);
	if (glyph) return true;
	return false;
}

bool font_get_codepoint_source_rectangle(font_t* font, uint32_t codepoint, uint32_t src_rect[4]) {
	glyph_t* glyph = font_get_glyph(font, codepoint);
	if (!glyph) return false;

	src_rect[0] = glyph->src_x;
	src_rect[1] = glyph->src_y;
	src_rect[2] = glyph->src_width;
	src_rect[3] = glyph->src_height;

	return true;
}

bool font_get_codepoint_uv(font_t* font, uint32_t codepoint, float uv_rect[4]) {
	glyph_t* glyph = font_get_glyph(font, codepoint);
	return font_get_glyph_uv(font, glyph, uv_rect);
}

void font_codepoint_size(font_t* font, uint32_t codepoint, float* width, float* height) {
	glyph_t* glyph = font_get_glyph(font, codepoint);

	if (glyph) {
		*width = glyph->width;
		*height = glyph->height;
	} else {
		*width = 0;
		*height = 0;
	}
}

void font_string_size(font_t* font, const char* string, float* width, float* height) {
	float total_width = 0;
	float total_height = font->font_size;

	for (size_t i = 0; i < strlen(string); i++) {
		if(string[i] == '\n'){
			total_height += font->font_size;
			continue;
		}
		
		glyph_t* glyph = font_get_glyph(font, (uint32_t)string[i]);

		if (glyph) {
			total_width += glyph->advance_width;
		}
	}

	*width = total_width;
	*height = total_height;
}
