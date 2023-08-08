#pragma warning (push, 0)
/*
 * Hacker Disassembler Engine 32 C
 * Copyright (c) 2008-2009, Vyacheslav Patkov.
 * All rights reserved.
 *
 */

#if defined(_M_IX86) || defined(__i386__)

#include "hde32.h"
#include "table32.h"
#include <distorm.h>
#include <mnemonics.h>
#include <intrin.h>

unsigned int hde32_disasm(const void *code, hde32s *hs)
{
	// Replace MinHook's HDE with distorm.
	// This is a code-specific hack, it does not act as a complete reimplemention of HDE.

#ifndef _MSC_VER
	memset((LPBYTE)hs, 0, sizeof(hde32s));
#else
	__stosb((LPBYTE)hs, 0, sizeof(hde32s));
#endif

	unsigned int diCount = 0;
	_DInst di;
	_CodeInfo ci;
	ci.code = code;
	ci.codeLen = 0x20; // Max instruction length is 0x15
	ci.codeOffset = 0;
#if defined(_M_IX86) || defined(__i386__)
	ci.dt = Decode32Bits;
#elif defined(_M_X64) || defined(__x86_64__)
	ci.dt = Decode64Bits;
#endif
	ci.features = DF_NONE;

	_DecodeResult res = distorm_decompose(&ci, &di, 1, &diCount);
	if (di.flags == FLAG_NOT_DECODABLE ||
		di.opcode == I_UNDEFINED)
	{
		hs->flags |= F_ERROR;
		return 0;
	}

	hs->opcode = *((uint8_t*)code);
	if (hs->opcode == 0x0f)
		hs->opcode2 = *((uint8_t*)((uint32_t)code + 1));
	hs->len = di.size;
	if (di.dispSize)
		hs->disp.disp32 = (uint32_t)di.disp;
	hs->imm.imm32 = (uint32_t)di.imm.dword;
	if (di.flags & FLAG_RIP_RELATIVE)
		hs->modrm = 0x5;
	for (int i = 0; i < 4; i++)
	{
		if (di.ops[i].type == O_IMM)
		{
			hs->flags |= (di.ops[i].size >> 1);
			break;
		}
	}

	return di.size;
}

#endif // defined(_M_IX86) || defined(__i386__)
