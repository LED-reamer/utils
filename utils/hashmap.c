#include "hashmap.h"

hashmap_t hashmap_create(allocator_t* allocator)
{
	hashmap_t hashmap = {
		.allocator = allocator,
		.buckets = linked_list_create(allocator),
	};
	
	return hashmap;
}

void hashmap_destroy(hashmap_t* hashmap)
{
	linked_list_destroy(hashmap->buckets);

	*hashmap = (hashmap_t){0};
}

void hashmap_set(hashmap_t* hashmap, hashmap_key_t key, void* value_ptr)
{
	hashmap_bucket_t* data;
	size_t size;
	linked_list_foreach(hashmap->buckets, &data, &size)
	{
		if(data->key == key)//overwrite
		{
			data->value_ptr = value_ptr;
			return;
		}
	}

	//key is not duplicate
	hashmap_bucket_t bucket = (hashmap_bucket_t){ 
		.key = key, 
		.value_ptr = value_ptr
	};
	linked_list_push(hashmap->buckets, &bucket, sizeof(hashmap_bucket_t));
	
}

void* hashmap_get(hashmap_t* hashmap, hashmap_key_t key)
{
	hashmap_bucket_t* data;
	size_t size;
	linked_list_foreach(hashmap->buckets, &data, &size)
	{
		if(data->key == key)
			return data->value_ptr;
	}
	return NULL;
}

void hashmap_remove(hashmap_t* hashmap, hashmap_key_t key)
{
	hashmap_bucket_t* data;
	size_t size;
	size_t index = 0;
	linked_list_foreach(hashmap->buckets, &data, &size)
	{
		if(data->key == key)
			linked_list_remove_index(hashmap->buckets, index);

		index++;
	}
}

bool hashmap_contains_key(hashmap_t* hashmap, hashmap_key_t key)
{
	hashmap_bucket_t* data;
	size_t size;
	linked_list_foreach(hashmap->buckets, &data, &size)
	{
		if(data->key == key)
			return true;
	}
	
	return false;
}
