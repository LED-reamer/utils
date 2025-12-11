#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "allocator.h"

typedef enum {
	ARGUMENT_REQUIRED_STRING,  // filename or "name of file" etc
	ARGUMENT_OPTIONAL_FLAG,	   // -f or --flag
	ARGUMENT_OPTIONAL_STRING,  // -s test or -s "testing this string"
	ARGUMENT_OPTIONAL_UINT32,
	ARGUMENT_OPTIONAL_INT32,
} argument_parser_type_e;

typedef struct
{
	argument_parser_type_e type;
	const char* name;
	const char* alias;
	const char* desc;  // description
	union {
		const char* value_string;
		bool value_bool;
		uint32_t value_uint32;
		int32_t value_int32;
	};
} argument_t;

typedef struct
{
	allocator_t* allocator;
	const char* program_name;
	const char* program_description;

	argument_t* arguments;	// dynamic array
} argument_parser_t;

argument_parser_t argument_parser_create(allocator_t* allocator, const char* program_description);
void argument_parser_destroy(argument_parser_t* arg_parser);

void argument_parser_add(argument_parser_t* arg_parser, argument_t argument);
void argument_parser_parse(argument_parser_t* arg_parser, int argc, char** argv);
void argument_parser_print_usage(argument_parser_t* arg_parser);

bool argument_parser_get_bool(argument_parser_t* arg_parser, const char* flag_name);
const char* argument_parser_get_string(argument_parser_t* arg_parser, const char* flag_name);
uint32_t argument_parser_get_uint32(argument_parser_t* arg_parser, const char* flag_name);
int32_t argument_parser_get_int32(argument_parser_t* arg_parser, const char* flag_name);
