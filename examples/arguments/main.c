#include "utils/arguments.h"
#include "utils/logging.h"
#include "utils/tools/memory_tracker.h"

int main(int argc, char** argv){
	debugger_init(false);
	argument_parser_t arg_parser = argument_parser_create(allocator_get_default(), "The programs description.");

	argument_parser_add(&arg_parser,
		(argument_t){
			.type = ARGUMENT_REQUIRED_STRING,
			.name="file",
			.alias="filename",
			.desc="specify a file path"
		}
	);
	argument_parser_add(&arg_parser,
		(argument_t){
			.type = ARGUMENT_OPTIONAL_FLAG,
			.name="-f",
			.alias="--flag",
			.desc="sets an important flag"
		}
	);
	argument_parser_add(&arg_parser,
		(argument_t){
			.type = ARGUMENT_OPTIONAL_STRING,
			.name="-s",
			.alias="--string",
			.desc="sets an important string"
		}
	);
	argument_parser_add(&arg_parser,
		(argument_t){
			.type = ARGUMENT_OPTIONAL_INT32,
			.name="-i",
			.alias="--int",
			.desc="sets an important integer (still is a string)"
		}
	);
	argument_parser_parse(&arg_parser, argc, argv);

	//use the arguments here
	LOG("%s", argument_parser_get_string(&arg_parser, "file"));
	if(argument_parser_get_bool(&arg_parser, "-f")){
		LOG("-f was specified");
	}else{
		WARNING("-f was not specified");
	}

	const char* string = argument_parser_get_string(&arg_parser, "-s");
	if(string != NULL)
		LOG("-s was %s", string);
	else
		WARNING("-s was not specified");

	int32_t integer = argument_parser_get_int32(&arg_parser, "-i");

	if(integer != 0)
		LOG("-i was %i", integer);
	else
		WARNING("-i was not specified");

	argument_parser_destroy(&arg_parser);
	debugger_deinit();
	return 0;
}
