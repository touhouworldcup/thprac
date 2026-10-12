#include "thprac_pe.h"
#include "thprac_utils.h"

bool CheckDLLFunction(const wchar_t* path, const char* funcName) {
    MappedFile file(path);

    auto exeSize = file.fileSize;
    auto exeBuffer = file.fileMapView;
    if (exeSize < 128)
        return 0;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)exeBuffer;
    if (!pDosHeader || pDosHeader->e_magic != 0x5a4d || (uintptr_t)pDosHeader->e_lfanew + 512 >= exeSize)
        return 0;

    // The PE optional header differs between PE32 and PE32+. Do not use
    // PIMAGE_NT_HEADERS here, because that typedef follows our own architecture
    // rather than the architecture of the file being read.
    uintptr_t ntHeaderAddr = (uintptr_t)exeBuffer + (uintptr_t)pDosHeader->e_lfanew;
    if (ntHeaderAddr > (uintptr_t)exeBuffer + exeSize - sizeof(DWORD) - sizeof(IMAGE_FILE_HEADER))
        return 0;

    auto processExports = [&](auto* pNtHeader) -> bool {
        if (!pNtHeader || pNtHeader->Signature != 0x00004550)
            return false;

        PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeader);
        if (!pSection)
            return false;

        const auto& exportDir = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        if (exportDir.VirtualAddress == 0 || exportDir.Size == 0)
            return false;

        auto pExportSectionVA = exportDir.VirtualAddress;
        for (DWORD i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++, pSection++) {
            if (pSection->VirtualAddress <= pExportSectionVA &&
                (ULONGLONG)pSection->VirtualAddress + pSection->SizeOfRawData > pExportSectionVA) {
                uintptr_t pSectionBase =
                    (uintptr_t)exeBuffer - (uintptr_t)pSection->VirtualAddress + pSection->PointerToRawData;
                PIMAGE_EXPORT_DIRECTORY pExportDirectory =
                    (PIMAGE_EXPORT_DIRECTORY)(pSectionBase + pExportSectionVA);

                // AddressOfNames is an RVA array of DWORDs on both PE32 and PE32+.
                DWORD* pExportNames = (DWORD*)(pSectionBase + pExportDirectory->AddressOfNames);
                for (DWORD j = 0; j < pExportDirectory->NumberOfNames; ++j) {
                    auto pFunctionName = (char*)(pSectionBase + pExportNames[j]);
                    if (!strcmp(pFunctionName, funcName))
                        return true;
                }
            }
        }
        return false;
        };

    // Select the PE header layout from the file, not from _WIN64.
    WORD optionalMagic = *(WORD*)(ntHeaderAddr + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER));
    if (optionalMagic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        return processExports((PIMAGE_NT_HEADERS32)ntHeaderAddr);
    if (optionalMagic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
        return processExports((PIMAGE_NT_HEADERS64)ntHeaderAddr);

    return false;
}

PIMAGE_NT_HEADERS GetNtHeader(HMODULE hMod) {
    if (!hMod) {
        return 0;
    }
    PIMAGE_DOS_HEADER pDosH = (PIMAGE_DOS_HEADER)hMod;
    PIMAGE_NT_HEADERS pNTH = (PIMAGE_NT_HEADERS)((uintptr_t)pDosH + (uintptr_t)pDosH->e_lfanew);
    return pNTH;
}

void* GetNtDataDirectory(HMODULE hMod, BYTE directory) {
    if (PIMAGE_NT_HEADERS pNTH = GetNtHeader(hMod)) {
        if (UINT_PTR DirVA = pNTH->OptionalHeader.DataDirectory[directory].VirtualAddress) {
            return (BYTE*)hMod + DirVA;
        }
    }
    return nullptr;
}
