#include "utils/random.h"
#include "utils/logging.h"


int main(void){
	LOG(ANSI_GREEN "generate some random values" ANSI_RESET);
	random_set_random_seed();
	float random_value_float = random_float(0, 100);
	double random_value_double = random_double(0, 100);
	int32_t random_value_int32 = random_int32(0, 100);
	uint32_t random_value_uint32 = random_uint32(0, 100);
	int64_t random_value_int64 = random_int64(0, 100);
	uint64_t random_value_uint64 = random_uint64(0, 100);

	LOG_FLOAT(random_value_float);
	LOG_DOUBLE(random_value_double);
	LOG_INT(random_value_int32);
	LOG_UINT(random_value_uint32);
	LOG_INT64(random_value_int64);
	LOG_UINT64(random_value_uint64);

	LOG(ANSI_GREEN"generate some guids" ANSI_RESET);
	uuid_t uuid1 = random_uuid();
	uuid_t uuid2 = random_uuid();
	uuid_t uuid3 = random_uuid();

	LOG("uuid1 = %s", uuid1.string);
	LOG("uuid2 = %s", uuid2.string);
	LOG("uuid3 = %s", uuid3.string);
	
	LOG(ANSI_GREEN"generating some perlin noise" ANSI_RESET);
	random_perlin2d_set_seed(random_uint64(0, UINT64_MAX));

	for(size_t i = 0; i < 20; i++){
		printf("%lf, ", random_perlin2d(i, 15, 8, 8));
	}
	printf("\n");
	
	return 0;
}
