#include "utils/tools/memory_tracker.h"

int main(void) {
	memory_tracker_init(false);

	void* data1 = malloc(150);
	void* data2 = realloc(data1, 151);

	free(data2);

	memory_tracker_deinit();
	return 0;
}
