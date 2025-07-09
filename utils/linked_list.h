#pragma once
#include "allocator.h"
#include "arena.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct linked_list_node_t{
    void* data;
    size_t size;
    struct linked_list_node_t* previous;
    struct linked_list_node_t* next;
}linked_list_node_t;

typedef struct{
	allocator_t* allocator;
	arena_t arena;
	linked_list_node_t* first;
	linked_list_node_t* last;
}linked_list_t;

linked_list_t linked_list_create(allocator_t* allocator);
void linked_list_destroy(linked_list_t* linked_list);
void linked_list_clear(linked_list_t* linked_list);
size_t linked_list_len(linked_list_t* linked_list);
void linked_list_push(linked_list_t* linked_list, void* data, size_t size);
void linked_list_pop(linked_list_t* linked_list);
void linked_list_add_index(linked_list_t* linked_list, size_t index, void* data, size_t size);
void linked_list_remove_index(linked_list_t* linked_list, size_t index);

void linked_list_get_index(linked_list_t* linked_list, size_t index, void** data, size_t* size);//does not allocate memory only point to list memory and returns its size
size_t linked_list_get_value(linked_list_t* linked_list, void* compare_data, size_t size);//returns first found index
//fast
bool __linked_list_foreach(linked_list_t* linked_list, void** data, size_t* size, void** next_node, uint8_t* looping_stage);
#define linked_list_foreach(linked_list, data_ptr_ptr, size_ptr) \
    uint8_t __looping_stage = 0; \
    void* __linked_list_next_node = NULL; \
    while(true == __linked_list_foreach(linked_list, (void*)data_ptr_ptr, size_ptr, &__linked_list_next_node, &__looping_stage))

bool __linked_list_foreach_backwards(linked_list_t* linked_list, void** data, size_t* size, void** next_node, uint8_t* looping_stage);
#define linked_list_foreach_backwards(linked_list, data_ptr_ptr, size_ptr) \
    uint8_t __looping_stage = 0; \
    void* __linked_list_next_node = NULL; \
    while(true == __linked_list_foreach_backwards(linked_list, (void*)data_ptr_ptr, size_ptr, &__linked_list_next_node, &__looping_stage))

//macros
//slow -> use foreach
#define linked_list_loop(linked_list, index) for(size_t index = 0; index < linked_list_len(linked_list); index++)
#define linked_list_push_value(linked_list, value) linked_list_push(linked_list, &value, sizeof(value));
#define linked_list_add_index_value(linked_list, value, index) linked_list_push(linked_list, index, &value, sizeof(value));
