#pragma once
#include <cstdint>
#include <Windows.h>
#include <psapi.h>
#undef UNICODE
#include <tlhelp32.h>
#define UNICODE

namespace THPrac
{
	__declspec(noinline) void EncryptAndExpand(void* input, size_t input_size, void** output, size_t* output_size);
	__declspec(noinline) void DecryptAndContract(uint32_t* input, void** output, size_t* output_size);
	__declspec(noinline) void* GetFunctionPtr(uint32_t* function_name, HMODULE hLibrary);
	__declspec(noinline) HMODULE GetLibraryHandle(uint32_t* library_name);
	__declspec(noinline) void InitFunctions();

#define DeclareXoredFunction(function) extern decltype(function)* X##function;

	DeclareXoredFunction(OpenProcess);
	DeclareXoredFunction(ReadProcessMemory);
	DeclareXoredFunction(WriteProcessMemory);
	DeclareXoredFunction(GetModuleFileNameExA);
	DeclareXoredFunction(VirtualProtectEx);
	DeclareXoredFunction(CreateProcessA);
	DeclareXoredFunction(CreateRemoteThread);
	DeclareXoredFunction(VirtualAllocEx);
	DeclareXoredFunction(VirtualFreeEx);
	DeclareXoredFunction(VirtualProtect);
	DeclareXoredFunction(GetModuleHandleA);
	DeclareXoredFunction(GetProcAddress);
	DeclareXoredFunction(CreateToolhelp32Snapshot);
	DeclareXoredFunction(Process32First);
	DeclareXoredFunction(Process32Next);
	DeclareXoredFunction(LoadLibraryA);

	extern uint32_t xoredInitShellcode[];
}