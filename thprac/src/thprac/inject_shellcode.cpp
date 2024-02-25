#include "thprac_load_exe.h"

// BIG HEADS UP:
// If you update this file, do the following:

// 1) Run "cl /O1 inject_shellcode.cpp /link /NODEFAULTLIB /DLL" in an x86 native tools command prompt inside this folder
// 2) Open the generated inject_shellcode.exe in something like x64dbg and copy paste every byte of the generated InjectShellcode
//    function into the INJECT_SHELLCODE array
// 3) Pad the array with 0xCC until the static_assert is happy

// If there's a better way to do this, please tell me

typedef void PExeMain();

// Undefined behaviour yeah whatever.
// If the one thing that calls this actually manages to overflow this buffer then you're trying to 
// inject thprac into an exe that is genuinely malformed and that wouldn't run even without thprac.
__forceinline void string_copy(char* out, char* in) {
    DWORD i = 0;
    for(; in[i]; i++) {
        out[i] = in[i];
    }
    out[i] = 0;
}

#define MakePointer(t, p, offset) ((t)((PUINT8)(p) + offset))
extern "C" __declspec(safebuffers) __declspec(dllexport) InjectResult WINAPI InjectShellcode(remote_param* param)
{
    union // MemModule base
    {
        UINT_PTR iBase;
        HMODULE hModule;
        LPVOID lpBase;
        PIMAGE_DOS_HEADER pImageDosHeader;
    } pe;

    auto pLoadLibraryW = param->pLoadLibraryW;
    auto pLoadLibraryA = param->pLoadLibraryA;
    auto pVirtualProtect = param->pVirtualProtect;
    auto pGetProcAddress = param->pGetProcAddress;
    auto pGetLastError = param->pGetLastError;

    DWORD lOldProtect;

    pe.hModule = pLoadLibraryW(param->sExePath);
    if (pe.hModule == NULL)
        return { InjectResult::LoadError, (WORD)pGetLastError() };

    /*-------------------------------------------------------------------------------
								  B A S E    R E L O C A T I O N
		-------------------------------------------------------------------------------*/

    PIMAGE_NT_HEADERS pImageNtHeader = MakePointer(PIMAGE_NT_HEADERS, pe.pImageDosHeader, pe.pImageDosHeader->e_lfanew);

    // Get the delta of the real image base with the predefined
    UINT_PTR lBaseDelta = ((PUINT8)pe.iBase - (PUINT8)pImageNtHeader->OptionalHeader.ImageBase);

    // This module has been loaded to the ImageBase, no need to do relocation
    if (0 != lBaseDelta) {
        if (!(0 == pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress || 0 == pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)) {

            PIMAGE_BASE_RELOCATION pImageBaseRelocation = MakePointer(PIMAGE_BASE_RELOCATION, pe.lpBase,
                pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

            if (NULL == pImageBaseRelocation)
                return { InjectResult::RelocationError, 0 };

            while (0 != (pImageBaseRelocation->VirtualAddress + pImageBaseRelocation->SizeOfBlock)) {
                PWORD pRelocationData = MakePointer(PWORD, pImageBaseRelocation, sizeof(IMAGE_BASE_RELOCATION));

                int NumberOfRelocationData = (pImageBaseRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

                for (int i = 0; i < NumberOfRelocationData; i++) {
                    if (IMAGE_REL_BASED_HIGHLOW == (pRelocationData[i] >> 12)) {
                        PDWORD pAddress = (PDWORD)(pe.iBase + pImageBaseRelocation->VirtualAddress + (pRelocationData[i] & 0x0FFF));

                        pVirtualProtect(pAddress, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &lOldProtect);
                        *pAddress += (DWORD)lBaseDelta;
                        pVirtualProtect(pAddress, sizeof(DWORD), lOldProtect, &lOldProtect);
                    }
                }

                pImageBaseRelocation = MakePointer(PIMAGE_BASE_RELOCATION, pImageBaseRelocation, pImageBaseRelocation->SizeOfBlock);
            }
        }
    }

    /*-------------------------------------------------------------------------------
								  I M P O R T    R E S O L V E
		-------------------------------------------------------------------------------*/
    if (!(pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress == 0 || pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0)) {

        PIMAGE_IMPORT_DESCRIPTOR pImageImportDescriptor = MakePointer(PIMAGE_IMPORT_DESCRIPTOR, pe.lpBase,
            pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

        for (; pImageImportDescriptor->Name; pImageImportDescriptor++) {
            // Get the dependent module name
            PCHAR pDllName = MakePointer(PCHAR, pe.lpBase, pImageImportDescriptor->Name);

            // Get the dependent module handle, or load the dependent module
            HMODULE hMod = pLoadLibraryA(pDllName);

            // Failed
            if (NULL == hMod) {
                string_copy(param->sLoadErrDllName, pDllName);
                return { InjectResult::LoadError, (WORD)pGetLastError() };
            }

            // Original thunk
            PIMAGE_THUNK_DATA pOriginalThunk = NULL;
            if (pImageImportDescriptor->OriginalFirstThunk)
                pOriginalThunk = MakePointer(PIMAGE_THUNK_DATA, pe.lpBase, pImageImportDescriptor->OriginalFirstThunk);
            else
                pOriginalThunk = MakePointer(PIMAGE_THUNK_DATA, pe.lpBase, pImageImportDescriptor->FirstThunk);

            // IAT thunk
            PIMAGE_THUNK_DATA pIATThunk = MakePointer(PIMAGE_THUNK_DATA, pe.lpBase,
                pImageImportDescriptor->FirstThunk);

            for (; pOriginalThunk->u1.AddressOfData; pOriginalThunk++, pIATThunk++) {
                FARPROC lpFunction = NULL;
                if (IMAGE_SNAP_BY_ORDINAL(pOriginalThunk->u1.Ordinal)) {
                    lpFunction = pGetProcAddress(hMod, (LPCSTR)IMAGE_ORDINAL(pOriginalThunk->u1.Ordinal));
                } else {
                    PIMAGE_IMPORT_BY_NAME pImageImportByName = MakePointer(
                        PIMAGE_IMPORT_BY_NAME, pe.lpBase, pOriginalThunk->u1.AddressOfData);

                    lpFunction = pGetProcAddress(hMod, (LPCSTR)&(pImageImportByName->Name));
                }

                // Write into IAT
                pVirtualProtect(&(pIATThunk->u1.Function), sizeof(DWORD), PAGE_EXECUTE_READWRITE, &lOldProtect);
                pIATThunk->u1.Function = (DWORD)lpFunction;
                pVirtualProtect(&(pIATThunk->u1.Function), sizeof(DWORD), lOldProtect, &lOldProtect);
            }
        }
    }

#define GET_HEADER_DICTIONARY(m, idx) &m->OptionalHeader.DataDirectory[idx]
    unsigned char* codeBase = (unsigned char*)pe.lpBase;
    PIMAGE_TLS_DIRECTORY tls;
    PIMAGE_TLS_CALLBACK* callback;

    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(pImageNtHeader, IMAGE_DIRECTORY_ENTRY_TLS);
    if (directory->VirtualAddress == 0) {
        return { InjectResult::Ok, 0 };
    }

    tls = (PIMAGE_TLS_DIRECTORY)(codeBase + directory->VirtualAddress);
    callback = (PIMAGE_TLS_CALLBACK*)tls->AddressOfCallBacks;
    if (callback) {
        while (*callback) {
            (*callback)((LPVOID)codeBase, DLL_PROCESS_ATTACH, NULL);
            callback++;
        }
    }

    // User data
    if (param->pUserData) {
        LPVOID* addrForUserData = (LPVOID*)(param->pAddrOfUserData + pe.iBase);
        *addrForUserData = param->pUserData;
    }

    PExeMain* pfnModuleEntry = NULL;
    pfnModuleEntry = MakePointer(PExeMain*, pe.lpBase, pImageNtHeader->OptionalHeader.AddressOfEntryPoint);
    if (pfnModuleEntry) {
        pfnModuleEntry();
    }
    return { InjectResult::Ok, 0 };
};

// Dummy function to make this compile
extern "C" BOOL WINAPI _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    return TRUE;
}