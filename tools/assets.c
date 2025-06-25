#include "assets.h"

#include "../logging.h"
#include "../random.h"
#include <zip.h>

size_t __string_to_asset_id(const char* string) { return random_hash_from_string(string); }

typedef struct
{
	const char* display_name;
	size_t type;
	size_t size;
	void* data;
}__asset_t;


asset_manager_t* asset_manager_create(allocator_t* allocator, const char* archive_directory, const char* password)
{
	asset_manager_t* asset_manager = allocator->malloc(sizeof(asset_manager_t));
	asset_manager->allocator = allocator;
	asset_manager->use_archive = false;
	
	if(archive_directory != NULL)
		asset_manager->use_archive = true;
	
	if(asset_manager->use_archive)
	{
		int32_t error = 0;
		asset_manager->archive = zip_open(archive_directory, ZIP_RDONLY, &error);
		if (error != 0)
		{
			ERROR("Could not load archive %s", archive_directory);
			return NULL;
		}
		error = 0;
		
		error = zip_set_default_password((zip_t*)asset_manager->archive, password);
		if (error != 0)
		{
			ERROR("Could not decrypt archive %s", archive_directory);
			return NULL;
		}
	}

	asset_manager->asset_count = 0;
	asset_manager->assets = hashmap_create(asset_manager->allocator);
	return asset_manager;
}

void asset_manager_destroy(asset_manager_t* asset_manager)
{
	asset_manager_remove_all_assets(asset_manager);
	if(asset_manager->use_archive)
		zip_close(asset_manager->archive);
	hashmap_destroy(asset_manager->assets);
	asset_manager->allocator->free(asset_manager);
}

void asset_manager_load_file(asset_manager_t* asset_manager, buffer_t* buffer, const char* filepath)
{
	if(asset_manager->use_archive)
	{
		zip_file_t* zip_file = zip_fopen_encrypted(asset_manager->archive, filepath, 0, NULL);

		if (!zip_file)
		{
			zip_close(asset_manager->archive);
			ERROR("Could not load archived file: %s, %s", filepath, zip_strerror(asset_manager->archive));
			return;
		}
		
		zip_stat_t file_info;
		zip_stat(asset_manager->archive, filepath, 0, &file_info);

		
		buffer_reserve_memory(buffer, file_info.size);
		zip_fread(zip_file, buffer->data, file_info.size);
		zip_fclose(zip_file);
	}
	else
	{
		buffer_free_data(buffer);
		if(buffer_load_data_from_file(buffer, filepath) != BUFFER_OK)
			ERROR("Could not load file %s", filepath);
	}
}

void asset_manager_load_asset(asset_manager_t* asset_manager, const char* filepath, size_t type, const char* display_name)
{
	__asset_t* new_asset = asset_manager->allocator->malloc(sizeof(__asset_t));
	new_asset->display_name = display_name;
	new_asset->type = type;

	buffer_t* file_buffer = buffer_create(asset_manager->allocator);
	asset_manager_load_file(asset_manager, file_buffer, filepath);

	new_asset->size = file_buffer->size;
	new_asset->data = asset_manager->allocator->malloc(new_asset->size);

	hashmap_set(asset_manager->assets, __string_to_asset_id(display_name), new_asset);
	buffer_destroy(file_buffer);

	asset_manager->asset_count++;
}

bool asset_manager_get_asset(asset_manager_t* asset_manager, const char* display_name, void** data, size_t* size)
{
	__asset_t* asset = (__asset_t*)hashmap_get(asset_manager->assets, __string_to_asset_id(display_name));

	if(asset->data == NULL)
	{
		*data = NULL;
		*size = 0;
		return false;
	}

	*data = asset->data;
	*size = asset->size;

	return true;
}

void asset_manager_remove_asset(asset_manager_t* asset_manager, const char* display_name)
{
	void* data;
	size_t size;
	bool result = asset_manager_get_asset(asset_manager, display_name, &data, &size);
	if(result == false) return;
	asset_manager->allocator->free(data);
	__asset_t* asset = (__asset_t*)hashmap_get(asset_manager->assets, __string_to_asset_id(display_name));
	asset_manager->allocator->free(asset);
	hashmap_remove(asset_manager->assets, __string_to_asset_id(display_name));
	
	asset_manager->asset_count--;
}

void asset_manager_remove_all_assets(asset_manager_t* asset_manager)
{
	size_t size;
	hashmap_bucket_t* ptr;
	linked_list_foreach_backwards(asset_manager->assets->buckets, (void*)&ptr, &size)
	{
		asset_manager->allocator->free(ptr->value_ptr);
	}
	linked_list_clear(asset_manager->assets->buckets);

	asset_manager->asset_count = 0;
}
