//link with -lm
#include "utils/image.h"
#include "utils/tools/memory_tracker.h"
#include <stdint.h>

int main(void){
	debugger_init(false);
	image_t image = image_load(allocator_get_default(), "image.png", true, IMAGE_DONT_CARE);

	//use image ...
	
	image_save(&image, IMAGE_PNG, "output.png");

	image_destroy(&image);
	debugger_deinit();
	return 0;
}
