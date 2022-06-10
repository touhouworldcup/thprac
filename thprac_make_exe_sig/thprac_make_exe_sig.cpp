#include <Windows.h>
#include <stdio.h>
#include "../thprac/src/3rdParties/MetroHash/metrohash128.h"
#include "../thprac/src/3rdParties/MetroHash/metrohash128.cpp"

struct ExeSig {
    uint32_t timeStamp;
    uint32_t textSize;
    uint32_t oepCode[10];
    uint32_t metroHash[4];
};

bool ReadMemory(void* buffer, void* addr, size_t size)
{
    SIZE_T bytesRead = 0;
    ReadProcessMemory(GetCurrentProcess(), addr, buffer, size, &bytesRead);
    return bytesRead == size;
}

bool GetExeInfo(void* exeBuffer, size_t exeSize, ExeSig& exeSigOut)
{
    if (exeSize < 128)
        return false;

    IMAGE_DOS_HEADER dosHeader;
    if (!ReadMemory(&dosHeader, exeBuffer, sizeof(IMAGE_DOS_HEADER)) || dosHeader.e_magic != 0x5a4d)
        return false;
    IMAGE_NT_HEADERS ntHeader;
    if (!ReadMemory(&ntHeader, (void*)((DWORD)exeBuffer + dosHeader.e_lfanew), sizeof(IMAGE_NT_HEADERS)) || ntHeader.Signature != 0x00004550)
        return false;

    exeSigOut.timeStamp = ntHeader.FileHeader.TimeDateStamp;
    exeSigOut.textSize = 0;
    for (auto& codeBlock : exeSigOut.oepCode) {
        codeBlock = 0;
    }
    for (auto& hashBlock : exeSigOut.metroHash) {
        hashBlock = 0;
    }

    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)((ULONG_PTR)((LONG)exeBuffer + dosHeader.e_lfanew) + ((LONG)(LONG_PTR) & (((IMAGE_NT_HEADERS*)0)->OptionalHeader)) + ntHeader.FileHeader.SizeOfOptionalHeader);
    for (int i = 0; i < ntHeader.FileHeader.NumberOfSections; i++, pSection++) {
        IMAGE_SECTION_HEADER section;
        if (!ReadMemory(&section, pSection, sizeof(IMAGE_SECTION_HEADER)))
            continue;
        if (!strcmp(".text", (char*)section.Name)) {
            exeSigOut.textSize = section.SizeOfRawData;
        }
        DWORD pOepCode = ntHeader.OptionalHeader.AddressOfEntryPoint;
        if (pOepCode >= section.VirtualAddress && pOepCode <= (section.VirtualAddress + section.Misc.VirtualSize)) {
            pOepCode -= section.VirtualAddress;
            pOepCode += section.PointerToRawData;
            pOepCode += (DWORD)exeBuffer;

            uint16_t oepCode[10];
            if (!ReadMemory(&oepCode, (void*)pOepCode, sizeof(oepCode)))
                continue;

            for (unsigned int i = 0; i < 10; ++i) {
                exeSigOut.oepCode[i] = (uint32_t) * (oepCode + i);
                exeSigOut.oepCode[i] ^= (i + 0x41) | ((i + 0x41) << 8);
            }
        }
    }

    MetroHash128 metro;
    if (exeSize < (1 << 23)) {
        metro.Hash((uint8_t*)exeBuffer, exeSize, (uint8_t*)exeSigOut.metroHash);
    }

    return true;
}

int main() {
    OPENFILENAMEW ofn = {};
    wchar_t fn[MAX_PATH] = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = fn;
    ofn.nMaxFile = sizeof(fn);
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrFilter = L"Application (*.exe)\0*.exe";
    if (!GetOpenFileNameW(&ofn)) {
        puts("Please select a file");
        getchar();
        return 1;
    }
    HANDLE hFile = CreateFileW(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    SIZE_T fileSize = GetFileSize(hFile, NULL);
    HANDLE hFileMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
    void* pFileMapView = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, fileSize);
    ExeSig out;
    GetExeInfo(pFileMapView, fileSize, out);
    UnmapViewOfFile(pFileMapView);
    CloseHandle(hFileMap);
    CloseHandle(hFile);

    printf(
        "    { idstr\n"
        "        nullptr\n"
        "        GAMEID_TITLE\n"
        "        CAT\n"
        "        vpatch_dll\n"
        "        appdata_str\n"
        "        Init\n"
        "        { %d, %d,\n"
        "            { 0x%4x, 0x%4x, 0x%4x, 0x%4x, 0x%4x,\n"
        "                0x%4x, 0x%4x, 0x%4x, 0x%4x, 0x%4x },\n"
        "            { 0x%8x, 0x%8x,\n"
        "                0x%8x, 0x%8x } } }",

        out.timeStamp, out.textSize,
        out.oepCode[0], out.oepCode[1], out.oepCode[2], out.oepCode[3], out.oepCode[4],
        out.oepCode[5], out.oepCode[6], out.oepCode[7], out.oepCode[8], out.oepCode[9],
        out.metroHash[0], out.metroHash[1], out.metroHash[2], out.metroHash[3]    
    );

    return 0;
}