//platform independent code

#pragma once

#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_RESET   "\x1b[0m"

#ifndef LOGGING_NO_OUTPUT
#include <stdlib.h>//for exit
#include <stdio.h>//for printf

#define LOG(...) do { \
	printf(ANSI_BLUE "LOG: " ANSI_RESET __VA_ARGS__);\
	printf("\n");\
} while(0)

#define WARNING(...) do { \
	printf(ANSI_YELLOW "WARNING: " ANSI_RESET __VA_ARGS__);\
	printf("\n");\
} while(0)

#define ERROR(...) do { \
	printf(ANSI_RED "ERROR: " ANSI_RESET __VA_ARGS__);\
	printf("\n");\
} while(0)

#define FATAL_ERROR(...) do { \
	printf(ANSI_MAGENTA "FATAL ERROR: " ANSI_RESET __VA_ARGS__);\
	printf("\n");\
	exit(EXIT_FAILURE);\
} while(0)

#define LOG_UINT(value)do { \
	LOG(#value ": %u" , value);\
} while(0)

#define LOG_INT(value)do { \
	LOG(#value ": %i", value);\
} while(0)

#define LOG_FLOAT(value)do { \
	LOG(#value ": %f", value);\
} while(0)

#define LOG_PTR(value)do { \
	LOG(#value ": %p", value);\
} while(0)

#define LOG_CHAR(value)do { \
	LOG(#value ": %c", value);\
} while(0)

#define LOG_SIZE(value)do { \
	LOG(#value ": %zu", value);\
} while(0)

#else

#define LOG(...);
#define WARNING(...);
#define ERROR(...);
#define FATAL_ERROR(...)do { \
	exit(EXIT_FAILURE);\
} while(0)

#define LOG_UINT(value);
#define LOG_INT(value);
#define LOG_FLOAT(value);
#define LOG_PTR(value);
#define LOG_SIZE(value);

#endif
