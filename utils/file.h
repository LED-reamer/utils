#pragma once
#include <stdio.h>
#include <stdbool.h>

//man 3 realpath

typedef struct{
	FILE* ptr;
}file_t;

file_t file_create(const char* filename);
file_t file_open(const char* filename);
file_t file_from_file_ptr(FILE* ptr);
void file_remove(const char* filename);//also removes directories!
bool file_write(file_t* file, void* data, size_t size);
bool file_read(file_t* file, void* data, size_t size);
size_t file_get_position(file_t* file);
void file_set_position(file_t* file, size_t position); //offset relative to file start
size_t file_get_size(file_t* file); //includes EOF
bool file_close(file_t* file);

