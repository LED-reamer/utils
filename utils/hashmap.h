#pragma once
#include "allocator.h"
#include "linked_list.h"


typedef uint64_t hashmap_key_t;

typedef struct
{
	hashmap_key_t key;
	void* value_ptr;
}hashmap_bucket_t;

typedef struct
{
	allocator_t* allocator;
	linked_list_t buckets;
}hashmap_t;

hashmap_t hashmap_create(allocator_t* allocator);
void hashmap_destroy(hashmap_t* hashmap);
void hashmap_set(hashmap_t* hashmap, hashmap_key_t key, void* value_ptr);
void* hashmap_get(hashmap_t* hashmap, hashmap_key_t key);
void hashmap_remove(hashmap_t* hashmap, hashmap_key_t key);
bool hashmap_contains_key(hashmap_t* hashmap, hashmap_key_t key);
