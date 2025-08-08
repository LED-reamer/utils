// link with -lm
#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "allocator.h"

#define JPG_QUALITY 50	// 0 - 100, higher number -> better quality but bigger size
#define IMAGE_AUTO 0
typedef enum {
	IMAGE_CHANNELS_AUTO = IMAGE_AUTO,
	IMAGE_GREY = 1,
	IMAGE_GREY_ALPHA = 2,
	IMAGE_RGB = 3,
	IMAGE_RGBA = 4,
} image_channels_e;

typedef enum {
	IMAGE_CHANNEL_SIZE_AUTO = IMAGE_AUTO,
	IMAGE_8BIT = 1,
	IMAGE_16BIT = 2,
	IMAGE_FLOAT32 = 4,
} image_channel_size_e;

typedef enum {
	IMAGE_PNG,
	IMAGE_BMP,
	IMAGE_TGA,
	IMAGE_JPG,
	IMAGE_HDR,
} image_filetype_e;

typedef struct {
	allocator_t* allocator;
	uint32_t width, height;
	image_channels_e channels;
	image_channel_size_e channel_size;
	size_t buffer_size;	 // kinda unnecessary but convenient
	void* pixels;
} image_t;

image_t image_create(allocator_t* allocator, uint32_t width, uint32_t height, image_channels_e channels, image_channel_size_e channel_size);
image_t image_create_from_file(allocator_t* allocator, const char* filename, bool flip_vertically, image_channels_e force_channels, image_channel_size_e force_channel_size);
image_t image_create_from_memory(allocator_t* allocator, void* buffer, size_t buffer_size, bool flip_vertically, image_channels_e force_channels, image_channel_size_e force_channel_size);
void image_destroy(image_t* image);
void image_save(image_t* image, image_filetype_e filetype, const char* filename, bool flip_vertically);
