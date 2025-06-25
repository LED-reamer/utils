//includes
// - allocator.h

#pragma once

#include "allocator.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	ARGUMENT_REQUIRED_STRING, // filename or "name of file" etc
	ARGUMENT_OPTIONAL_FLAG, // -f or --flag
	ARGUMENT_OPTIONAL_STRING, // -s test or -s "testing this string"
}argument_parser_type_e;

typedef struct
{
	argument_parser_type_e type;
	const char* name;
	const char* alias;
	const char* desc;//description
	union {
		bool value_bool;
		const char* value_string;
	};
}argument_t;

typedef struct
{
	allocator_t* allocator;
	const char* program_name;
	const char* program_description;
	
	argument_t* arguments;//dynamic array
}argument_parser_t;

argument_parser_t* argument_parser_create(allocator_t* allocator, const char* program_description);
void argument_parser_destroy(argument_parser_t* arg_parser);

void argument_parser_add(argument_parser_t* arg_parser, argument_t argument);
void argument_parser_parse(argument_parser_t* arg_parser, int argc, char** argv);
void argument_parser_print_usage(argument_parser_t* arg_parser);

bool argument_parser_get_bool(argument_parser_t* arg_parser, const char* flag_name);
const char* argument_parser_get_string(argument_parser_t* arg_parser, const char* flag_name);
bool argument_string_to_int32(int32_t *output, char* string);
