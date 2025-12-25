// link with -lm
#include <math.h>
#include <string.h>

#include "utils/font.h"
#include "utils/image.h"
#include "utils/logging.h"
#include "utils/tools/memory_tracker.h"

int main(void) {
	memory_tracker_init(false);
	const char* filename = "simple_font.ttf";

	font_t font = font_create_from_file(allocator_get_default(), filename, 24.0f);

	//for (size_t i = 0; i < font.num_glyphs; i++) {
	//	LOG("glyph (codepoint %i) is at reactangle(%u, %u, %u, %u), advance_width = %i, left_space = %i",
	//		font.glyphs[i].codepoint, font.glyphs[i].src_x, font.glyphs[i].src_y, font.glyphs[i].src_width, font.glyphs[i].src_height, font.glyphs[i].advance_width, font.glyphs[i].left_space);
	//}
	//WARNING("NOTE src_width is bigger than advance_width since with is the size of the sdf bitmap (which contains a buffer zone)");

	//LOG("font global info: font_size=%f, ascent=%i, descent=%i, line_height=%i, num_glyphs=%i", font.font_size, font.ascent, font.descent, font.line_height, font.num_glyphs);

	float w, h;
	font_string_size(&font, "Hello World", 16, &w, &h);
	LOG("size for \"Hello World\" is w=%f, h=%f", w, h);

	// test save fontatlas as image
	image_t img = image_create(allocator_get_default(), font.atlas_width, font.atlas_height, IMAGE_GREY, IMAGE_8BIT);
	memcpy(img.pixels, font.atlas, font.atlas_width * font.atlas_height);
	image_save(&img, IMAGE_PNG, "atlas.png", false);
	image_destroy(&img);

	font_destroy(&font);

	memory_tracker_deinit();
	return 0;
}
