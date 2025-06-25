#pragma once

#ifndef _WIN32
#include <pthread.h>
#endif
#include <stdbool.h>
#include <stddef.h>

typedef struct
{
 	#ifdef _WIN32
    void* handle;
	#else//POSIX
	pthread_t handle;
	#endif
}thread_t;

thread_t thread_create(void* (*func)(void* func_args), void* func_args);
void thread_join(thread_t* thread);
void thread_detach(thread_t* thread);
void thread_sleep_ms(size_t milliseconds);

typedef struct
{
	#ifdef _WIN32
	void* handle;
	#else//POSIX
	pthread_mutex_t handle;
	#endif
}mutex_t;

mutex_t mutex_create();
void mutex_destroy(mutex_t* mutex);
void mutex_lock(mutex_t* mutex);//blocks until locked
void mutex_unlock(mutex_t* mutex);
bool mutex_try_lock(mutex_t* mutex);//non-blocking, returns true if locked successfully
