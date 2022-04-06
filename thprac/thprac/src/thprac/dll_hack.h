#pragma once

#include <Windows.h>


template<typename TFunction>
class _dll_bridge
{
public:
	_dll_bridge() = delete;
#ifdef UNICODE
	_dll_bridge(const wchar_t* dll_name, const char* func_name)
	{
		wchar_t dll_path[MAX_PATH]{};
#else 
	_dll_bridge(char* dll_name, TC* func_name)
	{
		char* dll_path[MAX_PATH]{};
#endif
		UINT tmp_offset = 0;

		tmp_offset = GetSystemDirectory(dll_path, MAX_PATH);
		dll_path[tmp_offset++] = '\\';
		for (int i = 0; dll_name[i] != '\0';)
			dll_path[tmp_offset++] = dll_name[i++];

		m_dllModule = LoadLibrary(dll_path);
		m_origFunc = reinterpret_cast<TFunction*>(GetProcAddress(m_dllModule, func_name));
	}
	~_dll_bridge()
	{
		if (m_dllModule)
			FreeLibrary(m_dllModule);
	}

	TFunction* original()
	{
		return m_origFunc;
	}

private:
	TFunction *m_origFunc = nullptr;
	HMODULE m_dllModule = NULL;
};

#define DLL_REPLACE_FUNC(func, dll_name, func_name) static _dll_bridge<decltype (func)> _dll_repl(dll_name, func_name)
#define DLL_CALL_ORIG(...) (*_dll_repl.original())(__VA_ARGS__)