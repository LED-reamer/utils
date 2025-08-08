// on linux link with -ldl

#include "utils/dynamic_library.h"
#include "utils/logging.h"

int main(void) {
	dynamic_library_t* dl = dynamic_library_load("./dynamic_library.so");

	char* ret = dynamic_library_error();
	if (ret != NULL) {
		ERROR("%s", ret);
	}

	int (*my_func)(int, int) = dynamic_library_get_symbol(dl, "example_func");
	LOG("dynamic_library function: %i + %i = %i", 99, 1, my_func(99, 1));

	int* dl_data = dynamic_library_get_symbol(dl, "example_data");
	LOG("data in dynamic_library was %i, %i and %i", dl_data[0], dl_data[1], dl_data[2]);

	dynamic_library_close(dl);
	return 0;
}
