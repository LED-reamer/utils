#include "utils/buffer.h"
#include "utils/logging.h"


int main(void){
	buffer_t my_buffer = buffer_create(allocator_get_default());

	char* test_string = "this is a test string";
	size_t test_string_size = strlen(test_string) + 1 /*null terminator*/;

	//the buffer automatically resizes to fit the string
	buffer_copy_data(&my_buffer, test_string, test_string_size);

	//write buffer to file
	buffer_write_to_file(&my_buffer, "output.bin");

	if(buffer_load_data_from_file(&my_buffer, "output.bin")){
		LOG("loaded buffer successully from output.bin");
		LOG("buffer says \"%s\"", (char*)my_buffer.data);
	}else{
		ERROR("Could not load buffer from file");
	}
	buffer_destroy(&my_buffer);

	//you can also parse binary files:
	//writing...
	char* write_data1 = "this is a custom parsed binary file!";
	uint32_t write_data2 = 123;
	double write_data3 = 50.0;

	buffer_t write_buffer = buffer_create(allocator_get_default());
	buffer_write_cstring(&write_buffer, write_data1);
	buffer_write(&write_buffer, write_data2, uint32_t);
	buffer_write(&write_buffer, write_data3, double);
	buffer_destroy(&write_buffer);
	buffer_write_to_file(&write_buffer, "output.bin");

	//reading...
	

	buffer_t read_buffer = buffer_create(allocator_get_default());
	buffer_load_data_from_file(&read_buffer, "output.bin");
	const char* read_data1 = buffer_read_cstring(&write_buffer);
	uint32_t read_data2 = buffer_read(&read_buffer, uint32_t);
	double read_data3 = buffer_read(&read_buffer, double);

	if(0 == strcmp(read_data1, write_data1)
		&& read_data2 == write_data2
		&& read_data3 == read_data3){
		LOG("parsed correctly");
	}else{
		ERROR("Could not parse correctly");
	}
	buffer_destroy(&read_buffer);
	
	return 0;
}
