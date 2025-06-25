#pragma once
#include <stdint.h>

void random_set_seed(uint32_t seed);
uint32_t random_get_seed();
float random_float(float min, float max);
double random_double(double min, double max);
int32_t random_int32(int32_t min, int32_t max);
uint32_t random_uint32(uint32_t min, uint32_t max);
int64_t random_int64(int64_t min, int64_t max);
uint64_t random_uint64(uint64_t min, uint64_t max);


typedef struct
{
	char string[37];
}uuid_t;
uuid_t random_uuid();

uint64_t random_hash_from_string(const char* string);

void random_perlin2d_set_seed(uint64_t seed);
uint64_t random_perlin2d_get_seed();
double random_perlin2d(double x, double y, double freq, uint32_t depth);
