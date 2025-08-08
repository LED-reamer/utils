#include "thread.h"
#ifdef _WIN32
#include <windows.h>
#undef ERROR
#endif
#include "logging.h"

#ifdef _WIN32
typedef struct
{
	void* (*func)(void* func_args);
	void* func_args;
} __thread_windows_wrapper_arg_t;

DWORD WINAPI __windows_thread_function_wrapper(void* func_args) {
	__thread_windows_wrapper_arg_t* wrapper_args = func_args;
	wrapper_args->func(wrapper_args->func_args);
	return TRUE;
}
#endif

#ifndef _WIN32
#include <unistd.h>	 //for sleep
#endif

thread_t thread_create(void* (*func)(void* func_args), void* func_args) {
	thread_t thread = {0};
#ifdef _WIN32
	__thread_windows_wrapper_arg_t wrapper_arg = {.func = func, .func_args = func_args};
	thread.handle = (void*)CreateThread(NULL, 0, __windows_thread_function_wrapper, &wrapper_arg, 0, NULL);
#else  // POSIX
	pthread_create(&thread.handle, NULL, func, func_args);
#endif

	if (thread.handle == 0)
		ERROR("Could not create thread");

	return thread;
}

void thread_join(thread_t* thread) {
	void* return_value;
#ifdef _WIN32
	if (WaitForSingleObject(thread->handle, INFINITE) == WAIT_FAILED)
		ERROR("Could not join thread");
	if (GetExitCodeThread(thread->handle, (LPDWORD)&return_value) == 0)
		ERROR("Could not get exit code on joining thread");
#else  // POSIX
	if (pthread_join(thread->handle, &return_value) != 0)
		ERROR("Could not join thread");
#endif
}

void thread_detach(thread_t* thread) {
#ifdef _WIN32
	CloseHandle(thread->handle);  // does not end the thread
#else
	if (pthread_detach(thread->handle) != 0)
		ERROR("Could not detach thread");
#endif
}

void thread_sleep_ms(size_t milliseconds) {
#ifdef _WIN32
	Sleep(milliseconds);
#else
	if (usleep(milliseconds * 1000) != 0)
		ERROR("thread could not sleep for %zu milliseconds", milliseconds);
#endif
}

mutex_t mutex_create() {
	mutex_t mutex;

#ifdef _WIN32
	mutex.handle = (void*)CreateMutex(NULL, FALSE, NULL);
	if (mutex.handle == NULL)
		ERROR("Could not create mutex");
#else
	if (pthread_mutex_init(&mutex.handle, NULL) != 0)
		ERROR("Could not create mutex");
#endif

	return mutex;
}

void mutex_destroy(mutex_t* mutex) {
#ifdef _WIN32
	CloseHandle(mutex->handle);
#else
	if (pthread_mutex_destroy(&mutex->handle) != 0)
		ERROR("Could not destroy mutex");
#endif
}

void mutex_lock(mutex_t* mutex) {
#ifdef _WIN32
	if (WaitForSingleObject(mutex->handle, INFINITE) == WAIT_FAILED)
		ERROR("Could not lock mutex");
#else
	if (pthread_mutex_lock(&mutex->handle) != 0)
		ERROR("Could not lock mutex");
#endif
}

void mutex_unlock(mutex_t* mutex) {
#ifdef _WIN32
	if (!ReleaseMutex(mutex->handle))
		ERROR("Could not unlock mutex");
#else
	if (pthread_mutex_unlock(&mutex->handle) != 0)
		ERROR("Could not unlock mutex");
#endif
}

bool mutex_try_lock(mutex_t* mutex) {
#ifdef _WIN32
	if (WaitForSingleObject(mutex->handle, 0) == WAIT_TIMEOUT)
		return true;
#else
	if (pthread_mutex_trylock(&mutex->handle) == 0)
		return true;
#endif

	return false;
}
