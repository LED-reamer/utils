#pragma once
#include "allocator.h"
#include <stdint.h>


#define array_create(allocator, type, item_count, items_per_chunk) __array_create(allocator, sizeof(type), item_count, items_per_chunk)
#define array_push(array, value) do {\
	(array) = array_resize((array), array_len((array)) + 1);\
	(array)[array_len((array))-1] = value;\
	} while(0)
#define array_pop(array) do {\
		if(array_len(array) != 0)\
		array = array_resize(array, array_len(array)-1);\
	} while(0)
#define array_loop(array, index) for (size_t index = 0; index < array_len(array); index++)
#define array_loop_backwards(array, index) for (size_t index = array_len(array); index-- > 0;)

//use macro instead
void* __array_create(allocator_t* allocator, size_t item_size, size_t item_count, size_t items_per_chunk);

void array_destroy(void* array);
void* array_resize(void* array, size_t new_item_count);
void array_copy_to_ptr(void* array, void* destination);
size_t array_len(void* array);//returns number of reserved items
size_t array_item_size(void *array);
allocator_t* array_get_allocator(void* array);
void array_sort(void* array, int (*func_negative_if_a_less_b_etc)(const void* a, const void* b));//compare func: a>b: positive, a=b: zero, a<b: negative
void array_sort_strings(void* array);


//sort compare functions
int array_cmp_func_cstring(const void* a, const void* b);

int array_cmp_func_uint8_t(const void* a, const void* b);
int array_cmp_func_uint16_t(const void* a, const void* b);
int array_cmp_func_uint32_t(const void* a, const void* b);

int array_cmp_func_int8_t(const void* a, const void* b);
int array_cmp_func_int16_t(const void* a, const void* b);
int array_cmp_func_int32_t(const void* a, const void* b);

int array_cmp_func_size_t(const void* a, const void* b);

int array_cmp_func_real32(const void* a, const void* b);
int array_cmp_func_real64(const void* a, const void* b);
