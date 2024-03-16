﻿#include "thprac_load_exe.h"
#include "utils/utils.h"
#include <windows.h>

namespace THPrac {

// This array is generated by compiling inject_shellcode.cpp. Check the comment at the top of that file for more info.
unsigned char INJECT_SHELLCODE[] = {
    0x55, 0x53, 0x57, 0x56, 0x83, 0xEC, 0x38, 0x8B, 0x4C, 0x24, 0x4C, 0x8B, 0x41, 0x08, 0x8B, 0x51,
    0x0C, 0x89, 0x54, 0x24, 0x34, 0x8B, 0x51, 0x10, 0x89, 0x54, 0x24, 0x24, 0x8B, 0x51, 0x14, 0x89,
    0x54, 0x24, 0x20, 0x8B, 0x71, 0x18, 0x83, 0xC1, 0x1C, 0x51, 0xFF, 0xD0, 0x85, 0xC0, 0x0F, 0x84,
    0xA1, 0x02, 0x00, 0x00, 0x89, 0xC3, 0x8B, 0x40, 0x3C, 0x89, 0xD9, 0x89, 0x44, 0x24, 0x0C, 0x2B,
    0x4C, 0x03, 0x34, 0x89, 0x4C, 0x24, 0x04, 0x89, 0x5C, 0x24, 0x10, 0x89, 0x74, 0x24, 0x30, 0x0F,
    0x84, 0xDE, 0x00, 0x00, 0x00, 0x8B, 0x44, 0x24, 0x0C, 0x8B, 0x8C, 0x03, 0xA0, 0x00, 0x00, 0x00,
    0x85, 0xC9, 0x0F, 0x84, 0xCB, 0x00, 0x00, 0x00, 0x8B, 0x44, 0x24, 0x0C, 0x83, 0xBC, 0x03, 0xA4,
    0x00, 0x00, 0x00, 0x00, 0x0F, 0x84, 0xB9, 0x00, 0x00, 0x00, 0x8B, 0x44, 0x0B, 0x04, 0x8B, 0x14,
    0x0B, 0x01, 0xC2, 0x0F, 0x84, 0xAA, 0x00, 0x00, 0x00, 0x8D, 0x2C, 0x0B, 0x8D, 0x34, 0x0B, 0x83,
    0xC6, 0x04, 0x8D, 0x48, 0xF8, 0x83, 0xF9, 0x02, 0x0F, 0x82, 0x80, 0x00, 0x00, 0x00, 0x89, 0x34,
    0x24, 0xD1, 0xE9, 0x31, 0xFF, 0x89, 0x6C, 0x24, 0x18, 0x89, 0x4C, 0x24, 0x08, 0x0F, 0xB7, 0x5C,
    0x7D, 0x08, 0x89, 0xD8, 0x25, 0x00, 0xF0, 0x00, 0x00, 0x3D, 0x00, 0x30, 0x00, 0x00, 0x75, 0x50,
    0x8B, 0x44, 0x24, 0x18, 0x8B, 0x28, 0x81, 0xE3, 0xFF, 0x0F, 0x00, 0x00, 0x03, 0x5C, 0x24, 0x10,
    0x8D, 0x14, 0x2B, 0x89, 0x54, 0x24, 0x14, 0x8D, 0x44, 0x24, 0x1C, 0x50, 0x6A, 0x40, 0xB8, 0x04,
    0x00, 0x00, 0x00, 0x50, 0x52, 0x8B, 0x74, 0x24, 0x34, 0xFF, 0xD6, 0x8B, 0x44, 0x24, 0x04, 0x01,
    0x44, 0x1D, 0x00, 0x8B, 0x6C, 0x24, 0x18, 0x8D, 0x44, 0x24, 0x1C, 0x50, 0xFF, 0x74, 0x24, 0x20,
    0xB8, 0x04, 0x00, 0x00, 0x00, 0x50, 0xFF, 0x74, 0x24, 0x20, 0xFF, 0xD6, 0x8B, 0x4C, 0x24, 0x08,
    0x47, 0x39, 0xF9, 0x75, 0x98, 0x8B, 0x04, 0x24, 0x8B, 0x00, 0x8B, 0x5C, 0x24, 0x10, 0x8D, 0x34,
    0x28, 0x83, 0xC6, 0x04, 0x01, 0xC5, 0x8B, 0x06, 0x8B, 0x56, 0xFC, 0x01, 0xC2, 0x0F, 0x85, 0x5F,
    0xFF, 0xFF, 0xFF, 0x8B, 0x44, 0x24, 0x0C, 0x8B, 0x8C, 0x03, 0x80, 0x00, 0x00, 0x00, 0x89, 0xC8,
    0x89, 0x4C, 0x24, 0x08, 0x85, 0xC9, 0x0F, 0x84, 0x2C, 0x01, 0x00, 0x00, 0x8B, 0x44, 0x24, 0x0C,
    0x83, 0xBC, 0x03, 0x84, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x84, 0x1A, 0x01, 0x00, 0x00, 0x8B, 0x44,
    0x24, 0x08, 0x8B, 0x7C, 0x03, 0x0C, 0x85, 0xFF, 0x0F, 0x84, 0x0A, 0x01, 0x00, 0x00, 0x01, 0x5C,
    0x24, 0x08, 0x8D, 0x43, 0x04, 0x89, 0x44, 0x24, 0x2C, 0x8D, 0x43, 0x01, 0x89, 0x44, 0x24, 0x28,
    0x66, 0xB8, 0x02, 0x00, 0x89, 0x44, 0x24, 0x04, 0xC7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00, 0x8B,
    0x74, 0x24, 0x24, 0x89, 0xDD, 0x01, 0xFB, 0x53, 0xFF, 0x54, 0x24, 0x38, 0x89, 0x44, 0x24, 0x14,
    0x85, 0xC0, 0x74, 0x6E, 0x8B, 0x44, 0x24, 0x08, 0x8B, 0x18, 0x8B, 0x78, 0x10, 0x85, 0xDB, 0x0F,
    0x44, 0xDF, 0x8B, 0x44, 0x1D, 0x00, 0x85, 0xC0, 0x74, 0x7D, 0x01, 0xEF, 0x03, 0x5C, 0x24, 0x2C,
    0x85, 0xC0, 0x78, 0x0B, 0x8B, 0x4C, 0x24, 0x10, 0x01, 0xC8, 0x83, 0xC0, 0x02, 0xEB, 0x03, 0x0F,
    0xB7, 0xC0, 0x50, 0xFF, 0x74, 0x24, 0x18, 0xFF, 0x54, 0x24, 0x28, 0x89, 0x44, 0x24, 0x18, 0x8D,
    0x44, 0x24, 0x1C, 0x50, 0x6A, 0x40, 0xBD, 0x04, 0x00, 0x00, 0x00, 0x55, 0x57, 0xFF, 0xD6, 0x8B,
    0x44, 0x24, 0x18, 0x89, 0x07, 0x8D, 0x44, 0x24, 0x1C, 0x50, 0xFF, 0x74, 0x24, 0x20, 0x55, 0x57,
    0xFF, 0xD6, 0x01, 0xEF, 0x8B, 0x03, 0x01, 0xEB, 0x85, 0xC0, 0x75, 0xB4, 0x8B, 0x5C, 0x24, 0x10,
    0xEB, 0x49, 0x8A, 0x03, 0x84, 0xC0, 0x74, 0x23, 0x03, 0x7C, 0x24, 0x28, 0x31, 0xC9, 0x8B, 0x54,
    0x24, 0x4C, 0x8B, 0x5C, 0x24, 0x10, 0x88, 0x84, 0x0A, 0x24, 0x02, 0x00, 0x00, 0x8A, 0x04, 0x0F,
    0x41, 0x84, 0xC0, 0x75, 0xF1, 0xEB, 0x0E, 0x89, 0xEB, 0xEB, 0x27, 0x31, 0xC9, 0x8B, 0x54, 0x24,
    0x4C, 0x8B, 0x5C, 0x24, 0x10, 0xC6, 0x84, 0x0A, 0x24, 0x02, 0x00, 0x00, 0x00, 0xFF, 0x54, 0x24,
    0x30, 0x89, 0x04, 0x24, 0x31, 0xC0, 0x40, 0x89, 0x44, 0x24, 0x04, 0x83, 0x7C, 0x24, 0x14, 0x00,
    0x74, 0x7F, 0x8B, 0x44, 0x24, 0x08, 0x8B, 0x78, 0x20, 0x83, 0xC0, 0x14, 0x89, 0x44, 0x24, 0x08,
    0x85, 0xFF, 0x0F, 0x85, 0x1B, 0xFF, 0xFF, 0xFF, 0x8B, 0x44, 0x24, 0x0C, 0x8B, 0x84, 0x03, 0xC0,
    0x00, 0x00, 0x00, 0x85, 0xC0, 0x74, 0x3D, 0x8B, 0x74, 0x03, 0x0C, 0x85, 0xF6, 0x74, 0x19, 0x8B,
    0x06, 0x85, 0xC0, 0x74, 0x13, 0x83, 0xC6, 0x04, 0x6A, 0x00, 0x6A, 0x01, 0x53, 0xFF, 0xD0, 0x8B,
    0x06, 0x83, 0xC6, 0x04, 0x85, 0xC0, 0x75, 0xF0, 0x8B, 0x4C, 0x24, 0x4C, 0x8B, 0x01, 0x85, 0xC0,
    0x74, 0x06, 0x8B, 0x49, 0x04, 0x89, 0x04, 0x19, 0x8B, 0x44, 0x24, 0x0C, 0x8B, 0x44, 0x03, 0x28,
    0x01, 0xD8, 0xFF, 0xD0, 0xC7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x04, 0x24, 0x00,
    0x00, 0x00, 0x00, 0xEB, 0x0C, 0xFF, 0xD6, 0x89, 0x04, 0x24, 0x31, 0xC0, 0x40, 0x89, 0x44, 0x24,
    0x04, 0x8B, 0x0C, 0x24, 0xC1, 0xE1, 0x10, 0x0F, 0xB7, 0x44, 0x24, 0x04, 0x09, 0xC8, 0x83, 0xC4,
    0x38, 0x5E, 0x5F, 0x5B, 0x5D, 0xC2, 0x04, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
};

static_assert(sizeof(INJECT_SHELLCODE) % 16 == 0);

bool LoadSelf(HANDLE hProcess, void* userdata, size_t userdataSize)
{
    remote_param lModule = {
        .pLoadLibraryW = LoadLibraryW,
        .pLoadLibraryA = LoadLibraryA,
        .pVirtualProtect = VirtualProtect,
        .pGetProcAddress = GetProcAddress,
        .pGetLastError = GetLastError,
    };
    GetModuleFileNameW(NULL, lModule.sExePath, MAX_PATH);

    // User data
    if (userdata) {
        if (auto rUserData = VirtualAllocEx(hProcess, nullptr, userdataSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)) {
            WriteProcessMemory(hProcess, rUserData, userdata, userdataSize, nullptr);
            lModule.pUserData = rUserData;
            lModule.pAddrOfUserData = (PUINT8)((PUINT8)GetUserData() - (PUINT8)GetModuleHandleA(nullptr));
        }
    }    

    // Write shellcode and parameters
    LPVOID rModule = VirtualAllocEx(hProcess, nullptr, sizeof(remote_param), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!rModule)
        return false;
    defer(VirtualFreeEx(hProcess, rModule, 0, MEM_RELEASE));
    WriteProcessMemory(hProcess, rModule, &lModule, sizeof(remote_param), nullptr);
    LPVOID pRemoteInit = VirtualAllocEx(hProcess, nullptr, sizeof(INJECT_SHELLCODE), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!pRemoteInit)
        return false;
    defer(VirtualFreeEx(hProcess, pRemoteInit, 0, MEM_RELEASE));
    WriteProcessMemory(hProcess, pRemoteInit, INJECT_SHELLCODE, sizeof(INJECT_SHELLCODE), nullptr);

    // Invoke
    InjectResult rResult;
    if (auto tInit = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)pRemoteInit, rModule, 0, nullptr)) {
        WaitForSingleObject(tInit, INFINITE);
        GetExitCodeThread(tInit, (DWORD*)&rResult);
    } else {
        return false;
    }
    DWORD byteRet;
    ReadProcessMemory(hProcess, rModule, &lModule, sizeof(remote_param), &byteRet);

    if (rResult.error != InjectResult::Ok)
        return false;

    return true;
}

void** GetUserData()
{
    static void* userdata = nullptr;
    return &userdata;
}
}
