#include "utils/error.h"
#include "utils/logging.h"

error_t function_that_may_fail(int should_fail){
	if(should_fail){
		return error_create(STATUS_ERROR, "The function failed... (code: 123)");
	}else{
		//return error_create(STATUS_OK, NULL);
		return error_ok();//equivalent
	}
}

int main(void){
	error_t err = function_that_may_fail(1);
	if(!err.status){
		LOG("function_that_may_fail failed: %s", err.desc);
	}

	if(function_that_may_fail(0).status == STATUS_OK){
		LOG("function_that_may_fail did not fail");
	}
	return 0;
}
