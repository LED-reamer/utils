#pragma once

// cast void* to type*
#define cast_ptr(void_ptr, type_ptr) ((type_ptr)void_ptr)
#define UNUSED_PARAM(x) (void)(x)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// platform detection
#if defined(__APPLE__)
	#define UTILS_PLATFORM "apple"
#elif defined(__MINGW32__)
	#define UTILS_PLATFORM "windows"
#elif defined(__linux__)
	#define UTILS_PLATFORM "linux"
#else
	#define UTILS_PLATFORM "unknown platform"
#endif
