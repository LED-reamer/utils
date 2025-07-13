//link with -lm
#include "utils/font.h"
#include "utils/logging.h"
#include "utils/image.h"
#include "utils/tools/memory_tracker.h"
#include <math.h>
#include <string.h>

int main(void){
	debugger_init(false);
	const char* filename = "simple_font.ttf";

	font_t font = font_create_from_file(allocator_get_default(), filename, 24.0f);

	for(size_t i = 0; i < font.num_glyphs; i++){
		LOG("glyph (codepoint %i) is at reactangle(%u, %u, %u, %u), advance_width = %i, left_space = %i",
		font.glyphs[i].codepoint, font.glyphs[i].src_x, font.glyphs[i].src_y, font.glyphs[i].src_width, font.glyphs[i].src_height, font.glyphs[i].advance_width, font.glyphs[i].left_space);
	}
	WARNING("NOTE src_width is bigger than advance_width since with is the size of the sdf bitmap (which contains a buffer zone)");

	LOG("font global info: font_size=%f, ascent=%i, descent=%i, line_height=%i, num_glyphs=%i", font.font_size, font.ascent, font.descent, font.line_height, font.num_glyphs);

	uint32_t w, h;
	font_string_size(&font, "Hello World", &w, &h);
	LOG("size for \"Hello World\" is w=%u, h=%u", w, h);

	//test save fontatlas as image
	image_t img = image_create(allocator_get_default(), font.atlas_width, font.atlas_height, IMAGE_GREY, IMAGE_8BIT);
	memcpy(img.pixels, font.atlas, font.atlas_width*font.atlas_height);
	image_save(&img, IMAGE_PNG, "atlas.png", false);
	image_destroy(&img);


	font_destroy(&font);

	debugger_deinit();
	return 0;
}
