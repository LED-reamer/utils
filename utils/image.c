#include "image.h"

#include "buffer.h"
#include "logging.h"

#define STB_IMAGE_IMPLEMENTATION
#include "3rd-party/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rd-party/stb_image_write.h"

#ifdef IMAGE_RESIZE
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "3rd-party/stb_image_resize2.h"
#endif

#include <string.h>	 //for memcpy
#include <float.h>	 //for memcpy


image_t image_create(allocator_t* allocator, uint32_t width, uint32_t height, image_channels_e channels, image_channel_size_e channel_size) {
	// could throw an error here
	if (width == 0) width = 1;
	if (height == 0) height = 1;
	if (channels == IMAGE_CHANNELS_AUTO) channels = IMAGE_RGBA;
	if (channel_size == IMAGE_CHANNEL_SIZE_AUTO) channel_size = IMAGE_8BIT;

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

image_t image_create_from_file(allocator_t* allocator, const char* filename, bool flip_vertically, image_channels_e force_channels, image_channel_size_e force_channel_size) {
	buffer_t file_buffer = buffer_create(allocator);
	buffer_load_data_from_file(&file_buffer, filename);
	image_t image = image_create_from_memory(allocator, file_buffer.data, file_buffer.size, flip_vertically, force_channels, force_channel_size);
	buffer_destroy(&file_buffer);

	if (image.pixels == NULL) {
		ERROR("Could not load image at \"%s\"", filename);
		image_destroy(&image);
		return (image_t){0};
	}
	return image;
}

image_t image_create_from_memory(allocator_t* allocator, void* buffer, size_t buffer_size, bool flip_vertically, image_channels_e force_channels, image_channel_size_e force_channel_size) {
	image_t image = (image_t){
		.allocator = allocator,
		.channels = force_channels,
	};

	uint8_t desired_channels = force_channels;
	uint8_t desired_channel_size = force_channel_size;

	// defaults
	if (desired_channels == IMAGE_CHANNELS_AUTO)
		desired_channels = IMAGE_RGBA;
	if (desired_channel_size == IMAGE_CHANNEL_SIZE_AUTO) {
		if (stbi_is_16_bit_from_memory(buffer, buffer_size))
			desired_channel_size = IMAGE_16BIT;	 // use 16-bit if available
		else
			desired_channel_size = IMAGE_8BIT;
	}
	if (flip_vertically)
		stbi_set_flip_vertically_on_load(true);

	int channels_in_file = 0;
	void* stbi_data = NULL;
	switch (desired_channel_size) {
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

	if (stbi_data == NULL) {
		ERROR("Could not load image");
		image_destroy(&image);
		return (image_t){0};
	}

	// chose actual format
	if (force_channels == IMAGE_CHANNELS_AUTO) {
		image.channels = channels_in_file;
	}
	image.channel_size = desired_channel_size;
	image.buffer_size = image.width * image.height * image.channels * image.channel_size;

	image.pixels = image.allocator->amalloc(image.buffer_size);
	memcpy(image.pixels, stbi_data, image.buffer_size);
	stbi_image_free(stbi_data);

	return image;
}

image_t image_copy(allocator_t* allocator, image_t* source_image){
	image_t copy = image_create(allocator, source_image->width, source_image->height, source_image->channels, source_image->channel_size);

	copy.buffer_size = source_image->buffer_size;
	if(copy.width * copy.height * copy.buffer_size != 0){
		copy.pixels = copy.allocator->amalloc(copy.buffer_size);
		memcpy(copy.pixels, source_image->pixels, copy.buffer_size);
	}
	
	copy.channels_reversed = source_image->channels_reversed;

	return copy;
}

void image_destroy(image_t* image) {
	if (image->pixels != NULL)
		image->allocator->afree(image->pixels);
	*image = (image_t){0};
}

void image_save(image_t* image, image_filetype_e filetype, const char* filename, bool flip_vertically) {
	if (flip_vertically)
		stbi_flip_vertically_on_write(true);

	switch (filetype) {
		case IMAGE_PNG:
			if (!stbi_write_png(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels, image->width * image->channels * image->channel_size)) {
				ERROR("Couldn't save .png \"%s\"", filename);
			}
			break;
		case IMAGE_BMP:
			if (!stbi_write_bmp(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels)) {
				ERROR("Couldn't save .bmp \"%s\"", filename);
			}
			break;
		case IMAGE_TGA:
			if (!stbi_write_tga(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels)) {
				ERROR("Couldn't save .tga \"%s\"", filename);
			}
			break;
		case IMAGE_JPG:
			if (!stbi_write_jpg(filename, (int32_t)image->width, (int32_t)image->height, image->channels, image->pixels, JPG_QUALITY)) {
				ERROR("Couldn't save .jpg \"%s\"", filename);
			}
			break;
		case IMAGE_HDR:
			if (image->channel_size != 4) {
				ERROR("Couldn't save as .hdr since FLOAT32 channel size is required \"%s\"", filename);
				break;
			}
			if (!stbi_write_hdr(filename, (int32_t)image->width, (int32_t)image->height, image->channels, (float*)image->pixels)) {
				ERROR("Couldn't save .hdr \"%s\"", filename);
			}
			break;
		default:
			UNREACHABLE("Could not recognize filetype");
			break;
	}
}

#ifdef IMAGE_RESIZE
void image_resize(image_t* image, uint32_t new_width, uint32_t new_height, image_filter_e filtering){
	void* resized_pixels = image->allocator->amalloc(new_width * new_height * image->channels * image->channel_size);

	stbir_pixel_layout pixel_layout;
	switch(image->channels){
		case IMAGE_GREY:
			pixel_layout = STBIR_1CHANNEL;
			break;
		case IMAGE_GREY_ALPHA:
			if(image->channels_reversed)
				pixel_layout = STBIR_AR;
			else
				pixel_layout = STBIR_RA;
			break;
		case IMAGE_RGB:
			if(image->channels_reversed)
				pixel_layout = STBIR_BGR;
			else
				pixel_layout = STBIR_RGB;
			break;
		case IMAGE_RGBA:
			if(image->channels_reversed)
				pixel_layout = STBIR_ABGR;
			else
				pixel_layout = STBIR_RGBA;
			break;
		default:
			UNREACHABLE("Unknown image channels");
	}

	stbir_datatype datatype;
	switch(image->channel_size){
		case IMAGE_8BIT:
			datatype = STBIR_TYPE_UINT8;
			break;
		case IMAGE_16BIT:
			datatype = STBIR_TYPE_UINT16;
			break;
		case IMAGE_FLOAT32:
			datatype = STBIR_TYPE_FLOAT;
			break;
		default:
			UNREACHABLE("Unknown channel size");
	}

	stbir_resize(image->pixels, image->width, image->height, 0,
		resized_pixels, new_width, new_height, 0,
		pixel_layout,
		datatype,
		STBIR_EDGE_CLAMP,
		(stbir_filter)filtering);
	
	image->allocator->afree(image->pixels);
	image->pixels = resized_pixels;

	image->width = new_width;
	image->height = new_height;
	image->buffer_size = new_width * new_height * image->channels * image->channel_size;
}
#endif

void image_reverse_channels(image_t* image){
	if(image->channels == 1) return;
	
	for(size_t i = 0; i < image->width * image->height * image->channels; i += image->channels){
		uint8_t copy_buffer[4*4];//biggest possible format RGBA + FLOAT32 -> can hold every color
		memcpy(copy_buffer, image->pixels + i, image->channels);

		for(uint8_t channel = 0; channel < image->channels; channel++){
			memcpy(image->pixels + i + image->channel_size * (image->channels-1)-channel, copy_buffer + image->channel_size * channel, image->channel_size);
		}
	}

	image->channels_reversed = !image->channels_reversed;
}

void image_greyscale(image_t* image){
	//can only convert RGB or RGBA to GREY
	if(image->channels != 3 && image->channels != 4) return;

	image_channels_e grey_channels = (image->channels == IMAGE_RGBA) ? IMAGE_GREY_ALPHA : IMAGE_GREY;
	size_t grey_pixels_size = image->width * image->height * image->channel_size * grey_channels;
		
	void* grey_pixels = image->allocator->amalloc(grey_pixels_size);

	for(size_t i = 0; i < image->width * image->height; i++){
		static uint8_t copy_buffer[4 * 4];//biggest possible color format (float32 (4 bytes) * RGBA)

		memcpy(copy_buffer, image->pixels + i  * image->channels, image->channels * image->channel_size);

		switch(image->channel_size){
			case IMAGE_8BIT:
				((uint8_t*)grey_pixels)[i*grey_channels] = (uint8_t)(UINT8_MAX*((copy_buffer[0]/(float)UINT8_MAX)*0.299 + (copy_buffer[1]/(float)UINT8_MAX)*0.587 + (copy_buffer[2]/(float)UINT8_MAX)*0.114));
				if(grey_channels == IMAGE_GREY_ALPHA){
					if(image->channels_reversed)
						((uint8_t*)grey_pixels)[i*grey_channels+1] = copy_buffer[0];
					else
						((uint8_t*)grey_pixels)[i*grey_channels+1] = copy_buffer[3];
				}
				break;
			case IMAGE_16BIT:
				((uint16_t*)grey_pixels)[i] = (uint16_t)(UINT16_MAX*((((uint16_t*)copy_buffer)[0]/(float)UINT16_MAX)*0.299 + (((uint16_t*)copy_buffer)[1]/(float)UINT16_MAX)*0.587 + (((uint16_t*)copy_buffer)[2]/(float)UINT16_MAX)*0.114));
				if(grey_channels == IMAGE_GREY_ALPHA){
					if(image->channels_reversed)
						((uint16_t*)grey_pixels)[i*grey_channels+1] = ((uint16_t*)copy_buffer)[0];
					else
						((uint16_t*)grey_pixels)[i*grey_channels+1] = ((uint16_t*)copy_buffer)[3];
				}
				break;
			case IMAGE_FLOAT32:
				((float*)grey_pixels)[i] = ((float*)copy_buffer)[0]*0.299 + ((float*)copy_buffer)[1]*0.587 + ((float*)copy_buffer)[2]*0.114;
				if(grey_channels == IMAGE_GREY_ALPHA){
					if(image->channels_reversed)
						((float*)grey_pixels)[i*grey_channels+1] = ((float*)copy_buffer)[0];
					else
						((float*)grey_pixels)[i*grey_channels+1] = ((float*)copy_buffer)[3];
				}
				break;
			default:
			UNREACHABLE("Unknown channel size");
			break;
		}
	}
	
	image->allocator->afree(image->pixels);
	image->pixels = grey_pixels;

	image->channels = grey_channels;
}
