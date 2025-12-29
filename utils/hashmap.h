#pragma once
#include "allocator.h"
#include <stdbool.h>
#include <stdint.h>

//hashmap with open addressing, linear probing and dynamic resizing

typedef uint64_t hashmap_key_t;

typedef struct
{
	hashmap_key_t key;
	void* value_ptr;
}hashmap_bucket_t;

typedef struct
{
	allocator_t* allocator;
	
	size_t size;
	size_t capacity;//num of elements it can hold
	hashmap_bucket_t** buckets;
}hashmap_t;

hashmap_t hashmap_create(allocator_t* allocator);
void hashmap_destroy(hashmap_t* hashmap);
void hashmap_set(hashmap_t* hashmap, hashmap_key_t key, void* value_ptr);
void* hashmap_get(hashmap_t* hashmap, hashmap_key_t key);
void hashmap_remove(hashmap_t* hashmap, hashmap_key_t key);
void* hashmap_pop(hashmap_t* hashmap, hashmap_key_t key);//returns value_ptr and remove in one step
void hashmap_clear(hashmap_t* hashmap);
bool hashmap_contains_key(hashmap_t* hashmap, hashmap_key_t key);

#define hashmap_foreach(hashmap_ptr, entry) \
	hashmap_bucket_t* entry; \
    for (size_t __i = 0; __i < (hashmap_ptr)->capacity; __i++) \
        if ((entry = (hashmap_ptr)->buckets[__i]) != NULL)

#define hashmap_foreach_reverse(hashmap_ptr, entry) \
	hashmap_bucket_t* entry; \
    for (size_t __i = (hashmap_ptr)->capacity; __i-- > 0;) \
        if ((entry = (hashmap_ptr)->buckets[__i]) != NULL)
