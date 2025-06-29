#include "random.h"
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

uint32_t current_seed = 0;

void random_set_seed(uint32_t seed)
{
	srand(seed);
	current_seed = seed;
}

void random_set_random_seed()
{
	current_seed = time(0);
	random_set_seed(current_seed);
}

uint32_t random_get_seed()
{
	return current_seed;
}

float random_float(float min, float max)
{
	if(current_seed == 0)
		random_set_seed(time(0));

	return (((float)rand() / (float)RAND_MAX) * (max - min)) + min;
}

double random_double(double min, double max)
{
	if(current_seed == 0)
		random_set_seed(time(0));

	return (((double)rand() / (double)RAND_MAX) * (max - min)) + min;
}

int32_t random_int32(int32_t min, int32_t max)
{
	if(current_seed == 0)
		random_set_seed(time(0));
	
	return (int32_t)((rand() / (float)RAND_MAX) * (max - min)) + min;
}

uint32_t random_uint32(uint32_t min, uint32_t max)
{
	if(current_seed == 0)
		random_set_seed(time(0));
	
	return (uint32_t)((rand() / (float)RAND_MAX) * (max - min)) + min;
}

int64_t random_int64(int64_t min, int64_t max)
{
    return min + (rand() % (max - min));
}

uint64_t random_uint64(uint64_t min, uint64_t max)
{
    return min + (rand() % (max - min));
}

uuid_t random_uuid()
{
	if(current_seed == 0)
		random_set_seed(time(0));
	
	char v[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	//3fb17ebc-bc38-4939-bc8b-74f2443281d4
	//8 dash 4 dash 4 dash 4 dash 12
	//char buf[37] = {0};
	uuid_t uuid = {0};
	
	//gen random for all spaces because lazy
	for(int i = 0; i < 36; ++i)
	{
	    uuid.string[i] = v[rand()%16];
	}
	
	//put dashes in place
	uuid.string[8] = '-';
	uuid.string[13] = '-';
	uuid.string[18] = '-';
	uuid.string[23] = '-';
	
	//needs end byte
	uuid.string[36] = '\0';
	
	return uuid;
}

uint64_t random_hash_from_string(const char* string)
{
    uint64_t hash = 14695981039346656037U;//FNV-1a
    unsigned char c;

    while ((c = *string++))
    {
        hash ^= c;
        hash *= 1099511628211U;// FNV-1a prime
    }
    hash ^= hash >> 33;
    hash *= 0xff51afd7ed558ccd;
    hash ^= hash >> 33;

    return hash & 0xFFFFFFFFFFFFFFFF;
}


uint64_t current_perlin_seed = 0;

static int __hash[] = {208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,69,255,247,247,40,
                     185,248,251,176,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
                     9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
                     70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
                     203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
                     164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
                     228,108,33,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
                     232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
                     193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
                     101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
                     135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
                     114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

int __noise2(int x, int y)
{
    int tmp = __hash[(y + current_perlin_seed) % 256];
    return __hash[(tmp + x + current_perlin_seed) % 256];
}

double __lin_inter(double x, double y, double s)
{
    return x + s * (y-x);
}

double __smooth_inter(double x, double y, double s)
{
    return __lin_inter(x, y, s * s * (3-2*s));
}

double __noise2d(double x, double y)
{
    int x_int = x;
    int y_int = y;
    double x_frac = x - x_int;
    double y_frac = y - y_int;
    int s = __noise2(x_int, y_int);
    int t = __noise2(x_int+1, y_int);
    int u = __noise2(x_int, y_int+1);
    int v = __noise2(x_int+1, y_int+1);
    double low = __smooth_inter(s, t, x_frac);
    double high = __smooth_inter(u, v, x_frac);
    return __smooth_inter(low, high, y_frac);
}

void random_perlin2d_set_seed(uint64_t seed)
{
	current_perlin_seed = seed;
}

uint64_t random_perlin2d_get_seed()
{
	return current_perlin_seed;
}


double random_perlin2d(double x, double y, double freq, uint32_t depth)
{
    double xa = x*freq;
    double ya = y*freq;
    double amp = 1.0;
    double fin = 0;
    double div = 0.0;

    uint32_t i;
    for(i = 0; i < depth; i++)
    {
        div += 256 * amp;
        fin += __noise2d(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin/div;
}
