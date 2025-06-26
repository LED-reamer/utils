#include "utils/logging.h"
#include <stdint.h>

int main(void){
	LOG("this is a normal log. All logging functions support formating %i and "ANSI_MAGENTA"colors"ANSI_RESET, 123456);

	LOG("for debugging you can also log values:");
	uint8_t a = 1;
	int b = -2;
	float c = 3.076f;
	void* d = (void*)&a;
	char e = 'C';
	size_t f = 6;
	LOG_UINT(a);
	LOG_INT(b);
	LOG_FLOAT(c);
	LOG_PTR(d);
	LOG_CHAR(e);
	LOG_SIZE(f);

	//commented since UNREACHABLE exits the program...
	//switch (a){
	//	case 7: break;
	//	default: UNREACHABLE("the switch is missing a value!"); break;
	//}

	
	WARNING("this is warning!");
	ERROR("this is an error!");
	FATAL_ERROR("this is a fatal error and the program exits");
	return 0;
}
