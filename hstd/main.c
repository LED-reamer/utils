#define ALLOCATOR_DEBUG
#include "allocator.h"
#define ARRAY_REALLOC allocator.realloc
#define ARRAY_FREE allocator.free
#include "logger.h"
#include "types.h"
#include "array.h"

MAKE_ARRAY(ints, int);

int main(){
	ints arr = {0};

	array_push(&arr, 1);
	array_push(&arr, 2);
	array_push(&arr, 3);
	array_push(&arr, 4);

	array_fit_size(&arr);

	for(size_t i = 0; i < array_len(&arr); i++)
		logger.info("%i has size allocated %zu", arr.items[i], array_size(&arr));

	array_free(&arr);
	
	return 0;
}
