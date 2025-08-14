#include "hashmap.h"
#include <string.h>

#define FIRST_TABLE_CAPACITY 16
#define LOAD_FACTOR_UP 0.75
#define LOAD_FACTOR_DOWN 0.25

hashmap_t hashmap_create(allocator_t* allocator) {
	hashmap_t hashmap = {
		.allocator = allocator,
		.size = 0,
		.capacity = 0,
	};

	return hashmap;
}

void hashmap_destroy(hashmap_t* hashmap) {
	for (size_t i = 0; i < hashmap->capacity; i++) {
	    if (hashmap->buckets[i] != NULL) {
	        hashmap->allocator->afree(hashmap->buckets[i]);
	    }
	}
	if(hashmap->buckets != NULL)
		hashmap->allocator->afree(hashmap->buckets);

	*hashmap = (hashmap_t){0};
}

//returns index in array
size_t __hash(hashmap_key_t key, size_t capacity) {
    return key % capacity;
}

void __hashmap_resize(hashmap_t* hashmap) {
	float load_factor;
	if(hashmap->capacity != 0)
		load_factor = (float)hashmap->size / hashmap->capacity;
	else//hashmap was empty until now
		load_factor = LOAD_FACTOR_UP + 0.01;
		
	if (load_factor > LOAD_FACTOR_UP) {
		size_t new_capacity;
		if(hashmap->capacity == 0)
			new_capacity = FIRST_TABLE_CAPACITY;
		else
    		new_capacity = hashmap->capacity * 2;
    	
    	hashmap_bucket_t** new_table = hashmap->allocator->amalloc(new_capacity * sizeof(hashmap_bucket_t*));
    	memset(new_table, 0, new_capacity * sizeof(hashmap_bucket_t*));

    	for (size_t i = 0; i < hashmap->capacity; i++) {
    	    if (hashmap->buckets[i] != NULL) {
    	        size_t new_index = __hash(hashmap->buckets[i]->key, new_capacity);
    	        while (new_table[new_index] != NULL) {
    	            new_index = (new_index + 1) % new_capacity;//linear probing
    	        }
    	        new_table[new_index] = hashmap->buckets[i];
    	    }
    	}

		if(hashmap->capacity != 0)
    		hashmap->allocator->afree(hashmap->buckets);
    	hashmap->buckets = new_table;
    	hashmap->capacity = new_capacity;
    }else if (load_factor < LOAD_FACTOR_DOWN && hashmap->capacity > FIRST_TABLE_CAPACITY) {
        size_t new_capacity = hashmap->capacity / 2;
        hashmap_bucket_t** new_table = hashmap->allocator->amalloc(new_capacity * sizeof(hashmap_bucket_t*));
        memset(new_table, 0, new_capacity * sizeof(hashmap_bucket_t*));

        for (size_t i = 0; i < hashmap->capacity; i++) {
            if (hashmap->buckets[i] != NULL) {
                size_t new_index = __hash(hashmap->buckets[i]->key, new_capacity);
                while (new_table[new_index] != NULL) {
                    new_index = (new_index + 1) % new_capacity;//linear probing
                }
                new_table[new_index] = hashmap->buckets[i];
            }
        }

        hashmap->allocator->afree(hashmap->buckets);
        hashmap->buckets = new_table;
        hashmap->capacity = new_capacity;
    }
}

void hashmap_set(hashmap_t* hashmap, hashmap_key_t key, void* value_ptr) {
	if (hashmap->size >= hashmap->capacity * LOAD_FACTOR_UP) {
        __hashmap_resize(hashmap);
    }

    size_t index = __hash(key, hashmap->capacity);

    //linear probing (open addressing)
    while (hashmap->buckets[index] != NULL) {
        if (hashmap->buckets[index]->key == key) {
            hashmap->buckets[index]->value_ptr = value_ptr;
            return;
        }
        index = (index + 1) % hashmap->capacity;
    }

    hashmap->buckets[index] = hashmap->allocator->amalloc(sizeof(hashmap_bucket_t));
    hashmap->buckets[index]->key = key;
    hashmap->buckets[index]->value_ptr = value_ptr;
    hashmap->size++;
}

void* hashmap_get(hashmap_t* hashmap, hashmap_key_t key) {
	if(hashmap->buckets == NULL) return NULL;
	size_t index = __hash(key, hashmap->capacity);

    while (hashmap->buckets[index] != NULL) {
        if (hashmap->buckets[index]->key == key) {
            return hashmap->buckets[index]->value_ptr;//found key
        }
        index = (index + 1) % hashmap->capacity;
    }
    return NULL;
}

void hashmap_remove(hashmap_t* hashmap, hashmap_key_t key) {
	hashmap_pop(hashmap, key);
}

void* hashmap_pop(hashmap_t* hashmap, hashmap_key_t key){
	if(hashmap->buckets == NULL) return NULL;
	
	size_t index = __hash(key, hashmap->capacity);
	void* value_ptr = NULL;
    while (hashmap->buckets[index] != NULL) {
        if (hashmap->buckets[index]->key == key) {
        	value_ptr = hashmap->buckets[index]->value_ptr;
            hashmap->allocator->afree(hashmap->buckets[index]);
            hashmap->buckets[index] = NULL;
            hashmap->size--;

            __hashmap_resize(hashmap);
            break;//finish searching
        }
        index = (index + 1) % hashmap->capacity;
    }

    //clear memory if nothing is in the hashmap
    if(hashmap->size == 0){
    	hashmap->capacity = 0;
    	hashmap->allocator->afree(hashmap->buckets);
    	hashmap->buckets = NULL;
    }

    return value_ptr;
}

void hashmap_clear(hashmap_t* hashmap){
	for (size_t i = 0; i < hashmap->capacity; i++) {
	    if (hashmap->buckets[i] != NULL) {
	        hashmap->allocator->afree(hashmap->buckets[i]);
	    }
	}
	if(hashmap->buckets != NULL)
		hashmap->allocator->afree(hashmap->buckets);
	hashmap->size = 0;
	hashmap->capacity = 0;
}

bool hashmap_contains_key(hashmap_t* hashmap, hashmap_key_t key) {
	for (size_t i = 0; i < hashmap->capacity; i++) {
	    if (hashmap->buckets[i] != NULL) {
	    	if(hashmap->buckets[i]->key == key)
	        	return true;
	    }
	}

	return false;
}
