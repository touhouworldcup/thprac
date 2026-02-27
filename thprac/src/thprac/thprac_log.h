#pragma once
#include "utils/utils.h"
#include <stdint.h>
#include <string>

namespace THPrac {

void log_print(const char* const msg, size_t len);
void log_vprintf(const char* format, va_list va);
void log_printf(const char* format, ...);

int log_mbox(void* hwnd, unsigned int type, const char* caption, const char* text);
int log_vmboxf(void* hwnd, unsigned int type, const char* caption, const char* format, va_list va);
int log_mboxf(void* hwnd, unsigned int type, const char* caption, const char* format, ...);

void log_init(bool launcher, bool console);

inline void log_print(const char* const null_terminated) {
    return log_print(null_terminated, t_strlen(null_terminated));
}

}