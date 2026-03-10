#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#include <stdarg.h>

#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_RESET "\x1b[0m"

typedef struct{
	void (*info)(const char* fmt, ...);
	void (*warn)(const char* fmt, ...);
	void (*err)(const char* fmt, ...);
}logger_t;

static inline void logger_info(const char* fmt, ...) {
	fprintf(stdout, ANSI_BLUE "[info] " ANSI_RESET);
	va_list argptr;
	va_start(argptr, fmt);
	vfprintf(stdout, fmt, argptr);
	va_end(argptr);
	fprintf(stdout, "\n");
}
static inline void logger_warn(const char* fmt, ...) {
	fprintf(stdout, ANSI_YELLOW "[warn] " ANSI_RESET);
	va_list argptr;
	va_start(argptr, fmt);
	vfprintf(stdout, fmt, argptr);
	va_end(argptr);
	fprintf(stdout, "\n");
}
static inline void logger_err(const char* fmt, ...) {
	fprintf(stdout, ANSI_RED "[error] " ANSI_RESET);
	va_list argptr;
	va_start(argptr, fmt);
	vfprintf(stdout, fmt, argptr);
	va_end(argptr);
	fprintf(stdout, "\n");
}

static logger_t logger = {
	.info = &logger_info,
	.warn = &logger_warn,
	.err = &logger_err,
};

#endif
