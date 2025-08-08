#pragma once

typedef enum {
	STATUS_ERROR = 0,  // false
	STATUS_OK = 1,	   // true
} status_e;

typedef struct
{
	status_e status;
	const char* desc;
} error_t;

#ifdef ERROR_NO_STRINGS
#define error_create(status_code, desc) ({error_t err = (error_t){status_code, NULL}; err; })
#else
#define error_create(status_code, desc) ({error_t err = (error_t){status_code, desc}; err; })
#endif

#define error_ok() error_create(STATUS_OK, NULL)
#define error_failed() error_create(STATUS_ERROR, NULL)
