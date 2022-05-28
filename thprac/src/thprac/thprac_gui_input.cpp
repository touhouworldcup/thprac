#include "thprac_gui_input.h"
#include "thprac_gui_impl_win32.h"
#define NOMINMAX
#include <Windows.h>
#include <cstdint>

namespace THPrac
{
	namespace Gui
	{
		/***                In-Game Input                     ***/

		// Gen 1 (TH06 ~ TH08)
		static int16_t* __gi_gen1_r1 = nullptr;
		static int16_t* __gi_gen1_r2 = nullptr;
		static int16_t* __gi_gen1_r3 = nullptr;
		__forceinline bool __GI_Gen1_Reg3Test3(int16_t mask)
		{
			if (*__gi_gen1_r1 & mask)
				if ((*__gi_gen1_r1 & mask) != (*__gi_gen1_r2 & mask) || *__gi_gen1_r3)
					return true;
			return false;
		}
		__forceinline bool __GI_Gen1_Reg3Test2(int16_t mask)
		{
			if (*__gi_gen1_r1 & mask)
				if ((*__gi_gen1_r1 & mask) != (*__gi_gen1_r2 & mask))
					return true;
			return false;
		}
		__forceinline bool __GI_Gen1_Init(int reg1, int reg2, int reg3)
		{
			__gi_gen1_r1 = (int16_t*)reg1;
			__gi_gen1_r2 = (int16_t*)reg2;
			__gi_gen1_r3 = (int16_t*)reg3;
			return (!reg1 || !reg2 || !reg3);
		}
		__forceinline bool __GI_Gen1_Get(int key)
		{
			switch (key)
			{
			case 0x40:
				return false;
			case VK_UP:
				return __GI_Gen1_Reg3Test3(0x10);
			case VK_DOWN:
				return __GI_Gen1_Reg3Test3(0x20);
			case VK_LEFT:
				return __GI_Gen1_Reg3Test3(0x40);
			case VK_RIGHT:
				return __GI_Gen1_Reg3Test3(0x80);
			case VK_LSHIFT:
				return __GI_Gen1_Reg3Test2(0x4);
			case 0x5A: // Z
				return __GI_Gen1_Reg3Test2(0x1);
			default:
				return false;
			}
		}

		// Gen 2 (TH10 ~ Now)
		static int32_t* __gi_gen2_r1 = nullptr;
		static int8_t*  __gi_gen2_r2 = nullptr;
		__forceinline bool __GI_Gen2_Reg2Test2(int mask)
		{
			if (*__gi_gen2_r1 & mask || *__gi_gen2_r2 & mask) return true;
			return false;
		}
		__forceinline bool __GI_Gen2_Reg2Test1(int mask)
		{
			if (*__gi_gen2_r1 & mask) return true;
			return false;
		}
		__forceinline bool __GI_Gen2_Init(int reg1, int reg2, int reg3)
		{
			__gi_gen2_r1 = (int32_t*)reg1;
			__gi_gen2_r2 = (int8_t*)reg2;
			return (!reg1 || !reg2);
		}
		__forceinline bool __GI_Gen2_Get(int key)
		{
			switch (key)
			{
			case 0x40:
				return __GI_Gen2_Reg2Test2(0xf0);
			case VK_UP:
				return __GI_Gen2_Reg2Test2(0x10);
			case VK_DOWN:
				return __GI_Gen2_Reg2Test2(0x20);
			case VK_LEFT:
				return __GI_Gen2_Reg2Test2(0x40);
			case VK_RIGHT:
				return __GI_Gen2_Reg2Test2(0x80);
			case VK_LSHIFT:
				return __GI_Gen2_Reg2Test1(0x8);
			case 'X': // Z
				return __GI_Gen2_Reg2Test1(0x2);
			case 'Z': // Z
				return __GI_Gen2_Reg2Test1(0x1);
			default:
				return false;
			}
		}

		// API
		static ingame_input_gen_t __gi_gen = INGAGME_INPUT_NONE;
		bool InGameInputInit(ingame_input_gen_t gen, int reg1, int reg2, int reg3)
		{
			__gi_gen = gen;
			switch (gen)
			{
			case INGAGME_INPUT_GEN1:
				return __GI_Gen1_Init(reg1, reg2, reg3);
				break;
			case INGAGME_INPUT_GEN2:
				return __GI_Gen2_Init(reg1, reg2, reg3);
				break;
			default:
				return false;
			}
		}
		bool InGameInputGet(int key)
		{
			switch (__gi_gen)
			{
			case INGAGME_INPUT_GEN1:
				return __GI_Gen1_Get(key);
			case INGAGME_INPUT_GEN2:
				return __GI_Gen2_Get(key);
			default:
				return false;
			}
		}


		
		/***                   Keyboard Input                     ***/

		static uint8_t __ki_keystatus[0xFF]{};
		int KeyboardInputUpdate(int v_key)
		{
			if (ImplWin32CheckForeground() && GetAsyncKeyState(v_key) < 0)
			{
				if (__ki_keystatus[v_key] != 0xff)
					++__ki_keystatus[v_key];
			}
			else
				__ki_keystatus[v_key] = 0;
			return __ki_keystatus[v_key];
		}
		template<class... Args>
		int KeyboardInputUpdate(int key, Args... rest)
		{
			UpdateKey(v_key);
			UpdateKey(rest...);
		}
		void KeyboardInputUpdate()
		{
			for (auto i = 0; i < 0xff; ++i)
				KeyboardInputUpdate(i);
		}
		int  KeyboardInputGet(int v_key)
		{
			return __ki_keystatus[v_key];
		}
		bool KeyboardInputGetSingle(int v_key)
		{
			return __ki_keystatus[v_key] == 1;
		}
		bool KeyboardInputGetRaw(int v_key)
		{
			return (ImplWin32CheckForeground()) ? (GetAsyncKeyState(v_key) < 0) : false;
		}
	}
}