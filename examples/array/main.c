#include "utils/array.h"
#include "utils/logging.h"

int main(void){
	//second parameter is the element type (this is a macro)
	//third parameter defines how many items should already exist in the array
	//second parameter defines how many items should fit in a chunk
	float* my_array = array_create(allocator_get_default(), float, 0, 20);
	array_push(my_array, 17.789f);
	array_push(my_array, 985.0f);
	array_push(my_array, 60.0f);
	array_push(my_array, 78954.0f);
	array_push(my_array, 89.64f);
	array_push(my_array, 34.5f);
	array_push(my_array, 12.000003f);
	array_pop(my_array);
	
	array_sort(my_array, array_cmp_func_real32);
	
	my_array[0] = 123.456f;

	//for (size_t i = 0; i < array_len(my_array); i++)
	// or with macro
	array_loop(my_array, i){
		LOG("my_array[%zu] = %f", i, my_array[i]);
	}
	array_loop_backwards(my_array, i){
		LOG("my_array[%zu] = %f", i, my_array[i]);
	}

	array_destroy(my_array);
	return 0;
}
