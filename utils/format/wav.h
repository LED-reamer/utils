#pragma once
#include "../allocator.h"
#include "../buffer.h"

typedef enum {
	WAV_FORMAT_INTEGER,
	WAV_FORMAT_FLOAT,
} wav_audio_format_e;

typedef struct {
	allocator_t* allocator;
	wav_audio_format_e audio_format;
	uint16_t num_channels;
	uint32_t frequency;			// hertz
	uint32_t bytes_per_second;	// frequency * bytes_per_block
	uint16_t bytes_per_block;	// num_channels * bits_per_sample/8
	uint16_t bits_per_sample;

	buffer_t sample_buffer;
} wav_t;

void wav_load_from_file(wav_t* wav, allocator_t* allocator, const char* filename);
void wav_load_from_memory(wav_t* wav, allocator_t* allocator, void* data, size_t size);

void wav_destroy(wav_t* wav);
