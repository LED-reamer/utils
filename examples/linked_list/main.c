#include "utils/linked_list.h"
#include "utils/logging.h"
#include "utils/tools/memory_tracker.h"


//this is not all the functionality. see linked_list.h

int main(void){
	memory_tracker_init(false);
	LOG("starting test linked_list:");

	linked_list_t list = linked_list_create(allocator_get_default());

	LOG("adding some values");
	uint32_t num1 = 111111;
	uint32_t num2 = 222222;
	uint32_t num3 = 333333;
	//add an element at the end of the list
	linked_list_push(&list, &num1, sizeof(uint32_t));
	linked_list_push(&list, &num2, sizeof(uint32_t));

	//add an element at index = 0
	linked_list_add_index(&list, 0, &num3, sizeof(uint32_t));

	LOG("after adding the values the linked_list now holds %zu values", linked_list_len(&list));

	void* data_to_retrieve;
	size_t data_size;
	linked_list_get_index(&list, 0, &data_to_retrieve, &data_size);

	LOG("at index 0: %u with a size of %zu bytes", *(uint32_t*)data_to_retrieve, data_size);

	//remove the last element
	linked_list_pop(&list);
	
	//remove the element at index = 1
	linked_list_remove_index(&list, 1);

	linked_list_clear(&list);
	
	linked_list_destroy(&list);
	memory_tracker_deinit();
	return 0;
}
