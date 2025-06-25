#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rd-party/stb_image.h"
#include "3rd-party/stb_image_write.h"
#include "logging.h"

#include <string.h>

#define MALLOC(size) image->allocator->malloc(size)
#define FREE(ptr) image->allocator->free(ptr)

image_t* image_load(allocator_t* allocator, const char* filename, uint8_t flip_vertically, image_channels_e force_channels)
{
	image_t* image = allocator->malloc(sizeof(image_t));
	if(image == NULL) return NULL;
	image->allocator = allocator;
	
	image->bits_per_channel = 8;
	int channels;
	stbi_set_flip_vertically_on_load(flip_vertically);
	uint8_t* data = stbi_load(filename, (int32_t*)(&image->width), (int32_t*)(&image->height), &channels, force_channels);
	if(data == NULL)
	{
		ERROR("Couldn't load image \"%s\"", filename);
		return NULL;
	}
	if(channels > 4) return NULL;
	if(channels < force_channels) channels = force_channels;
	image->channels = channels;

	size_t data_size = image->width * image->height * image->channels * (image->bits_per_channel / 8);
	image->pixels = MALLOC(data_size);
	memcpy(image->pixels, data, data_size);
	stbi_image_free(data);
	
	return image;
}

image_t* image_load_from_memory(allocator_t* allocator, uint8_t* pixels, size_t buffer_size, uint8_t flip_vertically, image_channels_e force_channels)
{
	image_t* image = allocator->malloc(sizeof(image_t));
	if(image == NULL) return NULL;
	image->allocator = allocator;
	
	image->bits_per_channel = 8;
	int channels;
	stbi_set_flip_vertically_on_load(flip_vertically);
	uint8_t* data = stbi_load_from_memory(pixels, buffer_size, (int32_t*)(&image->width), (int32_t*)(&image->height), &channels, force_channels);
	if(data == NULL)
	{
		ERROR("Couldn't load image from memory");
		return NULL;
	}
	if(channels > 4) return NULL;
	if(channels < force_channels) channels = force_channels;
	image->channels = channels;

	size_t data_size = image->width * image->height * image->channels * (image->bits_per_channel / 8);
	image->pixels = MALLOC(data_size);
	memcpy(image->pixels, data, data_size);
	stbi_image_free(data);
	
	return image;
}

image_t* image_load_16bit(allocator_t* allocator, const char* filename, uint8_t flip_vertically, image_channels_e force_channels)
{
	image_t* image = allocator->malloc(sizeof(image_t));
	if(image == NULL) return NULL;
	image->allocator = allocator;
	
	image->bits_per_channel = 16;
	int channels;
	stbi_set_flip_vertically_on_load(flip_vertically);
	uint16_t* data = stbi_load_16(filename, (int32_t*)(&image->width), (int32_t*)(&image->height), &channels, force_channels);
	if(data == NULL)
	{
		ERROR("Couldn't load image \"%s\"", filename);
		return NULL;
	}
	if(channels > 4) return NULL;
	if(channels < force_channels) channels = force_channels;
	image->channels = channels;

	size_t data_size = image->width * image->height * image->channels * (image->bits_per_channel / 8);
	image->pixels = MALLOC(data_size);
	memcpy(image->pixels, data, data_size);
	stbi_image_free(data);
	
	return image;
}

image_t* image_load_from_memory_16bit(allocator_t* allocator, uint8_t* pixels, size_t buffer_size, uint8_t flip_vertically, image_channels_e force_channels)
{
	image_t* image = allocator->malloc(sizeof(image_t));
	if(image == NULL) return NULL;
	image->allocator = allocator;
	
	image->bits_per_channel = 16;
	int channels;
	stbi_set_flip_vertically_on_load(flip_vertically);
	uint16_t* data = stbi_load_16_from_memory(pixels, buffer_size, (int32_t*)(&image->width), (int32_t*)(&image->height), &channels, force_channels);
	if(data == NULL)
	{
		ERROR("Couldn't load image from memory");
		return NULL;
	}
	if(channels > 4) return NULL;
	if(channels < force_channels) channels = force_channels;
	image->channels = channels;

	size_t data_size = image->width * image->height * image->channels * (image->bits_per_channel / 8);
	image->pixels = MALLOC(data_size);
	memcpy(image->pixels, data, data_size);
	stbi_image_free(data);
	
	return image;
}

void image_destroy(image_t* image)
{
	FREE(image->pixels);
	FREE(image);
}

void image_save(image_t* image, image_filetype_e type, const char* filename)
{
	switch(type)
	{
		case IMAGE_PNG:
			if(!stbi_write_png(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels, image->width * image->channels * (image->bits_per_channel / 8)))
			{
				ERROR("Couldn't save .png \"%s\"", filename);
			}
			break;
		case IMAGE_BMP:
			if(!stbi_write_bmp(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels))
			{
				ERROR("Couldn't save .bmp \"%s\"", filename);
			}
			break;
		case IMAGE_TGA:
			if(!stbi_write_tga(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels))
			{
				ERROR("Couldn't save .tga \"%s\"", filename);
			}
			break;
		case IMAGE_JPG:
			if(!stbi_write_jpg(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels, JPG_QUALITY))
			{
				ERROR("Couldn't save .jpg \"%s\"", filename);
			}
			break;
		case IMAGE_HDR:
			if(image->bits_per_channel != 16)
			{
				ERROR("Couldn't save as .hdr since 16 bits_per_channel are required \"%s\"", filename);
				break;
			}
			if(!stbi_write_hdr(filename, (int32_t)image->width, (int32_t)image->height, image->channels, (float*)image->pixels))
			{
				ERROR("Couldn't save .hdr \"%s\"", filename);
			}
			break;
		default:
			break;
	}
}
