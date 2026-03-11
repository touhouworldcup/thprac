#include "thprac_gui_input.h"
#include "thprac_gui_locale.h"
#include "thprac_gui_impl_win32.h"
#include <imgui.h>
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
		__forceinline bool __GI_Gen2_Init(int reg1, int reg2, [[maybe_unused]] int reg3)
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
				return __GI_Gen2_Reg2Test1(0x6);
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

		bool InGameInputGetConfirm()
		{
			return Gui::InGameInputGet('Z') || KeyboardInputGetRaw(VK_RETURN);
		}

		
		/***                    Menu Chords                          ***/

		constinit const char* ChordKeyStrings[ChordKey_COUNT] = {
			"Ctrl",
			"Shift",
			"Alt",
			"Caps",
			"Tab",
			"Spacebar",
			"Backspace",
			"F11",
			"F12",
			"Insert",
			"Home",
			"PgUp",
			"Delete",
			"End",
			"PgDn",
			"Up",
			"Down",
			"Left",
			"Right",
			"A",
			"B",
			"X",
			"Y",
			"L1",
			"L2",
			"R1",
			"R2",
			"Start",
			"Select",
			"HomeMenu",
		};
		int ChordKeyVKs[ChordKey_COUNT] = {
			VK_CONTROL,
			VK_SHIFT,
			VK_MENU,
			VK_CAPITAL,
			VK_TAB,
			VK_SPACE,
			VK_BACK,
			VK_F11,
			VK_F12,
			VK_INSERT,
			VK_HOME,
			VK_PRIOR,
			VK_DELETE,
			VK_END,
			VK_NEXT,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
		};

		// Returns the time the desired chord has been pressed for.
		int GetChordPressedDuration(int target_chord) {
            int min_held = 1 << 30;
            // Scan for keys until the keyboard keys which are not supported.
            for (int key = 0; key < ChordKey_KEYBOARD_COUNT; ++key) {
                // If the key is in the target chord, check for duration that key was held.
                if (target_chord & (1 << key)) {
                    //int held = KeyboardInputUpdate(HotkeyChordToVK(key));
                    int held = KeyboardInputUpdate(HotkeyChordToVK(key));
					// If one of the required keys is not pressed, set held time to 0 and break out early.
                    if (held == 0) {
                        min_held = 0;
                        break;
					// If it is held, update the least key held duration
                    } else if (held < min_held) {
                        min_held = held;
					}
                }
            }

			return min_held;
		}

		bool GetChordPressed(int chord) {
			return GetChordPressedDuration(chord) == 1;
		}
		
        unsigned int HotkeyChordToLabel(int chord, char* chord_name) {
            char* p = chord_name;

            for (int key = 0; key < ChordKey_COUNT; key++) {
                if (chord & (1 << key)) {

                    auto* keyname = ChordKeyStrings[key];
                    if (key == ChordKey_Space) {
                        keyname = S(THPRAC_HOTKEY_SPACEBAR);
                    }

                    size_t keylen = strlen(keyname);

                    memcpy(p, keyname, keylen);
                    p += keylen;
                    *p++ = ' ';
                    *p++ = '+';
                    *p++ = ' ';
                }
            }

            if (p != chord_name) {
                *p-- = 0;
                *p-- = 0;
                *p-- = 0;
            } else {
                strcpy(p, S(THPRAC_HOTKEY_UNASSIGNED));
            }
            return p - chord_name;
        }

		// Convert chords to usable VKs
		int HotkeyChordToVK(int chord) {
			if (chord < 0 || chord >= ChordKey_COUNT) {
				return 0;
            } else {
				return ChordKeyVKs[chord];
			}
		}
	}
}
