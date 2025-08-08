#pragma once
#include <alloca.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef struct
{
	char* buffer;
	size_t len;
} string_t;

#define string_create(cstring_copy) ({               \
	string_t string = {0};                           \
	string.len = strlen(cstring_copy);               \
	string.buffer = alloca(string.len);              \
	memcpy(string.buffer, cstring_copy, string.len); \
	string;                                          \
})

#define string_create_from_buffer(char_ptr, length) ({ \
	string_t string = {0};                             \
	string.len = length;                               \
	string.buffer = alloca(string.len);                \
	memcpy(string.buffer, char_ptr, string.len);       \
	string;                                            \
})

#define string_create_from_character(character) ({ \
	string_t string = {0};                         \
	string.len = 1;                                \
	string.buffer = alloca(string.len);            \
	string.buffer[0] = character;                  \
	string;                                        \
})

#define string_create_on_heap(malloc_function, char_ptr, length) ({ \
	string_t string = {0};                                          \
	string.len = length;                                            \
	string.buffer = malloc_function(string.len);                    \
	memcpy(string.buffer, char_ptr, string.len);                    \
	string;                                                         \
})

#define string_cstring(string) ({               \
	char* cstring = alloca(string.len + 1);     \
	memcpy(cstring, string.buffer, string.len); \
	cstring[string.len] = '\0';                 \
	cstring;                                    \
})

#define string_concat(string1, string2) ({                            \
	string_t string = {0};                                            \
	string.len = string1.len + string2.len;                           \
	string.buffer = alloca(string.len);                               \
	memcpy(string.buffer, string1.buffer, string1.len);               \
	memcpy(string.buffer + string1.len, string2.buffer, string2.len); \
	string;                                                           \
})

#define string_length(string) ({ \
	string.len;                  \
})

size_t string_max_len(size_t len1, size_t len2);
bool string_compare(string_t string1, string_t string2);
bool string_compare_cstring(string_t string, const char* cstring);
bool string_begins_with(string_t string, string_t prefix);
bool string_ends_with(string_t string, string_t suffix);
int32_t string_find(string_t string, string_t search);
int32_t string_find_from(string_t string, string_t search, size_t start_index);
bool string_contains(string_t string, string_t search);
bool string_is_int(string_t string);
bool string_is_uint(string_t string);
bool string_is_real(string_t string);
string_t string_to_upper(string_t string);
string_t string_to_lower(string_t string);
