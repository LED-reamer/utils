#include "wav.h"

#include "../logging.h"

void wav_load_from_file(wav_t* wav, allocator_t* allocator, const char* filename) {
	buffer_t buffer = buffer_create(allocator);
	buffer_load_data_from_file(&buffer, filename);
	wav_load_from_memory(wav, allocator, buffer.data, buffer.size);
	buffer_destroy(&buffer);
}

void wav_load_from_memory(wav_t* wav, allocator_t* allocator, void* data, size_t size) {
	wav->allocator = allocator;
	wav->sample_buffer = buffer_create(allocator);

	buffer_t buffer = buffer_create(allocator);
	buffer_copy_data(&buffer, data, size);

	// skip first 20 bytes
	buffer_read_buffer(&buffer, 20);

	uint16_t audio_format = buffer_read(&buffer, uint16_t);
	if (audio_format == 1)
		wav->audio_format = WAV_FORMAT_INTEGER;
	else if (audio_format == 3)
		wav->audio_format = WAV_FORMAT_FLOAT;
	else
		ERROR("Invalid .wav file");

	wav->num_channels = buffer_read(&buffer, uint16_t);
	wav->frequency = buffer_read(&buffer, uint32_t);
	wav->bytes_per_second = buffer_read(&buffer, uint32_t);
	wav->bytes_per_block = buffer_read(&buffer, uint16_t);
	wav->bits_per_sample = buffer_read(&buffer, uint16_t);

	// skip next 4 bytes
	buffer_read_buffer(&buffer, 4);
	uint32_t sampled_data_size = buffer_read(&buffer, uint32_t);

	buffer_copy_data(&wav->sample_buffer, buffer_read_buffer(&buffer, sampled_data_size), sampled_data_size);

	buffer_destroy(&buffer);
}

void wav_destroy(wav_t* wav) {
	buffer_destroy(&wav->sample_buffer);
}
