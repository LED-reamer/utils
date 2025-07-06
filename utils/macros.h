#pragma once

//cast void* to type*
#define cast_ptr(void_ptr, type_ptr) ((type_ptr)void_ptr)
#define UNUSED_PARAM(x) (void)(x)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
