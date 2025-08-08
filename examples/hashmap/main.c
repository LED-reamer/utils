//this needs to be compiled with utils/linked_list.c

#include "utils/hashmap.h"
#include "utils/logging.h"
#include "utils/tools/memory_tracker.h"


int main(void){
	memory_tracker_init(false);
	LOG("starting test hashmap:");

	hashmap_t hashmap = hashmap_create(allocator_get_default());

	LOG("adding some (normally you would add pointers that point to some objects...)");
	hashmap_set(&hashmap, 0, (void*)0);
	hashmap_set(&hashmap, 1, (void*)111);
	hashmap_set(&hashmap, 2, (void*)222);
	hashmap_set(&hashmap, 3, (void*)333);

	void* data = hashmap_get(&hashmap, 2);
	if(data == NULL) ERROR("ha1");
	LOG("getting data of 2 (should be 222 or in a real world application some pointer...): %lu", (uint64_t)data);

	LOG("removing some");
	hashmap_remove(&hashmap, 0);
	hashmap_remove(&hashmap, 1);
	hashmap_remove(&hashmap, 3);
	
	data = hashmap_get(&hashmap, 2);
	if(data == NULL) ERROR("ha");
	LOG("getting data of 2 (should be 222): %lu", (uint64_t)data);
	
	hashmap_destroy(&hashmap);
	memory_tracker_deinit();
	return 0;
}
