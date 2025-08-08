#include "dynamic_library.h"

#ifdef _WIN32

// WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>

dynamic_library_t dynamic_library_load(char* file_name) {
	return LoadLibrary(file_name);
}

void dynamic_library_close(dynamic_library_t dl) {
	FreeLibrary(dl);
}

void* dynamic_library_get_symbol(dynamic_library_t dl, char* symbol_string) {
	return (void*)GetProcAddress(dl, symbol_string);
}

char* dynamic_library_error() {
	return GetLastError();
}

#else

// LINUX
#include <dlfcn.h>

dynamic_library_t dynamic_library_load(char* file_name) {
	return dlopen(file_name, RTLD_NOW);
}

void dynamic_library_close(dynamic_library_t dl) {
	dlclose(dl);
}

void* dynamic_library_get_symbol(dynamic_library_t dl, char* symbol_string) {
	return dlsym(dl, symbol_string);
}

char* dynamic_library_error() {
	return dlerror();
}

#endif
