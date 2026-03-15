#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>


typedef struct{
	char name[PATH_MAX];
}path_t;

path_t path_get_working_directory();
path_t path_get_absolute(const char* path);//resolves ../ etc
bool path_move(const char* path, const char* new_path);

typedef struct{
	FILE* ptr;
}file_t;

file_t file_create(const char* filename);
file_t file_open(const char* filename);
file_t file_from_file_ptr(FILE* ptr);
void file_remove(const char* filename);//also removes directories!
bool file_write(file_t* file, void* data, size_t size);//advances position
size_t file_read(file_t* file, void* data, size_t size);//returns bytes read
size_t file_read_no_advance(file_t* file, void* data, size_t size);//returns bytes read
size_t file_get_position(file_t* file);
void file_set_position(file_t* file, size_t position); //offset relative to file start
size_t file_get_size(file_t* file); //excludes EOF
bool file_close(file_t* file);

