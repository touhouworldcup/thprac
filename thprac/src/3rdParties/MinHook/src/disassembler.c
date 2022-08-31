#include "disassembler.h"
#include <distorm.h>
#include <mnemonics.h>

BOOL DisAsm(void* code, DisAsmCtx* ctx)
{
    // Replace MinHook's HDE with distorm.
    // This is a code-specific hack, it does not act as a complete reimplemention of HDE.

    memset((LPBYTE)ctx, 0, sizeof(DisAsmCtx));

    unsigned int diCount = 0;
    _DInst di;
    _CodeInfo ci;
    ci.code = (uint8_t*)code;
    ci.codeLen = 0x20; // Max instruction length is 0x15
    ci.codeOffset = 0;
#if defined(_M_IX86) || defined(__i386__)
    ci.dt = Decode32Bits;
#elif defined(_M_X64) || defined(__x86_64__)
    ci.dt = Decode64Bits;
#endif
    ci.features = DF_NONE;

    _DecodeResult res = distorm_decompose(&ci, &di, 1, &diCount);
    if (di.flags == FLAG_NOT_DECODABLE || di.opcode == I_UNDEFINED) {
        return FALSE;
        //hs->flags |= F_ERROR;
        //return 0;
    }

    ctx->ins[0] = ((uint8_t*)code)[0];
    if (ctx->ins[0] == 0x0f) { // Is it a two byte instruction?
        ctx->ins[1] = ((uint8_t*)code)[1];
    } else {
        ctx->ins[1] = 0;
    }
    ctx->insLen = di.size;
    ctx->imm.imm32 = di.imm.dword;
    for (int i = 0; i < 4; i++) {
        if (di.ops[i].type == O_IMM) {
            ctx->immLen = di.ops[i].size;
            break;
        }
    }
    ctx->disp.disp64 = di.disp;
    ctx->dispLen = di.dispSize;
    ctx->isRipRelative = di.flags & FLAG_RIP_RELATIVE;

    return TRUE;
}

