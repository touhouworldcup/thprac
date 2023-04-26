#pragma once

namespace THPrac
{
	namespace Gui
	{
		// In-Game Input
		enum ingame_input_gen_t
		{
			INGAGME_INPUT_NONE,
				INGAGME_INPUT_GEN1,
				INGAGME_INPUT_GEN2,
		};
		bool InGameInputInit(ingame_input_gen_t gen, int reg1, int reg2, int reg3 = 0);
		bool InGameInputGet(int key);


		// Keyboard Input
		int  KeyboardInputUpdate(int v_key);
		void KeyboardInputUpdate();
		template<class... Args>
		int  KeyboardInputUpdate(int key, Args... rest);
		int  KeyboardInputGet(int v_key);
		bool KeyboardInputGetSingle(int v_key);
		bool KeyboardInputGetRaw(int v_key);

		bool InGameInputGetConfirm();
	}
}
