#include "linked_list.h"
#include "array.h"
#include "logging.h"

#include <string.h>

struct __node_t
{
    void* data;
    size_t size;
    struct __node_t* previous;
    struct __node_t* next;
};

struct __linked_list_t
{
    allocator_t* allocator;
    struct __node_t* first;
    struct __node_t* last;
};

linked_list_t* linked_list_create(allocator_t* allocator)
{
    struct __linked_list_t* linked_list = allocator->malloc(sizeof(struct __linked_list_t));
    linked_list->allocator = allocator;
    linked_list->first = NULL;
    linked_list->last = NULL;

    return (linked_list_t*)linked_list;
}

void linked_list_destroy(linked_list_t* linked_list)
{
    if(linked_list == NULL) return;
    struct __linked_list_t* list = (struct __linked_list_t*)linked_list;
    struct __node_t* current = list->first;
    
    while (current != NULL)
    {
        struct __node_t* next = current->next;
        list->allocator->free(current->data);
        list->allocator->free(current);
        current = next;
    }
    
    list->first = NULL;
    list->last = NULL;
    list->allocator->free(list);
}

void linked_list_clear(linked_list_t* linked_list)
{
    if(linked_list == NULL) return;
    struct __linked_list_t* list = (struct __linked_list_t*)linked_list;
    struct __node_t* current = list->first;
    
    while (current != NULL)
    {
        struct __node_t* next = current->next;
        list->allocator->free(current->data);
        list->allocator->free(current);
        current = next;
    }
    
    list->first = NULL;
    list->last = NULL;
}

size_t linked_list_len(linked_list_t* linked_list)
{
    struct __linked_list_t* list = (struct __linked_list_t*)linked_list;
    size_t len = 0;
    
    struct __node_t* current = list->first;
    while (current != NULL)
    {
        len++;
        current = current->next;
    }
    return len;
}

void linked_list_push(linked_list_t* linked_list, void* data, size_t size)
{
    struct __linked_list_t* list = (struct __linked_list_t*)linked_list;
    struct __node_t* new_node = list->allocator->malloc(sizeof(struct __node_t));
    new_node->data = list->allocator->malloc(size);
    memcpy(new_node->data, data, size);
    new_node->size = size;
    new_node->previous = list->last;
    new_node->next = NULL;

    if (list->last != NULL)
    {
        list->last->next = new_node;
    }
    else
    {
        list->first = new_node;
    }

    list->last = new_node;
}

void linked_list_pop(linked_list_t* linked_list)
{
    struct __linked_list_t* list = (struct __linked_list_t*)linked_list;
    struct __node_t* last_node = list->last;

    if (last_node == NULL) return;

    if (last_node->previous == NULL)
    {
        list->first = NULL;
        list->last = NULL;
    }
    else
    {
        last_node->previous->next = NULL;
        list->last = last_node->previous;
    }

    list->allocator->free(last_node->data);
    list->allocator->free(last_node);
}

void linked_list_add_index(linked_list_t* linked_list, size_t index, void* data, size_t size)
{
    struct __linked_list_t* list = (struct __linked_list_t*)linked_list;
    struct __node_t* current = list->first;
    size_t index_count = 0;

    struct __node_t* new_node = list->allocator->malloc(sizeof(struct __node_t));
    new_node->data = list->allocator->malloc(size);
    memcpy(new_node->data, data, size);
    new_node->size = size;
    
    if (index == 0)
    {
        new_node->previous = NULL;
        new_node->next = list->first;

        if (list->first != NULL)
        {
            list->first->previous = new_node;
        }
        list->first = new_node;

        if (list->last == NULL)
        {
            list->last = new_node;
        }

        return;
    }

    while (current != NULL && index_count < index)
    {
        current = current->next;
        index_count++;
    }

    if (current == NULL)
    {
        ERROR("linked_list_add_index failed, index out of range");
        return;
    }

    new_node->previous = current->previous;
    new_node->next = current;

    if (current->previous != NULL)
    {
        current->previous->next = new_node;
    }
    else
    {
        list->first = new_node;
    }

    current->previous = new_node;

    if (new_node->next == NULL)
    {
        list->last = new_node;
    }
}

void linked_list_remove_index(linked_list_t* linked_list, size_t index)
{
    struct __linked_list_t* list = (struct __linked_list_t*)linked_list;
    struct __node_t* current = list->first;

    size_t index_count = 0;

    while (current != NULL)
    {
        if (index_count == index) break;
        index_count++;
        current = current->next;
    }

    if (current == NULL)
    {
        ERROR("linked_list_remove_index failed, index out of range");
        return;
    }

    if (current->previous == NULL)
    {
        list->first = current->next;
        if (list->first != NULL)
        {
            list->first->previous = NULL;
        }
    }
    else
    {
        current->previous->next = current->next;
        if (current->next != NULL)
        {
            current->next->previous = current->previous;
        }
    }

    if (current->next == NULL)
    {
        list->last = current->previous;
    }

    list->allocator->free(current->data);
    list->allocator->free(current);
}

void linked_list_get_index(linked_list_t* linked_list, size_t index, void** data, size_t* size)
{
    struct __linked_list_t* list = (struct __linked_list_t*)linked_list;
    struct __node_t* current = list->first;

    size_t index_count = 0;
    while (current != NULL)
    {
        if (index_count == index) break;
        index_count++;
        current = current->next;
    }

    if (current == NULL)
    {
        ERROR("linked_list_get_index failed, index out of range");
        return;
    }

    *data = current->data;
    if (size != NULL)
    {
        *size = current->size;
    }
}

size_t __max_size(size_t s1, size_t s2)
{
    if(s1 > s2) return s1;
    return s2;
}

size_t linked_list_get_value(linked_list_t* linked_list, void* compare_data, size_t size)
{
    struct __linked_list_t* list = (struct __linked_list_t*)linked_list;
    struct __node_t* current = list->first;
    
    size_t index_count = 0;
    
    while (current != NULL)
    {
        if (memcmp(compare_data, current->data, __max_size(size, current->size)) == 0)
        {
            return index_count;
        }
        
        index_count++;
        current = current->next;
    }
    
    ERROR("linked_list_get_value failed, value not found");
    return (size_t)-1;  //SIZE_MAX as error value
}

bool __linked_list_foreach(linked_list_t* linked_list, void** data, size_t* size, void** next_node, uint8_t* looping_stage)
{
    struct __linked_list_t* list = (struct __linked_list_t*)linked_list;

    if (list->first == NULL) {
        *data = NULL;
        *size = 0;
        return false;
    }

    if (*next_node == NULL && *looping_stage != 1)
    {
        *next_node = (void*)list->first;
    }

    if (*next_node != NULL) {
        struct __node_t* current_node = (struct __node_t*)*next_node;

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

bool __linked_list_foreach_backwards(linked_list_t* linked_list, void** data, size_t* size, void** next_node, uint8_t* looping_stage)
{
    struct __linked_list_t* list = (struct __linked_list_t*)linked_list;

    if (list->last == NULL)
    {
        *data = NULL;
        *size = 0;
        return false;
    }

    if (*next_node == NULL && *looping_stage != 1)
    {
        *next_node = (void*)list->last;
    }

    if (*next_node != NULL)
    {
        struct __node_t* current_node = (struct __node_t*)*next_node;

        *data = current_node->data;
        *size = current_node->size;

        if (current_node->previous != NULL)
        {
            *next_node = (void*)current_node->previous;
            return true;
        } 
        else
        {
            *next_node = NULL;
            *looping_stage = 1;
            return true;
        }
    }

    *data = NULL;
    *size = 0;
    return false;
}
