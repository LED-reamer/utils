#include "array.h"
#include "logging.h"

#include <string.h>

typedef struct
{
	allocator_t* allocator;
	size_t item_count;
	size_t item_size;  // fixed value
	size_t chunk_size; // fixed value
}array_header_t;

#define array_header(a) ((array_header_t *)(a)-1)

void* __array_create(allocator_t* allocator, size_t item_size, size_t item_count, size_t items_per_chunk){
	void *ptr = 0;
	array_header_t* header = allocator->amalloc(sizeof(array_header_t));
	header->allocator = allocator;

	if(header){
	  header->item_count = 0;
	  header->item_size = item_size;
	  header->chunk_size = item_size * items_per_chunk;
	  ptr = header + 1;
	}
	ptr = array_resize(ptr, item_count);
	return ptr;
}

void array_destroy(void *array) { array_header(array)->allocator->afree(array_header(array)); }

size_t __chunks_needed(size_t chunk_size, size_t item_size, size_t item_count){
	if((item_size * item_count) % chunk_size == 0)
		return ((item_size * item_count) / chunk_size);
	else
		return ((item_size * item_count) / chunk_size) + 1;
}

void* array_resize(void* array, size_t new_item_count){
	array_header_t *h = array_header(array);
	//check if resizing is needed
	if(__chunks_needed(h->chunk_size, h->item_size, h->item_count) != __chunks_needed(h->chunk_size, h->item_size, new_item_count)){
		h = array_header(array)->allocator->arealloc(h, sizeof(array_header_t) + h->chunk_size * __chunks_needed(h->chunk_size, h->item_size, new_item_count));

		if (!h){
		  ERROR("Could not allocate extra dynamic array memory");
		  return NULL;
		}
	}

	h->item_count = new_item_count;

	void *ptr = h + 1;
	return ptr;
}

void array_copy_to_ptr(void* array, void* destination){
	array_header_t *h = array_header(array);
	memcpy(destination, array, h->item_count * h->item_size);
}

size_t array_len(void *array){
	return array_header(array)->item_count;
}

size_t array_item_size(void *array){
	return array_header(array)->item_size;
}

allocator_t* array_get_allocator(void* array){
	return array_header(array)->allocator;
}

void array_sort(void* array, int (*func_negative_if_a_less_b_etc)(const void* a, const void* b)){
	qsort(array, array_header(array)->item_count, array_header(array)->item_size, func_negative_if_a_less_b_etc);
}

void array_sort_strings(void* array){
	array_sort(array, array_cmp_func_cstring);
}



//sort compare functions
#define IMPLEMENT_COMPARE_FUNC(type) \
int array_cmp_func_##type(const void* a, const void* b) { \
    type valA = *(const type*)a; \
    type valB = *(const type*)b; \
    return (valA > valB) - (valA < valB); \
}

IMPLEMENT_COMPARE_FUNC(uint8_t)
IMPLEMENT_COMPARE_FUNC(uint16_t)
IMPLEMENT_COMPARE_FUNC(uint32_t)
IMPLEMENT_COMPARE_FUNC(int8_t)
IMPLEMENT_COMPARE_FUNC(int16_t)
IMPLEMENT_COMPARE_FUNC(int32_t)
IMPLEMENT_COMPARE_FUNC(size_t)

int array_cmp_func_cstring(const void* a, const void* b) {
    const char* strA = *(const char**)a;
    const char* strB = *(const char**)b;
    return strcmp(strA, strB);
}

int array_cmp_func_real32(const void* a, const void* b) {
    float valA = *(const float*)a;
    float valB = *(const float*)b;
    if (valA < valB) return -1;
    if (valA > valB) return 1;
    return 0;
}

int array_cmp_func_real64(const void* a, const void* b) {
    double valA = *(const double*)a;
    double valB = *(const double*)b;
    if (valA < valB) return -1;
    if (valA > valB) return 1;
    return 0;
}

#undef IMPLEMENT_COMPARE_FUNC
