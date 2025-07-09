#include "utils/thread.h"
#include "utils/logging.h"

static int static_memory = 0;
static mutex_t static_memory_mutex;

typedef struct{
	int arg1;
	int arg2;
}threaded_func_args_t;

static void* threaded_func(void* args){
	mutex_lock(&static_memory_mutex);
	//now you can access the memory without interfering with other threads
	static_memory += ((threaded_func_args_t*)args)->arg1 + ((threaded_func_args_t*)args)->arg2;
	mutex_unlock(&static_memory_mutex);

	return NULL;
}


int main(void){
	static_memory_mutex = mutex_create();
	
	threaded_func_args_t args1 = {15, 5};
	threaded_func_args_t args2 = {-1, 8};
	threaded_func_args_t args3 = {49, 12};
	threaded_func_args_t args4 = {80, -79};
	
	thread_t t1 = thread_create(threaded_func, &args1);
	thread_t t2 = thread_create(threaded_func, &args2);
	thread_t t3 = thread_create(threaded_func, &args3);
	thread_t t4 = thread_create(threaded_func, &args4);

	thread_join(&t1);
	thread_join(&t2);
	thread_join(&t3);
	thread_join(&t4);

	int compare_result = (15+5-1+8+49+12+80-79);
	LOG("Result was %i. If it's not %i the mutex didn't work", static_memory, compare_result);
	
	return 0;
}
