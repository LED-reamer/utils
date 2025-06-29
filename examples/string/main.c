#include "utils/string.h"
#include "utils/logging.h"


int main(void){
	WARNING("be careful, stack memory is limited. So use it for simple string manipulation only");
	
	string_t simple_string = string_create("This string was allocated on the stack!");

	LOG("You need to convert string_t to cstrings since they don't use null termination: %s", string_cstring(simple_string));

	string_t hello = string_create("hello ");
	char character_buffer[5] = { 'w', 'o', 'r', 'l', 'd'};//no null terminator
	size_t character_buffer_length = 5;
	string_t world = string_create_from_buffer(character_buffer, character_buffer_length);

	string_t hello_world = string_concat(hello, world);
	const char* cstring_hello_world = string_cstring(hello_world);
	LOG("combine strings: %s", cstring_hello_world);

	string_t hell = string_create("hell");
	LOG("%s contains %s?", cstring_hello_world, string_cstring(hell));
	if(string_contains(hello_world, hell)){
		LOG("yes");
	}else ERROR("no - this should not happen :(");


	LOG("There are a lot more functions to showcase... see utils/string.h");
	return 0;
}
