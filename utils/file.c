#include "file.h"
#include <stdlib.h>
#include <unistd.h>

path_t path_get_working_directory(){
	path_t new_path = {0};
	getcwd(new_path.name, PATH_MAX);
	return new_path;
}


path_t path_get_absolute(const char* path){
	path_t new_path = {0};
	realpath(path, new_path.name);
	return new_path;
}

bool path_move(const char* path, const char* new_path){
	if(rename(path, new_path) != 0) return false;
	return true;
}

file_t file_create(const char* filename){
	file_t file = {0};
	file.ptr = fopen(filename, "wb+");
	if(file.ptr == NULL) return (file_t){0};
	return file;
}

file_t file_open(const char* filename){
	file_t file = {0};
	file.ptr = fopen(filename, "rb+");
	if(file.ptr == NULL) return (file_t){0};
	return file;
}

file_t file_from_file_ptr(FILE* ptr){
	if(ptr == NULL) return (file_t){0};
	file_t file = {0};
	file.ptr = ptr;
	return file;
}

void file_remove(const char* filename){
	remove(filename);
}

bool file_write(file_t* file, void* data, size_t size){
	size_t bytes_written = fwrite(data, size, 1, file->ptr);
	if(bytes_written != size) return false;
	return true;
}

bool file_read(file_t* file, void* data, size_t size){
	size_t bytes_read = fread(data, size, 1, file->ptr);
	if(bytes_read != size) return false;
	return true;
}

size_t file_get_position(file_t* file){
	long pos = ftell(file->ptr);
	if(pos == -1) return 0;
	return pos;
}

void file_set_position(file_t* file, size_t offset){
	int ret = fseek(file->ptr, offset, SEEK_SET);
	if (ret == -1) {
		 fseek(file->ptr, file_get_size(file), SEEK_SET);
	}
}

size_t file_get_size(file_t* file){
	long old_pos = ftell(file->ptr);
	fseek(file->ptr, 0L, SEEK_END);
	long size = ftell(file->ptr);
	fseek(file->ptr, old_pos, SEEK_SET);//reset to old position
	return size;
}

bool file_close(file_t* file){
	if(file->ptr == NULL) return false;
	if(fclose(file->ptr) != 0) return false;
	return true;
}
