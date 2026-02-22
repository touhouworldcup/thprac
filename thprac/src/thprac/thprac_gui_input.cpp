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

        int __gbackspace_menu_chord_current = 1 << ChordKey_Backspace;
        int __gadvanced_menu_chord_current = 1 << ChordKey_F12;
        int __gscreenshot_chord_current = 1 << ChordKey_Home;
        int __gtracker_chord_current = 1 << ChordKey_End;
        int __glanguage_chord_current = 1 << ChordKey_Alt;

        const char* ChordKeyStrings[ChordKey_COUNT];

		int ChordKeyVKs[ChordKey_COUNT];

		void MenuChordInitArrays() {
            ChordKeyStrings[ChordKey_Ctrl] =		"Ctrl";
            ChordKeyStrings[ChordKey_Shift] =		"Shift";
            ChordKeyStrings[ChordKey_Alt] =			"Alt";
            ChordKeyStrings[ChordKey_Caps] =		"Caps";
            ChordKeyStrings[ChordKey_Tab] =			"Tab";
            ChordKeyStrings[ChordKey_Space] =		S(THPRAC_HOTKEY_SPACEBAR);            
			ChordKeyStrings[ChordKey_Backspace] =	"Backspace";
            ChordKeyStrings[ChordKey_F11] =			"F11";
            ChordKeyStrings[ChordKey_F12] =			"F12";
            ChordKeyStrings[ChordKey_Insert] =		"Insert";
            ChordKeyStrings[ChordKey_Home] =		"Home";
            ChordKeyStrings[ChordKey_PgUp] =		"PgUp";
            ChordKeyStrings[ChordKey_Delete] =		"Delete";
            ChordKeyStrings[ChordKey_End] =			"End";
            ChordKeyStrings[ChordKey_PgDn] =		"PgDn";
            ChordKeyStrings[ChordKey_DPad_Up] =		"Up";
            ChordKeyStrings[ChordKey_DPad_Down] =	"Down";
            ChordKeyStrings[ChordKey_DPad_Left] =	"Left";
            ChordKeyStrings[ChordKey_DPad_Right] =	"Right";
            ChordKeyStrings[ChordKey_A] =			"A";
            ChordKeyStrings[ChordKey_B] =			"B";
            ChordKeyStrings[ChordKey_X] =			"X";
            ChordKeyStrings[ChordKey_Y] =			"Y";
            ChordKeyStrings[ChordKey_L1] =			"L1";
            ChordKeyStrings[ChordKey_L2] =			"L2";
            ChordKeyStrings[ChordKey_R1] =			"R1";
            ChordKeyStrings[ChordKey_R2] =			"R2";
            ChordKeyStrings[ChordKey_Start] =		"Start";
            ChordKeyStrings[ChordKey_Select] =		"Select";
            ChordKeyStrings[ChordKey_HomeMenu] =	"HomeMenu";

            ChordKeyVKs[ChordKey_Ctrl] =        VK_CONTROL;
            ChordKeyVKs[ChordKey_Shift] =       VK_SHIFT;
            ChordKeyVKs[ChordKey_Alt] =		    VK_MENU;
            ChordKeyVKs[ChordKey_Caps] =		VK_CAPITAL;
            ChordKeyVKs[ChordKey_Tab] =         VK_TAB;
            ChordKeyVKs[ChordKey_Space] = 		VK_SPACE;
            ChordKeyVKs[ChordKey_Backspace] =   VK_BACK;
            ChordKeyVKs[ChordKey_F11] =         VK_F11;
            ChordKeyVKs[ChordKey_F12] =         VK_F12;
            ChordKeyVKs[ChordKey_Insert] =      VK_INSERT;
            ChordKeyVKs[ChordKey_Home] =        VK_HOME;
            ChordKeyVKs[ChordKey_PgUp] =        VK_PRIOR;
            ChordKeyVKs[ChordKey_Delete] =      VK_DELETE;
            ChordKeyVKs[ChordKey_End] =         VK_END;
            ChordKeyVKs[ChordKey_PgDn] =        VK_NEXT;
            ChordKeyVKs[ChordKey_DPad_Up] =     0, // Currently no controller support
            ChordKeyVKs[ChordKey_DPad_Down] =   0;
            ChordKeyVKs[ChordKey_DPad_Left] =	0;
            ChordKeyVKs[ChordKey_DPad_Right] =	0;
            ChordKeyVKs[ChordKey_A] =			0;
            ChordKeyVKs[ChordKey_B] =			0;
            ChordKeyVKs[ChordKey_X] =           0;
            ChordKeyVKs[ChordKey_Y] =           0;
            ChordKeyVKs[ChordKey_L1] =          0;
            ChordKeyVKs[ChordKey_L2] =          0;
            ChordKeyVKs[ChordKey_R1] =          0;
            ChordKeyVKs[ChordKey_R2] =          0;
            ChordKeyVKs[ChordKey_Start] =       0;
            ChordKeyVKs[ChordKey_Select] =      0;
            ChordKeyVKs[ChordKey_HomeMenu] =    0;
            
		}

		bool MenuChordInitFromCfg() {
            int backspace_menu_chord = 0;
            int advanced_menu_chord = 0;
            int screenshot_chord = 0;
            int tracker_chord = 0;
            int language_chord = 0;
            
			MessageBoxW(NULL, L"TODO: implement settings JSON", NULL, 0);
            return false;
			#if 0
            __gbackspace_menu_chord_current = backspace_menu_chord;
            __gadvanced_menu_chord_current = advanced_menu_chord;
            __gscreenshot_chord_current = screenshot_chord;
            __gtracker_chord_current = tracker_chord;
            __glanguage_chord_current = language_chord;
            return true;
			#endif
		}

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
		
		int GetBackspaceMenuChord() { return __gbackspace_menu_chord_current; }
		int GetAdvancedMenuChord() { return __gadvanced_menu_chord_current; }
		int GetScreenshotChord() { return __gscreenshot_chord_current; }
		int GetTrackerChord() { return __gtracker_chord_current; }
		int GetLanguageChord() { return __glanguage_chord_current; }

		// Convert chords to user-readable string.
        std::string HotkeyChordToLabel(int chord) {

            // Special case for if no keys are pressed at all
            if (chord == 0)
                return std::string(S(THPRAC_HOTKEY_UNASSIGNED));

            std::string s;

            // See if the key is present in the chord and add it to the string if so.
            for (int key = 0; key < ChordKey_COUNT; ++key) {
                if (chord & (1 << key)) {
                    s += ChordKeyStrings[key];
                    s += std::string(" + ");
                }
            }
            // Cut off last " + " and return our label string.
            s.resize(s.length() - 3);
            return s;
        }

		// Convert chords to usable VKs
		int HotkeyChordToVK(int chord) {
			if (chord < 0 || chord >= ChordKey_COUNT) {
				return 0;
            } else {
				return ChordKeyVKs[chord];
			}
		}

		void HotkeyInit() {
			MessageBoxW(NULL, L"TODO: implement settings JSON", NULL, MB_OK);
			MenuChordInitArrays();
		}
	}
}
