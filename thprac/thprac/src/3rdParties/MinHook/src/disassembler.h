#pragma once
#define NOMINMAX
#include <Windows.h>
#include <stdint.h>

typedef struct _DisAsmCtx {
    uint8_t ins[2];
    size_t insLen;
    union {
        uint64_t imm64;
        uint32_t imm32;
        uint16_t imm16;
        uint8_t imm8;
    } imm;
    size_t immLen;
    union {
        uint64_t disp64;
        uint32_t disp32;
        uint16_t disp16;
        uint8_t disp8;
    } disp;
    size_t dispLen;

    // (modrm & 0xC7) == 0x05
    BOOL isRipRelative;
} DisAsmCtx;

#ifdef __cplusplus
extern "C" {
#endif

BOOL DisAsm(void* code, DisAsmCtx* ctx);

#ifdef __cplusplus
}
#endif
