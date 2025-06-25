//link with -lm
//platform independent code

//includes:
//	logging.h
//	allocator.h

#pragma once
#include "allocator.h"
#include <stdint.h>
#include <stddef.h>

#define JPG_QUALITY 50 //0 - 100, higher number -> better quality but bigger size

typedef enum
{
	IMAGE_NONE, //do not use!
	IMAGE_R, //monochrome
	IMAGE_RA,//monochrome and alpha
	IMAGE_RGB,
	IMAGE_RGBA
}image_channels_e;

typedef enum
{
	IMAGE_PNG,
	IMAGE_BMP,
	IMAGE_TGA,
	IMAGE_JPG,
	IMAGE_HDR,
}image_filetype_e;

typedef struct
{
	allocator_t* allocator;
	uint32_t width, height;
	image_channels_e channels;
	uint8_t bits_per_channel;
	void* pixels;
}image_t;

//8-bit
image_t* image_load(allocator_t* allocator, const char* filename, uint8_t flip_vertically, image_channels_e force_channels);
image_t* image_load_from_memory(allocator_t* allocator, uint8_t* pixels, size_t buffer_size, uint8_t flip_vertically, image_channels_e force_channels);
//16-bit
image_t* image_load_16bit(allocator_t* allocator, const char* filename, uint8_t flip_vertically, image_channels_e force_channels);
image_t* image_load_from_memory_16bit(allocator_t* allocator, uint8_t* pixels, size_t buffer_size, uint8_t flip_vertically, image_channels_e force_channels);
void image_destroy(image_t* image);
void image_save(image_t* image, image_filetype_e type, const char* filename);
