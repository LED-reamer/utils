#include "linked_list.h"

#include <string.h>

#include "array.h"
#include "logging.h"

#define LINKED_LIST_CHUNK_SIZE 1024

linked_list_t linked_list_create(allocator_t* allocator) {
	linked_list_t linked_list = (linked_list_t){
		.allocator = allocator,
		.arena = arena_create(allocator, LINKED_LIST_CHUNK_SIZE, 0),
		.first = NULL,
		.last = NULL,
	};

	return linked_list;
}

void linked_list_destroy(linked_list_t* linked_list) {
	arena_destroy(&linked_list->arena);
	*linked_list = (linked_list_t){0};
}

void linked_list_clear(linked_list_t* linked_list) {
	arena_reset(&linked_list->arena);
	linked_list->first = NULL;
	linked_list->last = NULL;
}

size_t linked_list_len(linked_list_t* linked_list) {
	size_t len = 0;

	linked_list_node_t* current = linked_list->first;
	while (current != NULL) {
		len++;
		current = current->next;
	}
	return len;
}

void linked_list_push(linked_list_t* linked_list, void* data, size_t size) {
	linked_list_node_t* new_node = arena_allocate(&linked_list->arena, sizeof(linked_list_node_t));
	new_node->data = arena_allocate(&linked_list->arena, size);
	memcpy(new_node->data, data, size);
	new_node->size = size;
	new_node->previous = linked_list->last;
	new_node->next = NULL;

	if (linked_list->last != NULL) {
		linked_list->last->next = new_node;
	} else {
		linked_list->first = new_node;
	}

	linked_list->last = new_node;
}

void linked_list_pop(linked_list_t* linked_list) {
	linked_list_node_t* last_node = linked_list->last;

	if (last_node == NULL) return;

	if (last_node->previous == NULL) {
		linked_list->first = NULL;
		linked_list->last = NULL;
	} else {
		last_node->previous->next = NULL;
		linked_list->last = last_node->previous;
	}
}

void linked_list_add_index(linked_list_t* linked_list, size_t index, void* data, size_t size) {
	linked_list_node_t* current = linked_list->first;
	size_t index_count = 0;

	linked_list_node_t* new_node = arena_allocate(&linked_list->arena, sizeof(linked_list_node_t));
	new_node->data = arena_allocate(&linked_list->arena, size);
	memcpy(new_node->data, data, size);
	new_node->size = size;

	if (index == 0) {
		new_node->previous = NULL;
		new_node->next = linked_list->first;

		if (linked_list->first != NULL) {
			linked_list->first->previous = new_node;
		}
		linked_list->first = new_node;

		if (linked_list->last == NULL) {
			linked_list->last = new_node;
		}

		return;
	}

	while (current != NULL && index_count < index) {
		current = current->next;
		index_count++;
	}

	if (current == NULL) {
		ERROR("linked_list_add_index failed, index out of range");
		return;
	}

	new_node->previous = current->previous;
	new_node->next = current;

	if (current->previous != NULL) {
		current->previous->next = new_node;
	} else {
		linked_list->first = new_node;
	}

	current->previous = new_node;

	if (new_node->next == NULL) {
		linked_list->last = new_node;
	}
}

void linked_list_remove_index(linked_list_t* linked_list, size_t index) {
	linked_list_node_t* current = linked_list->first;

	size_t index_count = 0;

	while (current != NULL) {
		if (index_count == index) break;
		index_count++;
		current = current->next;
	}

	if (current == NULL) {
		ERROR("linked_list_remove_index failed, index out of range");
		return;
	}

	if (current->previous == NULL) {
		linked_list->first = current->next;
		if (linked_list->first != NULL) {
			linked_list->first->previous = NULL;
		}
	} else {
		current->previous->next = current->next;
		if (current->next != NULL) {
			current->next->previous = current->previous;
		}
	}

	if (current->next == NULL) {
		linked_list->last = current->previous;
	}
}

void linked_list_get_index(linked_list_t* linked_list, size_t index, void** data, size_t* size) {
	linked_list_node_t* current = linked_list->first;

	size_t index_count = 0;
	while (current != NULL) {
		if (index_count == index) break;
		index_count++;
		current = current->next;
	}

	if (current == NULL) {
		ERROR("linked_list_get_index failed, index out of range");
		return;
	}

	*data = current->data;
	if (size != NULL) {
		*size = current->size;
	}
}

size_t __max_size(size_t s1, size_t s2) {
	if (s1 > s2) return s1;
	return s2;
}

size_t linked_list_get_value(linked_list_t* linked_list, void* compare_data, size_t size) {
	linked_list_node_t* current = linked_list->first;

	size_t index_count = 0;

	while (current != NULL) {
		if (memcmp(compare_data, current->data, __max_size(size, current->size)) == 0) {
			return index_count;
		}

		index_count++;
		current = current->next;
	}

	ERROR("linked_list_get_value failed, value not found");
	return (size_t)-1;	// SIZE_MAX as error value
}

bool __linked_list_foreach(linked_list_t* linked_list, void** data, size_t* size, void** next_node, uint8_t* looping_stage) {
	if (linked_list->first == NULL) {
		*data = NULL;
		*size = 0;
		return false;
	}

	if (*next_node == NULL && *looping_stage != 1) {
		*next_node = (void*)linked_list->first;
	}

	if (*next_node != NULL) {
		linked_list_node_t* current_node = (linked_list_node_t*)*next_node;

		*data = current_node->data;
		*size = current_node->size;

		if (current_node->next != NULL) {
			*next_node = (void*)current_node->next;
			return true;
		} else {
			*next_node = NULL;
			*looping_stage = 1;
			return true;
		}
	}

	*data = NULL;
	*size = 0;
	return false;
}

bool __linked_list_foreach_backwards(linked_list_t* linked_list, void** data, size_t* size, void** next_node, uint8_t* looping_stage) {
	if (linked_list->last == NULL) {
		*data = NULL;
		*size = 0;
		return false;
	}

	if (*next_node == NULL && *looping_stage != 1) {
		*next_node = (void*)linked_list->last;
	}

	if (*next_node != NULL) {
		linked_list_node_t* current_node = (linked_list_node_t*)*next_node;

		*data = current_node->data;
		*size = current_node->size;

		if (current_node->previous != NULL) {
			*next_node = (void*)current_node->previous;
			return true;
		} else {
			*next_node = NULL;
			*looping_stage = 1;
			return true;
		}
	}

	*data = NULL;
	*size = 0;
	return false;
}
