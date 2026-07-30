#include <stdio.h>
#include <string.h>
#include <windows.h>
DWORD RvaToOffset(DWORD rva, IMAGE_NT_HEADERS32* nt)
{
    IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);

    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; i++, section++) {
        DWORD va = section->VirtualAddress;

        DWORD size = section->Misc.VirtualSize;

        if (rva >= va && rva < va + size) {
            return section->PointerToRawData
                + (rva - va);
        }
    }

    return 0;
}


bool PatchAPI(
    const char* input,
    const char* output)
{
    if (!CopyFileA(input, output, FALSE)) {
        printf("Copy failed: %lu\n", GetLastError());
        return false;
    }

    HANDLE hFile = CreateFileA(output, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);

    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    HANDLE hMap = CreateFileMappingA(hFile, nullptr, PAGE_READWRITE, 0, 0, nullptr);

    if (!hMap) {
        CloseHandle(hFile);
        return false;
    }

    BYTE* base = (BYTE*)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0);

    if (!base) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        return false;
    }

    bool patched = false;

    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)base;

    IMAGE_NT_HEADERS32* nt = (IMAGE_NT_HEADERS32*)(base + dos->e_lfanew);

    auto& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    IMAGE_IMPORT_DESCRIPTOR* imp = NULL; 
     DWORD importOffset = 0;
    if (dir.VirtualAddress == 0)
        goto finish;

    
    importOffset = RvaToOffset(dir.VirtualAddress, nt);
    imp = (IMAGE_IMPORT_DESCRIPTOR*)(base + importOffset);

    for (; imp->Name; imp++) {
        char* dll = (char*)(base + RvaToOffset(imp->Name, nt));

        if (_stricmp(dll, "KERNEL32.dll") != 0)
            continue;
        DWORD thunkOffset = RvaToOffset(imp->OriginalFirstThunk, nt);

        if (!imp->OriginalFirstThunk) {
            thunkOffset = RvaToOffset(imp->FirstThunk, nt);
        }

        IMAGE_THUNK_DATA* thunk = (IMAGE_THUNK_DATA*)(base + thunkOffset);

        for (; thunk->u1.AddressOfData; thunk++) {
            IMAGE_IMPORT_BY_NAME* ibn = (IMAGE_IMPORT_BY_NAME*)(base + RvaToOffset(thunk->u1.AddressOfData, nt));
            char* name = (char*)ibn->Name;
            if (strcmp(name, "GetSystemTimePreciseAsFileTime") == 0) {
                printf("Patch: %s -> GetSystemTimeAsFileTime\n", name);
                strcpy_s(name, 64, "GetSystemTimeAsFileTime");
                patched = true;
            }
        }
    }

finish:

    UnmapViewOfFile(base);
    CloseHandle(hMap);
    CloseHandle(hFile);

    if (patched)
        printf("Patch success\n");
    else
        printf("No patch needed\n");

    return true;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf( "Usage:\n IATPatch.exe input.exe output.exe\n");
        return 1;
    }

    if (!PatchAPI(argv[1], argv[2])) {
        printf("Patch failed\n");
        return 2;
    }

    return 0;
}