// link with -lm
#include <stdint.h>

#include "utils/image.h"
#include "utils/logging.h"
#include "utils/tools/memory_tracker.h"

int main(void) {
	memory_tracker_init(false);
	const char* filename = "image.png";
	image_t image = image_create_from_file(allocator_get_default(), filename, true, IMAGE_AUTO, IMAGE_AUTO);
	LOG("image \"%s\":\n\t- number of color channels = %u\n\t- number of bits per channels = %u\n\t- width = %u\n\t- height = %u", filename, image.channels, image.channel_size * 8, image.width, image.height);

	// use image ...

	image_save(&image, IMAGE_PNG, "output.png", true);

	image_destroy(&image);
	memory_tracker_deinit();
	return 0;
}
