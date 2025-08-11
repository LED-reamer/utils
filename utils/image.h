// link with -lm
#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "allocator.h"

#define JPG_QUALITY 50	// 0 - 100, higher number -> better quality but bigger size
#define IMAGE_AUTO 0

//determines number of channels not order
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

typedef enum {
	IMAGE_FILTER_AUTO         = IMAGE_AUTO,
	IMAGE_FILTER_BOX          = 1,  // A trapezoid w/1-pixel wide ramps, same result as box for integer scale ratios
	IMAGE_FILTER_TRIANGLE     = 2,  // On upsampling, produces same results as bilinear texture filtering
	IMAGE_FILTER_CUBICBSPLINE = 3,  // The cubic b-spline (aka Mitchell-Netrevalli with B=1,C=0), gaussian-esque
	IMAGE_FILTER_CATMULLROM   = 4,  // An interpolating cubic spline
	IMAGE_FILTER_MITCHELL     = 5,  // Mitchell-Netrevalli filter with B=1/3, C=1/3
	IMAGE_FILTER_POINT_SAMPLE = 6,  // Simple point sampling
} image_filter_e;

typedef struct {
	allocator_t* allocator;
	uint32_t width, height;
	image_channels_e channels;
	image_channel_size_e channel_size;
	size_t buffer_size;	 // kinda unnecessary but convenient
	void* pixels;

	bool channels_reversed;
} image_t;

image_t image_create(allocator_t* allocator, uint32_t width, uint32_t height, image_channels_e channels, image_channel_size_e channel_size);
image_t image_create_from_file(allocator_t* allocator, const char* filename, bool flip_vertically, image_channels_e force_channels, image_channel_size_e force_channel_size);
image_t image_create_from_memory(allocator_t* allocator, void* buffer, size_t buffer_size, bool flip_vertically, image_channels_e force_channels, image_channel_size_e force_channel_size);
image_t image_copy(allocator_t* allocator, image_t* source_image);
void image_destroy(image_t* image);
void image_save(image_t* image, image_filetype_e filetype, const char* filename, bool flip_vertically);

void image_resize(image_t* image, uint32_t new_width, uint32_t new_height, image_filter_e filtering);

// utility

//for example rgba to abgr
void image_reverse_channels(image_t* image);
//gamma and perceptual luminance corrected greyscale image (linear approximation)
void image_greyscale(image_t* image);

void image_invert(image_t* image);
