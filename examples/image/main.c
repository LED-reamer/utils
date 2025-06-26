//link with -lm
#include "utils/image.h"
#include <stdint.h>

int main(void){
	image_t image = image_load(allocator_get_default(), "image.png", true, IMAGE_DONT_CARE);

	//use image ...
	
	image_save(&image, IMAGE_PNG, "output.png");

	image_destroy(&image);
	return 0;
}
