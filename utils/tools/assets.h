#pragma once
#include <stdbool.h>

#include "../allocator.h"
#include "../buffer.h"
#include "../hashmap.h"

typedef struct
{
	allocator_t* allocator;
	bool use_archive;
	void* archive;	// opaque pointer (zip_t*)

	size_t asset_count;
	hashmap_t assets;
} asset_manager_t;

asset_manager_t asset_manager_create(allocator_t* allocator, const char* archive_directory /*nullable*/, const char* password /*nullable*/);
void asset_manager_destroy(asset_manager_t* asset_manager);
void asset_manager_load_file(asset_manager_t* asset_manager, buffer_t* buffer, const char* filepath);						 // immediate load
void asset_manager_load_asset(asset_manager_t* asset_manager, const char* filepath, size_t type, const char* display_name);	 // safes as asset
bool asset_manager_get_asset(asset_manager_t* asset_manager, const char* display_name, void** data, size_t* size);
void asset_manager_remove_asset(asset_manager_t* asset_manager, const char* display_name);
void asset_manager_remove_all_assets(asset_manager_t* asset_manager);
