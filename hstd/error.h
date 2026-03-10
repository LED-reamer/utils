#ifndef ERROR_H
#define ERROR_H

typedef const char* error_t;

static inline error_t ok() {return (error_t){0}; }
static inline error_t error(const char* description) {return (error_t){description}; }

#endif
