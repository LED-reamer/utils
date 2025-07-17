#include "arguments.h"

#include "array.h"
#include "logging.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

//returns NULL if flag could not be found
argument_t* __get_argument_from_string(argument_parser_t* arg_parser, const char* flag_name_or_alias, bool getter);


argument_parser_t argument_parser_create(allocator_t* allocator, const char* program_description)
{
	argument_parser_t arg_parser = (argument_parser_t){
		.allocator = allocator,
		.program_description = program_description,
		.arguments = array_create(allocator, argument_t, 0, 10),
	};


	argument_parser_add(&arg_parser,
		(argument_t){
			.type = ARGUMENT_OPTIONAL_FLAG,
			.name="-h",
			.alias="--help",
			.desc="shows this help message"
		}
	);

	return arg_parser;
}

void argument_parser_destroy(argument_parser_t* arg_parser)
{
	array_destroy(arg_parser->arguments);
	*arg_parser = (argument_parser_t){0};
}

void argument_parser_add(argument_parser_t* arg_parser, argument_t argument)
{
	//check if argument already exists
	if(NULL != __get_argument_from_string(arg_parser, argument.name, false))
	{
		ERROR("Could not add argument %s since it already exists", argument.name);
	}

	array_push(arg_parser->arguments, argument);
}

void argument_parser_parse(argument_parser_t* arg_parser, int argc, char** argv)
{
	if(!(argc > 0))
	{
		ERROR("wasn't provided correct argc or argv");
	}
	arg_parser->program_name = argv[0];

	int positional_arg_count = 0;
	//handle positional arguments first
	for(size_t i = 0; i < array_len(arg_parser->arguments); i++)
	{
		if(arg_parser->arguments[i].type != ARGUMENT_REQUIRED_STRING)
			continue;

		if(positional_arg_count+1 >= argc || NULL != __get_argument_from_string(arg_parser, argv[positional_arg_count+1], false))
		{
		 	argument_parser_print_usage(arg_parser);
			exit(EXIT_FAILURE);
		}

		arg_parser->arguments[i].value_string = argv[positional_arg_count+1];
		positional_arg_count++;
	}

	//handle flags
	size_t num_invalid_args = 0;
	for (int i = 1; i < argc; i++)
	{
		//skip positional arguments
		if(i <= positional_arg_count) continue;

		//check if token is an argument
		argument_t* arg = __get_argument_from_string(arg_parser, argv[i], false);
		char *endptr;
		if(arg != NULL)
		{
			switch(arg->type)
			{
				case ARGUMENT_OPTIONAL_FLAG:
					arg->value_bool = true;
					break;
				case ARGUMENT_OPTIONAL_STRING:
					if(i+1 >= argc) FATAL_ERROR("no string provided for %s", arg->name);
					arg->value_string = argv[i+1];
					i++;
					break;
				case ARGUMENT_OPTIONAL_UINT32:
					if(i+1 >= argc) FATAL_ERROR("no unsigned integer provided for %s", arg->name);
					arg->value_uint32 = strtoul(argv[i+1], &endptr, 10);
					if(*endptr != '\0'){
						FATAL_ERROR("invalid unsigned integer provided for %s", arg->name);
					}
					i++;
					break;
				case ARGUMENT_OPTIONAL_INT32:
					if(i+1 >= argc) FATAL_ERROR("no signed integer provided for %s", arg->name);
					arg->value_int32 = strtol(argv[i+1], &endptr, 10);
					if(*endptr != '\0'){
						FATAL_ERROR("invalid signed integer provided for %s", arg->name);
					}
					i++;
					break;
				default:
					break;
			}
			continue;
		}

		num_invalid_args++;
		printf("invalid argument: %s\n", argv[i]);
	}

	if(num_invalid_args != 0)
	{
		argument_parser_print_usage(arg_parser);
		exit(EXIT_FAILURE);
	}

	if(argument_parser_get_bool(arg_parser, "-h"))
		argument_parser_print_usage(arg_parser);
}

void argument_parser_print_usage(argument_parser_t* arg_parser)
{
	printf("usage: %s ", arg_parser->program_name);

	for(size_t i = 0; i < array_len(arg_parser->arguments); i++)
	{
		if(arg_parser->arguments[i].type == ARGUMENT_REQUIRED_STRING)
			printf("<%s> ", arg_parser->arguments[i].name);
		else
			printf("[%s] ", arg_parser->arguments[i].name);
	}

	printf("\n\n%s\n\noptions:\n", arg_parser->program_description);


	for(size_t i = 0; i < array_len(arg_parser->arguments); i++)
	{
		printf("\t%s, %s\t\t%s\n", arg_parser->arguments[i].name, arg_parser->arguments[i].alias, arg_parser->arguments[i].desc);
	}
}

bool argument_parser_get_bool(argument_parser_t* arg_parser, const char* flag_name)
{
	argument_t* arg = __get_argument_from_string(arg_parser, flag_name, true);
	if(arg == NULL)
	{
		FATAL_ERROR("Could not find \"%s\"", flag_name);
	}

	return arg->value_bool;
}

const char* argument_parser_get_string(argument_parser_t* arg_parser, const char* flag_name)
{
	argument_t* arg = __get_argument_from_string(arg_parser, flag_name, true);
	if(arg == NULL)
	{
		FATAL_ERROR("Could not find \"%s\"", flag_name);
	}

	return arg->value_string;
}

uint32_t argument_parser_get_uint32(argument_parser_t* arg_parser, const char* flag_name){
	argument_t* arg = __get_argument_from_string(arg_parser, flag_name, true);
	if(arg == NULL)
	{
		FATAL_ERROR("Could not find \"%s\"", flag_name);
	}

	return arg->value_uint32;
}

int32_t argument_parser_get_int32(argument_parser_t* arg_parser, const char* flag_name){
	argument_t* arg = __get_argument_from_string(arg_parser, flag_name, true);
	if(arg == NULL)
	{
		FATAL_ERROR("Could not find \"%s\"", flag_name);
	}

	return arg->value_int32;
}

//local functions
argument_t* __get_argument_from_string(argument_parser_t* arg_parser, const char* flag_name_or_alias, bool getter)
{
	if(flag_name_or_alias == NULL)
	{
		ERROR("Could not get flag since it's name was NULL");
	}

	if(!getter)
		if(flag_name_or_alias[0] != '-') return NULL;

	for (size_t i = 0; i < array_len(arg_parser->arguments); i++)
	{
		if(0 == strncmp(flag_name_or_alias, arg_parser->arguments[i].name, strlen(arg_parser->arguments[i].name))
			&& strlen(flag_name_or_alias) == strlen(arg_parser->arguments[i].name))
		{
			return &arg_parser->arguments[i];
		}

		if(0 == strncmp(flag_name_or_alias, arg_parser->arguments[i].alias, strlen(arg_parser->arguments[i].alias))
			&& strlen(flag_name_or_alias) == strlen(arg_parser->arguments[i].alias))
		{
			return &arg_parser->arguments[i];
		}
	}
	return NULL;
}
