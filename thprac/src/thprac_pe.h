#include <Windows.h>

PIMAGE_NT_HEADERS GetNtHeader(HMODULE hMod);
void* GetNtDataDirectory(HMODULE hMod, BYTE directory);
bool CheckDLLFunction(const wchar_t* path, const char* funcName);
