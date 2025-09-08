#include "thprac_gui_input.h"
#include "thprac_gui_locale.h"
#include "thprac_gui_impl_win32.h"
#include "thprac_launcher_cfg.h"
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

		
		/***                      Menu Chords                        ***/

		
        int __gbackspace_menu_chord_current;
        int __gadvanced_menu_chord_current;
        int __gspecial_menu_chord_current;
        int __gscreenshot_chord_current;


		bool MenuChordInitFromCfg() {
            int backspace_menu_chord = 0;
            int advanced_menu_chord = 0;
            int special_menu_chord = 0;
            int screenshot_chord = 0;
            if (
				!LauncherSettingGet("backspace_menu_chord", backspace_menu_chord) ||
				!LauncherSettingGet("advanced_menu_chord", advanced_menu_chord) || 
				!LauncherSettingGet("special_menu_chord", special_menu_chord) || 
				!LauncherSettingGet("screenshot_chord", screenshot_chord)
			) {
                return false;
            }
            __gbackspace_menu_chord_current = backspace_menu_chord;
            __gadvanced_menu_chord_current = advanced_menu_chord;
            __gspecial_menu_chord_current = special_menu_chord;
            __gscreenshot_chord_current = screenshot_chord;
            return true;
		}

		void MenuChordAutoSet() {
			__gbackspace_menu_chord_current = 1 << ImGuiKey_Backspace;
			__gadvanced_menu_chord_current = 1 << ImGuiKey_F12;
			__gspecial_menu_chord_current = 1 << ImGuiKey_F11;
			__gscreenshot_chord_current = 1 << ImGuiKey_Home;
		}


		// Returns the time the desired chord has been pressed for.
		int GetChordPressedDuration(int target_chord) {
			// Very bad copy paste...
			const int KeyMap[] = {
				VK_TAB,
				VK_LEFT,
				VK_RIGHT,
				VK_UP,
				VK_DOWN,
				VK_PRIOR,
				VK_NEXT,
				VK_HOME,
				VK_END,
				VK_INSERT,
				VK_DELETE,
				VK_BACK,
				VK_SPACE,
				VK_RETURN,
                VK_ESCAPE,
                VK_RETURN,
				VK_CONTROL,
				VK_MENU,
				VK_SHIFT,
				VK_CAPITAL,
				VK_F11,
				VK_F12
			};

            int min_held = 2;

            // Scan for keys until the keyboard keys which are not supported.
            for (int key = 0; key <= ImGuiKey_F12; ++key) {
                // If the key is in the target chord, check for duration that key was held.
                if (target_chord & (1 << key)) {
                    int held = KeyboardInputUpdate(KeyMap[key]);
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
		int GetSpecialMenuChord() { return __gspecial_menu_chord_current; }
		int GetScreenshotChord() { return __gspecial_menu_chord_current; }

		// Convert ImGui Chords to user-readable string.
        std::string HotkeyChordToLabel(int chord) {
            // Dirty hardcoded
            const char* KEY_NAMES[] = {
                "Tab",
                "If you see this, I messed up",
                "If you see this, I messed up",
                "If you see this, I messed up",
                "If you see this, I messed up",
                "PageUp",
                "PageDown",
                "Home",
                "End",
                "Insert",
                "Delete",
                "Backspace",
                S(THPRAC_HOTKEY_SPACEBAR),
                "If you see this, I messed up",
                "Esc",
                "If you see this, I messed up",
                "Ctrl",
                "Alt",
                "Shift",
                "Caps",
                "F11",
                "F12"
            };

            // Special case for if no keys are pressed at all
            if (chord == 0)
                return std::string(S(THPRAC_HOTKEY_UNASSIGNED));

            std::string s;

            // Ctrl Alt Shift are always first.
            if (chord & (1 << ImGuiKey_Ctrl))
                s += std::string("Ctrl + ");
            if (chord & (1 << ImGuiKey_Alt))
                s += std::string("Alt + ");
            if (chord & (1 << ImGuiKey_Shift))
                s += std::string("Shift + ");

            // Add the rest of the keys in the chord to the text.
            for (int key = 0; key <= ImGuiKey_F12; ++key) {
                // We've already added these texts first, no need to do it again.
                if (key == ImGuiKey_Ctrl || key == ImGuiKey_Alt || key == ImGuiKey_Shift)
                    continue;
                // Idk why this entry exists when its just the same as Enter.
                if (key == ImGuiKey_KeyPadEnter)
                    continue;

                // See if the key is present in the chord and add it to the string if so.
                if (chord & (1 << key)) {
                    s += KEY_NAMES[key];
                    s += std::string(" + ");
                }
            }

            // Cut off last " + " and return our label string.
            s.resize(s.length() - 3);
            return s;
        }

	}
}
