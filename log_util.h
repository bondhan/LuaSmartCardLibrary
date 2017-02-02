#pragma once

#include "TMP_CONF.h"

enum {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*logfunc_t)(int, const char*);

void log_open_file(char *filename);
void log_close_file(void);
void logstring_default(int level, const char *str);
int log_printf(int level, const char *format, ...);

#ifdef __cplusplus
}
#endif