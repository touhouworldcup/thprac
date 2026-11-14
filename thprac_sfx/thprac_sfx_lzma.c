#define Z7_LZMA_SIZE_OPT 
#define Z7_EXTRACT_ONLY

#include "lzma-sdk/LzmaDec.c"
#include "lzma-sdk/CpuArch.c"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "thprac_sfx_lzma_config.h"
#define DECOMP_BUF_SIZE EXE_SIZE + DLL32_SIZE + DLL64_SIZE

static const Byte PAYLOAD[] = {
    #embed "payload.bin"
};

#pragma function(memcpy)
void* memcpy(void* dst, const void* src, size_t n) {
    Byte* dst_real = (Byte*)dst;
    const Byte* src_real = (Byte*)src;
    for (size_t i = 0; i < n; i++) {
        dst_real[i] = src_real[i];
    }
    return dst;
}

void* memset(void* dst, int c, size_t n) {
    Byte* dst_real = (Byte*)dst;
    for(size_t i = 0; i < n; i++) {
        dst_real[i] = (Byte)c;
    }
    return dst;
}

typedef struct EverythingBuffer EverythingBuffer;
struct EverythingBuffer {
    wchar_t path_buf[32768];
    CLzmaProb probs[NUM_BASE_PROBS + ((UInt32)LZMA_LIT_SIZE << (LZMA_LC + LZMA_LP))];
    Byte decomp_buf[DECOMP_BUF_SIZE];
};

void write_out(const wchar_t* self_path, SizeT path_len, wchar_t* path_buf, Byte* decomp_buf, SizeT exe_size, SizeT dll32_size, SizeT dll64_size) {
    memcpy(path_buf, self_path, (path_len - 3) * sizeof(wchar_t));
    path_buf[path_len - 3] = L'o';
    path_buf[path_len - 2] = L'l';
    path_buf[path_len - 1] = L'd';

    DeleteFileW(path_buf);
    if(!MoveFileW(self_path, path_buf)) {
        ExitProcess(2);
    }

    HANDLE h_exe = CreateFileW(self_path, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        
    // get length of the directory part
    SIZE_T dir_len = 0;
    for (SIZE_T i = path_len; i > 0; i--) {
        if (self_path[i - 1] == L'\\') { dir_len = i; break; }
    }

    // get absolute index of the first '.' in the file name
    SIZE_T first_dot_pos = dir_len;
    while (first_dot_pos < path_len&& self_path[first_dot_pos] != L'.') first_dot_pos++;

    // prepare dll paths: shift everything from the first '.' onward right by 2,
    // including the null terminator
    for (SIZE_T i = path_len - first_dot_pos + 1; i-- > 0; ) {
        path_buf[first_dot_pos + i + 2] = path_buf[first_dot_pos + i];
    }

    {
        wchar_t* p = path_buf + path_len + 2 - 3;
        p[0] = L'd'; p[1] = L'l'; p[2] = L'l';
    }

    // dll32 path
    path_buf[first_dot_pos + 0] = L'3';
    path_buf[first_dot_pos + 1] = L'2';
    HANDLE h_dll32 = CreateFileW(path_buf, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // dll64 path
    path_buf[first_dot_pos + 0] = L'6';
    path_buf[first_dot_pos + 1] = L'4';
    HANDLE h_dll64 = CreateFileW(path_buf, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    WriteFile(h_exe,   decomp_buf,  exe_size,  nullptr, nullptr);
    WriteFile(h_dll32, decomp_buf + exe_size,  dll32_size, nullptr, nullptr);
    WriteFile(h_dll64, decomp_buf + exe_size + dll32_size, dll64_size, nullptr, nullptr);

    CloseHandle(h_exe);
    CloseHandle(h_dll32);
    CloseHandle(h_dll64);
}

typedef struct UNICODE_STRING UNICODE_STRING;
struct UNICODE_STRING {
    USHORT Length;
    USHORT MaxLength;
    PWSTR  Buffer;
};

#define elementsof(a) (sizeof(a) / sizeof((a)[0]))

void start(SizeT* peb) {
    auto* buf = (EverythingBuffer*)VirtualAlloc(0, sizeof(EverythingBuffer), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if(!buf) {
        ExitProcess(1);
    }

    CLzmaDec decoder = {
        .prop = {
            .lc = LZMA_LC,
            .lp = LZMA_LP,
            .pb = LZMA_PB,
            .dicSize = LZMA_DICSIZE
        },
        .probs = buf->probs,
        .probs_1664 = buf->probs + 1664,
        .dic = buf->decomp_buf,
        .dicBufSize = sizeof(buf->decomp_buf),
        .remainLen = kMatchSpecLenStart + 2,
        .numProbs = elementsof(buf->probs),
    };

    const Byte* comp_buf = PAYLOAD;
    SizeT comp_buf_len = sizeof(PAYLOAD);   

    ELzmaFinishMode finish_mode = {};
    ELzmaStatus status = {};

    LzmaDec_DecodeToDic(&decoder, sizeof(buf->decomp_buf), comp_buf, &comp_buf_len, finish_mode, &status);  
    
    // peb->ProcessParameters->ImagePathName
    UNICODE_STRING* self_path    = (UNICODE_STRING*)(peb[4] + 0x38);
    write_out(self_path->Buffer, self_path->Length / sizeof(wchar_t), buf->path_buf, buf->decomp_buf, EXE_SIZE, DLL32_SIZE, DLL64_SIZE);

    VirtualFree(buf, 0, MEM_RELEASE);

    // peb->ProcessParameters->CommandLine
    UNICODE_STRING* self_cmdline = (UNICODE_STRING*)(peb[4] + 0x40);

    STARTUPINFOW si = { .cb = sizeof(si) };
    PROCESS_INFORMATION pi = {};

    CreateProcessW(self_path->Buffer, self_cmdline->Buffer, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    ExitProcess(0);
}