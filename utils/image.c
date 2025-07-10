#include "image.h"
#include "buffer.h"
#include "logging.h"

#define STB_IMAGE_IMPLEMENTATION
#include "3rd-party/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rd-party/stb_image_write.h"

#include <string.h> //for memcpy

image_t image_create(allocator_t* allocator, uint32_t width, uint32_t height, image_channels_e channels, image_channel_size_e channel_size){
	//could throw an error here
	if(width == 0) width = 1;
	if(height == 0) height = 1;
	if(channels == IMAGE_CHANNELS_AUTO) channels = IMAGE_RGBA;
	if(channel_size == IMAGE_CHANNEL_SIZE_AUTO) channel_size = IMAGE_8BIT;

	image_t image = (image_t){
		.allocator = allocator,
		.width = width,
		.height = height,
		.channels = channels,
		.channel_size = channel_size,
		.buffer_size = channels * channel_size * width * height,
	};

	image.pixels = image.allocator->amalloc(image.buffer_size);

	return image;
}

image_t image_create_from_file(allocator_t* allocator, const char* filename, bool flip_vertically, image_channels_e force_channels, image_channel_size_e force_channel_size){
	buffer_t file_buffer = buffer_create(allocator);
	buffer_load_data_from_file(&file_buffer, filename);
	image_t image =  image_create_from_memory(allocator, file_buffer.data, file_buffer.size, flip_vertically, force_channels, force_channel_size);
	buffer_destroy(&file_buffer);

	if(image.pixels == NULL){
		ERROR("Could not load image at \"%s\"", filename);
		image_destroy(&image);
		return (image_t){0};
	}
	return image;
}

image_t image_create_from_memory(allocator_t* allocator, void* buffer, size_t buffer_size, bool flip_vertically, image_channels_e force_channels, image_channel_size_e force_channel_size){
	image_t image = (image_t){
		.allocator = allocator,
		.channels = force_channels,
	};

	uint8_t desired_channels = force_channels;
	uint8_t desired_channel_size = force_channel_size;

	//defaults
	if(desired_channels == IMAGE_CHANNELS_AUTO)
		desired_channels = IMAGE_RGBA;
	if(desired_channel_size == IMAGE_CHANNEL_SIZE_AUTO){
		if(stbi_is_16_bit_from_memory(buffer, buffer_size))
			desired_channel_size = IMAGE_16BIT;//use 16-bit if available
		else
			desired_channel_size = IMAGE_8BIT;
	}
	if(flip_vertically)
		stbi_set_flip_vertically_on_load(true);

	int channels_in_file = 0;
	void* stbi_data = NULL;
	switch(desired_channel_size){
		case IMAGE_8BIT:
			stbi_data = stbi_load_from_memory(buffer, buffer_size, (int32_t*)&image.width, (int32_t*)&image.height, &channels_in_file, desired_channels);
			break;
		case IMAGE_16BIT:
			stbi_data = stbi_load_16_from_memory(buffer, buffer_size, (int32_t*)&image.width, (int32_t*)&image.height, &channels_in_file, desired_channels);
			break;
		case IMAGE_FLOAT32:
			stbi_data = stbi_loadf_from_memory(buffer, buffer_size, (int32_t*)&image.width, (int32_t*)&image.height, &channels_in_file, desired_channels);
			break;
	}

	if(stbi_data == NULL){
		ERROR("Could not load image");
		image_destroy(&image);
		return (image_t){0};
	}

	//chose actual format
	if(force_channels == IMAGE_CHANNELS_AUTO){
		image.channels = channels_in_file;
	}
	image.channel_size = desired_channel_size;
	image.buffer_size = image.width * image.height * image.channels * image.channel_size;

	image.pixels = image.allocator->amalloc(image.buffer_size);
	memcpy(image.pixels, stbi_data, image.buffer_size);
	stbi_image_free(stbi_data);

	return image;
}

void image_destroy(image_t* image){
	if(image->pixels != NULL)
		image->allocator->afree(image->pixels);
	*image = (image_t){0};
}

void image_save(image_t* image, image_filetype_e filetype, const char* filename, bool flip_vertically){
	if(flip_vertically)
		stbi_flip_vertically_on_write(true);

	switch(filetype){
		case IMAGE_PNG:
			if(!stbi_write_png(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels, image->width * image->channels * image->channel_size)){
				ERROR("Couldn't save .png \"%s\"", filename);
			}
			break;
		case IMAGE_BMP:
			if(!stbi_write_bmp(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels)){
				ERROR("Couldn't save .bmp \"%s\"", filename);
			}
			break;
		case IMAGE_TGA:
			if(!stbi_write_tga(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels)){
				ERROR("Couldn't save .tga \"%s\"", filename);
			}
			break;
		case IMAGE_JPG:
			if(!stbi_write_jpg(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels, JPG_QUALITY)){
				ERROR("Couldn't save .jpg \"%s\"", filename);
			}
			break;
		case IMAGE_HDR:
			if(image->channel_size != 4){
				ERROR("Couldn't save as .hdr since FLOAT32 channel size is required \"%s\"", filename);
				break;
			}
			if(!stbi_write_hdr(filename, (int32_t)image->width, (int32_t)image->height, image->channels, (float*)image->pixels)){
				ERROR("Couldn't save .hdr \"%s\"", filename);
			}
			break;
		default:
			UNREACHABLE("Could not recognize filetype");
			break;
	}
}
