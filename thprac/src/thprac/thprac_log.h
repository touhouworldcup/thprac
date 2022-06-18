#pragma once

namespace THPrac {
bool LogInit();
void LogClose();
void LogA(unsigned int priority, const char* fmt, ...);
void LogW(unsigned int priority, const wchar_t* fmt, ...);
void DbgPrtA(unsigned int priority, const char* fmt, ...);
void DbgPrtW(unsigned int priority, const wchar_t* fmt, ...);
void DbgLogA(unsigned int priority, const char* fmt, ...);
void DbgLogW(unsigned int priority, const wchar_t* fmt, ...);
}
