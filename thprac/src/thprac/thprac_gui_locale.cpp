#include "thprac_gui_locale.h"
#include "thprac_launcher_cfg.h"
#include <imgui.h>
#include <imgui_freetype.h>

#define NOMINMAX
#include <Windows.h>

namespace THPrac {
namespace Gui {
#pragma region Japanese Glyph Range
static const short offsetsFrom0x4E00[] =
{
	-1,0,1,3,0,0,0,0,1,0,1,0,2,1,1,0,4,2,4,3,2,4,5,0,1,0,6,0,0,2,2,1,0,0,6,0,0,0,3,0,0,17,1,10,1,1,3,1,0,1,0,1,2,0,1,0,2,0,1,0,1,2,0,1,
	0,0,1,2,0,0,0,4,6,0,4,0,2,1,0,2,0,1,1,4,0,0,0,0,0,3,0,0,3,0,0,7,0,1,1,3,4,5,7,0,2,0,0,0,0,8,1,0,17,0,3,1,1,1,1,0,5,2,0,5,0,0,0,0,
	1,1,1,1,0,0,0,0,0,10,5,0,2,1,0,4,0,2,3,2,1,2,1,1,1,6,2,1,2,0,9,1,0,0,5,0,8,2,0,0,5,3,0,0,0,5,0,1,0,1,1,0,0,1,0,0,8,0,3,1,2,1,10,0,
	2,1,1,4,1,1,1,0,0,1,2,1,1,0,0,0,1,0,0,0,1,8,2,9,3,0,0,5,3,1,0,3,1,8,6,5,1,0,0,1,4,2,4,7,3,6,0,0,17,0,4,0,0,0,1,6,3,2,4,2,1,1,3,4,
	8,1,1,5,0,6,3,1,4,0,0,1,13,1,0,2,1,3,0,1,8,7,4,2,0,0,2,0,0,1,0,0,0,0,0,0,1,0,0,0,1,3,5,1,5,2,2,1,0,0,0,3,3,0,0,0,3,3,1,2,0,2,0,1,
	0,1,1,0,2,0,0,1,6,1,1,0,0,1,1,1,3,0,0,1,0,0,0,5,6,1,2,0,0,0,0,13,0,0,2,0,4,0,1,0,2,2,0,4,1,0,0,2,0,1,2,2,0,0,1,3,2,0,3,0,5,6,0,3,
	0,3,1,2,2,0,0,0,0,0,7,0,2,2,0,0,0,6,0,0,0,3,1,5,0,0,4,4,0,1,0,0,0,7,1,3,3,0,4,4,0,7,3,0,2,5,0,0,5,2,4,4,2,1,1,1,1,8,2,2,0,3,0,0,
	2,1,1,0,10,7,3,0,1,0,2,0,1,4,1,0,4,0,0,1,0,1,3,0,1,6,6,6,0,0,0,3,0,0,1,1,0,0,0,0,0,2,3,0,0,0,2,0,1,1,3,5,2,4,0,0,0,1,0,0,2,6,2,1,
	17,1,1,4,0,4,0,1,0,3,4,0,0,6,6,0,4,0,0,0,0,0,0,5,1,0,1,1,3,1,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,2,0,0,1,6,1,0,3,0,0,0,0,0,0,0,0,0,9,1,
	0,0,0,5,4,0,0,0,7,1,0,1,0,0,0,3,3,1,0,0,2,0,2,13,8,1,6,1,1,0,0,3,0,0,2,3,1,5,1,3,3,5,7,0,2,2,0,5,0,4,4,2,0,2,2,0,0,23,3,2,0,3,0,3,
	7,9,1,0,6,1,5,24,1,1,4,5,1,3,1,8,3,2,5,1,4,23,0,1,1,2,0,2,1,0,0,12,0,0,1,0,0,0,7,0,0,0,0,0,1,1,5,13,0,1,1,10,5,1,2,3,0,4,15,3,0,7,1,0,
	10,1,0,0,2,13,5,1,0,1,1,1,8,0,9,1,4,7,8,3,1,0,0,2,4,3,1,5,5,0,2,4,4,4,6,1,2,6,1,13,3,0,0,0,4,0,0,6,1,3,2,0,2,1,2,10,1,1,0,5,0,2,2,2,
	0,6,3,2,0,1,0,1,6,7,0,4,2,15,1,2,1,2,3,0,0,0,15,2,1,2,0,12,10,8,7,8,3,1,0,0,30,2,4,2,3,0,0,7,2,0,19,0,1,1,0,1,3,1,2,0,3,0,9,3,3,3,2,5,
	4,0,0,2,0,4,5,0,8,1,4,0,1,2,2,3,2,2,4,1,9,3,4,4,15,3,4,0,1,8,0,9,6,0,2,2,3,1,2,1,1,2,3,6,2,11,0,1,1,0,0,4,1,0,0,0,0,11,0,4,4,0,0,2,
	0,1,5,2,1,1,0,0,1,0,2,5,6,5,2,0,0,4,0,0,0,2,0,1,2,5,1,2,1,0,1,3,4,0,3,4,3,0,0,0,5,2,5,2,2,9,1,2,3,12,1,2,7,2,1,4,0,1,0,4,2,8,0,0,
	2,0,15,3,1,1,13,6,3,2,0,4,3,5,5,0,5,3,0,4,2,16,7,3,3,1,18,18,7,0,23,8,0,0,2,0,6,1,0,9,0,9,1,2,2,11,19,1,0,21,2,4,1,3,1,3,7,1,0,1,12,0,2,0,
	1,0,1,1,0,1,3,0,1,2,1,4,2,1,2,1,2,2,4,1,0,0,1,0,0,1,5,1,0,0,0,0,0,0,1,2,0,0,0,0,7,1,2,0,0,0,1,0,5,2,0,0,0,0,0,4,3,1,0,0,6,1,0,0,
	1,3,0,0,0,0,1,2,4,1,0,1,1,3,0,1,0,1,1,0,0,0,0,0,2,0,1,4,3,5,1,1,3,4,3,6,0,0,0,0,0,0,0,0,0,3,0,1,1,0,1,0,0,1,1,1,8,1,0,0,1,0,2,3,
	2,1,7,18,3,16,6,0,2,6,4,32,6,0,6,4,1,0,5,4,1,9,6,7,0,3,13,34,3,24,2,2,20,2,3,34,11,1,0,11,1,0,0,5,2,4,1,0,2,1,1,0,0,0,2,2,2,0,0,0,0,1,3,1,
	0,3,0,2,5,4,4,2,0,0,1,7,5,1,1,0,2,2,0,0,4,2,3,0,1,4,7,0,9,1,0,0,14,0,0,1,1,0,0,0,0,0,0,0,1,1,0,2,2,4,5,0,0,2,1,3,5,0,3,1,7,0,0,0,
	8,0,0,4,0,0,4,0,2,6,4,0,1,0,4,0,2,7,1,0,2,0,0,2,1,2,4,0,0,0,2,0,0,1,0,0,0,0,0,2,3,5,0,0,1,3,1,1,3,1,4,0,0,11,1,1,2,1,3,1,3,3,0,3,
	2,1,0,0,2,0,1,3,1,2,2,0,0,2,0,1,0,0,0,0,0,3,1,0,3,0,0,2,1,2,6,0,0,2,0,4,0,4,3,5,0,0,3,2,0,8,0,0,0,2,0,2,10,4,4,2,2,1,1,15,2,3,2,2,
	0,2,0,3,1,0,0,0,0,3,1,8,8,7,1,2,1,2,3,0,4,2,0,0,0,2,0,0,0,0,0,1,0,4,14,4,1,0,0,4,1,1,3,0,3,0,2,2,0,1,0,7,1,1,1,3,0,7,1,9,6,1,1,2,
	1,1,3,0,7,0,1,2,0,0,0,1,5,6,0,3,0,0,2,2,4,0,3,7,12,9,3,1,2,0,1,0,0,1,6,3,0,4,0,1,0,1,1,0,2,2,1,2,0,1,0,6,3,7,3,1,0,2,1,3,9,2,1,1,
	0,1,3,3,3,3,4,3,0,0,0,5,18,2,11,3,0,0,1,1,1,1,7,1,1,0,0,1,0,0,3,3,0,2,0,2,3,3,3,0,0,1,1,3,2,3,0,0,5,0,0,1,1,5,1,2,2,2,1,2,4,5,2,4,
	2,2,2,0,2,0,4,0,6,0,0,0,0,1,0,2,0,1,12,5,3,3,2,0,7,0,0,0,0,2,0,1,0,1,0,3,0,0,1,0,0,2,0,10,0,0,2,1,1,0,0,4,1,0,1,0,4,0,0,2,4,6,0,5,
	8,2,3,5,4,2,0,0,9,2,0,1,0,4,1,4,8,1,0,0,4,3,4,2,0,7,4,0,2,2,2,3,1,2,3,0,0,0,0,1,1,0,0,0,1,5,1,6,7,0,1,2,5,0,1,3,3,0,5,1,10,5,1,3,
	18,1,4,2,10,3,1,3,0,12,3,3,14,6,14,1,5,6,1,0,0,8,4,9,0,6,3,5,0,3,1,1,0,1,1,6,1,0,4,0,2,7,4,5,1,5,0,0,0,0,1,5,2,1,0,7,2,0,1,23,10,5,0,0,
	0,2,2,1,0,1,1,1,2,0,5,7,1,1,5,1,3,4,0,2,5,3,1,1,0,1,0,9,0,3,1,4,1,0,5,1,1,0,2,1,2,0,1,3,0,0,1,0,6,1,2,0,3,3,0,4,0,2,2,4,1,1,4,1,
	2,0,0,0,0,2,0,3,8,7,3,0,4,1,0,3,0,10,0,4,1,0,4,1,4,0,5,0,5,0,7,3,2,10,6,1,0,0,0,4,0,0,3,1,3,6,5,0,0,7,4,0,5,4,3,4,2,5,4,13,1,12,2,0,
	1,0,2,8,6,1,0,0,3,0,2,0,0,0,0,2,4,0,1,1,6,0,1,3,1,1,6,0,0,1,0,0,0,0,2,2,1,3,2,8,2,4,0,0,0,1,2,2,2,1,0,0,0,0,2,4,2,0,0,1,1,1,1,4,
	1,0,7,1,1,4,4,1,0,1,1,0,2,0,0,12,3,2,0,0,0,1,7,0,5,4,0,0,1,0,3,1,2,0,3,6,2,1,0,1,0,0,0,0,3,1,2,0,2,0,0,14,2,0,6,2,7,0,7,1,3,0,2,0,
	2,0,0,0,2,1,5,1,0,1,0,4,1,0,0,1,7,3,6,1,1,0,7,1,0,0,1,9,3,0,2,0,2,1,1,0,1,0,3,0,4,1,0,0,0,0,1,0,3,0,8,3,0,0,0,1,4,2,1,0,1,4,0,2,
	3,6,3,6,0,5,4,2,2,1,0,0,2,6,0,0,0,6,4,1,5,3,1,2,3,1,2,7,8,0,0,4,2,0,1,0,0,0,0,5,0,1,0,0,2,0,1,1,0,3,0,0,1,1,7,3,2,2,0,5,0,3,6,0,
	5,2,0,1,6,2,2,1,3,3,0,0,2,2,4,0,13,0,4,4,2,5,1,1,2,2,5,3,2,0,3,1,3,1,1,1,5,0,0,9,2,0,0,2,6,0,1,0,0,1,12,0,5,1,0,28,0,3,4,3,0,1,6,4,
	0,0,4,7,0,1,4,4,2,6,0,13,1,6,0,2,0,7,0,1,15,0,8,0,9,2,3,6,2,0,1,3,10,4,1,0,2,0,8,1,2,1,4,0,10,2,0,0,1,2,0,4,3,0,3,0,1,3,3,0,1,2,0,0,
	10,11,7,2,1,1,0,0,0,0,1,2,0,0,2,0,4,5,1,0,3,1,2,0,2,3,11,0,1,0,3,20,6,0,0,1,3,11,16,0,1,0,5,1,0,0,11,1,6,2,2,0,0,0,7,1,5,1,7,2,3,1,4,3,
	3,1,0,2,2,1,5,0,8,2,2,1,4,0,1,0,0,0,0,1,2,4,0,1,8,5,1,3,0,0,1,2,1,0,4,2,23,6,4,3,2,0,5,3,0,6,0,2,2,2,1,0,2,2,0,19,0,1,6,2,2,0,1,1,
	5,2,0,12,1,0,3,1,5,0,3,1,0,18,2,2,2,3,3,5,4,5,0,5,0,7,4,3,0,0,4,1,1,1,1,0,0,9,1,0,0,0,0,7,1,3,0,0,3,0,0,1,1,0,2,1,0,0,1,8,1,3,4,6,
	2,8,1,2,11,6,0,2,11,0,0,1,9,3,5,1,3,0,1,2,7,4,2,3,0,2,2,4,1,0,5,0,4,1,0,8,0,19,1,2,0,5,0,1,0,3,2,5,1,1,0,0,10,1,0,7,0,4,0,5,6,5,11,2,
	3,2,0,2,4,8,0,1,9,6,2,1,7,8,12,5,6,1,5,6,0,1,20,2,3,0,0,2,6,3,3,2,3,3,7,2,5,1,3,2,1,0,1,0,0,6,0,4,3,13,13,4,6,18,0,2,0,7,3,0,11,0,3,3,
	6,18,0,0,0,7,0,0,0,0,12,6,9,3,1,17,7,3,11,10,4,0,1,4,4,7,1,5,5,9,2,2,1,6,0,2,6,1,1,0,1,1,4,14,6,2,2,4,4,0,3,5,8,3,4,1,10,4,4,5,1,1,1,0,
	1,8,4,0,0,4,0,7,3,1,0,2,6,20,12,1,1,3,1,2,0,3,0,0,0,0,0,0,5,0,0,3,5,3,1,0,1,0,0,1,1,0,4,1,8,1,4,3,0,1,1,4,10,13,1,9,2,0,5,11,1,1,7,1,
	1,4,1,1,5,0,6,2,0,5,3,0,3,0,12,11,0,3,0,2,5,2,0,0,0,2,5,1,7,0,4,0,9,7,11,2,1,1,5,0,0,5,1,9,2,1,1,10,18,8,0,7,4,1,5,1,2,0,15,1,4,4,2,0,
	15,4,2,2,24,2,12,0,0,0,0,3,4,1,19,3,0,0,0,1,0,0,2,6,4,3,2,7,4,2,4,18,8,3,4,12,12,4,4,7,3,1,0,0,1,2,1,2,2,0,3,12,8,0,3,3,2,1,1,1,0,3,1,0,
	1,2,4,0,0,0,3,0,1,0,16,2,1,2,4,0,1,0,2,1,2,0,3,5,2,2,0,0,6,6,0,2,0,2,0,1,0,1,5,2,5,1,5,5,0,0,1,2,0,2,0,0,1,1,2,1,5,4,1,0,2,0,1,2,
	3,0,0,4,6,1,0,0,5,1,1,2,9,1,11,6,0,0,1,1,0,5,2,3,6,6,3,0,0,2,0,5,3,1,3,4,0,1,2,1,0,1,0,2,1,3,1,1,0,0,0,0,1,0,2,1,0,6,1,2,5,20,1,3,
	3,0,0,4,2,0,2,1,1,7,4,3,0,1,0,1,1,0,0,1,2,3,3,1,3,5,2,2,2,0,0,1,0,14,2,0,0,4,0,2,10,2,0,1,1,3,6,18,0,5,1,1,0,1,2,14,3,0,11,5,12,0,0,4,
	6,0,2,2,7,0,0,1,7,5,13,0,3,1,0,1,1,1,3,0,0,3,14,9,4,0,1,0,15,0,0,8,1,1,5,10,23,10,2,0,1,0,4,7,4,5,4,0,3,1,1,1,11,3,1,5,0,9,1,1,2,3,2,1,
	0,4,0,2,5,9,2,0,3,2,2,10,3,12,2,0,6,12,3,0,0,13,1,1,1,6,0,0,6,2,2,0,2,2,0,0,0,1,2,2,4,9,7,0,0,2,0,4,2,0,0,22,3,3,1,0,7,3,0,0,0,0,7,1,
	5,0,2,2,6,1,1,0,1,0,1,3,2,10,4,2,4,2,1,0,5,2,2,1,2,0,12,0,3,4,3,0,0,1,0,1,3,6,0,0,7,9,0,0,7,4,3,1,2,0,2,1,1,1,0,3,9,0,1,0,0,0,6,0,
	8,0,3,0,6,3,4,3,0,0,2,2,1,1,5,3,2,2,0,2,1,0,3,2,1,6,2,0,2,1,4,1,1,1,0,3,1,7,1,2,1,4,0,1,3,12,11,0,1,0,1,0,0,1,0,0,0,1,1,6,7,1,4,1,
	0,5,4,11,0,3,1,1,2,1,0,1,1,2,0,3,8,2,3,2,1,1,7,0,2,1,0,1,0,0,0,2,13,2,3,0,0,2,3,5,2,0,8,6,6,2,0,0,0,2,6,0,1,1,3,2,0,7,2,0,5,0,0,0,
	2,8,0,2,8,5,0,0,2,0,6,6,1,3,4,2,1,5,1,1,4,1,0,1,0,2,3,4,0,1,0,5,2,0,0,9,0,1,1,7,3,3,2,0,0,4,0,0,0,0,1,4,3,3,2,1,0,0,1,1,0,2,1,3,
	0,0,0,2,0,1,2,3,0,1,0,0,0,0,4,0,0,8,0,1,0,0,1,0,5,0,6,0,0,0,1,5,1,0,0,5,4,2,2,0,0,2,1,5,2,0,2,0,2,3,11,5,3,5,0,0,0,2,2,8,0,0,0,0,
	0,0,0,1,0,2,1,0,1,0,0,7,2,0,3,1,0,5,1,1,0,0,1,0,6,0,2,2,2,1,6,5,2,0,3,0,0,5,0,8,2,0,1,0,0,2,4,2,3,2,1,1,0,0,1,0,2,1,2,3,0,1,6,0,
	0,2,0,2,0,2,4,0,1,1,1,2,8,1,1,4,5,4,0,0,0,1,0,0,6,0,153,3,10,6,0,0,1,3,11,7,0,0,0,2,1,1,2,1,1,8,0,1,0,0,0,1,1,1,5,2,2,2,0,5,3,0,4,0,
	6,1,0,3,3,3,4,1,5,1,0,10,0,4,2,1,4,2,5,1,0,3,0,1,0,0,0,5,3,1,2,1,0,3,9,1,10,2,4,1,8,3,11,1,1,3,0,1,0,12,0,0,0,0,0,2,5,0,0,6,0,1,1,0,
	6,2,1,1,0,1,3,0,2,3,0,2,1,1,0,1,5,8,0,1,5,1,7,2,0,0,1,0,2,2,0,7,1,1,2,3,3,0,4,2,0,0,0,0,0,15,0,7,5,5,1,1,5,4,6,5,2,1,0,1,0,0,9,5,
	0,4,1,0,1,0,2,3,0,0,4,0,1,0,4,1,4,5,4,1,0,2,2,4,0,6,2,1,4,3,0,0,1,3,1,4,3,1,4,0,0,4,0,2,1,1,0,1,2,5,0,5,1,1,2,0,2,1,0,1,1,0,0,1,
	6,0,2,0,1,0,9,0,0,0,6,1,0,0,0,0,6,6,16,0,5,4,1,1,1,0,2,0,1,0,3,0,0,0,4,8,3,1,0,3,6,3,1,5,0,4,0,0,1,1,1,3,0,0,1,1,7,11,0,0,0,2,3,0,
	1,0,3,1,0,0,3,5,1,0,4,0,3,3,1,0,3,4,8,0,2,0,6,4,2,3,1,0,1,0,0,1,0,15,0,4,2,5,26,1,1,3,0,12,4,4,2,3,3,0,2,4,0,1,0,5,11,2,0,3,4,1,1,4,
	2,1,3,0,1,0,8,1,3,0,0,0,1,8,5,0,7,0,0,17,8,2,4,3,2,3,0,10,0,4,8,3,0,4,1,2,2,1,0,0,1,1,3,1,1,0,9,0,10,3,4,2,2,1,11,4,1,3,2,0,2,4,1,2,
	0,0,1,2,0,4,2,0,17,1,5,7,2,0,11,4,1,0,0,1,0,1,4,4,1,5,0,7,7,3,1,4,0,0,0,2,4,3,1,10,3,0,0,2,9,2,3,1,3,2,0,1,5,0,2,4,10,1,1,0,0,0,0,1,
	0,9,0,6,7,1,1,1,0,4,6,0,6,0,0,2,0,12,1,0,0,3,2,3,0,2,11,0,2,3,14,17,14,1,3,0,4,1,1,0,7,3,0,2,1,7,1,14,0,0,6,1,6,6,0,4,0,0,3,0,5,10,2,1,
	0,1,1,1,0,2,2,4,1,2,0,4,4,2,0,0,0,8,0,0,0,2,1,1,0,2,1,0,0,2,3,0,0,4,1,1,8,3,7,2,2,3,2,0,9,1,0,1,4,1,1,1,5,0,2,0,0,0,1,5,2,0,1,1,
	1,6,2,5,4,17,0,1,8,1,1,3,6,0,1,2,3,1,0,3,2,1,1,3,8,0,11,2,2,3,0,1,1,2,4,1,0,3,2,0,0,1,2,0,0,8,2,0,3,9,4,9,3,1,5,0,4,0,3,1,1,1,3,0,
	0,4,2,4,4,1,5,0,0,2,5,2,1,4,3,0,0,0,4,3,1,6,5,2,0,1,7,1,0,0,0,0,8,0,1,2,0,2,0,2,0,1,1,6,9,0,4,0,2,0,0,5,2,2,1,3,1,0,10,6,4,0,10,1,
	2,5,2,0,16,7,0,0,3,1,8,2,1,2,7,1,4,0,0,1,0,3,6,0,0,1,6,5,2,4,2,0,6,2,1,0,17,7,1,0,5,2,13,11,1,0,4,1,1,1,4,3,0,2,1,1,3,1,4,2,3,1,0,1,
	3,0,0,4,6,7,0,2,0,5,2,1,1,0,2,2,1,1,0,1,0,0,0,14,2,1,1,2,0,3,1,1,2,5,1,0,1,0,1,1,3,0,2,1,4,1,2,2,2,1,2,3,0,0,1,2,3,3,0,0,3,0,0,1,
	1,0,3,1,0,2,1,3,0,1,3,1,0,0,1,9,1,3,2,1,0,0,1,3,4,1,2,0,6,5,7,1,5,4,0,8,1,1,2,6,4,0,3,1,1,2,8,2,6,1,1,1,1,0,2,3,156,2,4,1,4,0,0,0,
	0,1,1,6,4,6,8,1,11,0,0,1,5,2,3,2,0,10,2,1,0,1,0,0,4,0,0,0,0,0,1,0,0,2,0,1,0,0,2,0,0,1,0,1,0,0,2,2,2,0,2,1,6,0,0,1,1,1,0,0,1,3,2,12,
	1,0,6,0,2,2,1,1,0,2,0,1,77,1,0,3,1,2,2,0,2,13,4,24,4,10,6,3,3,3,4,0,1,0,1,4,3,0,1,1,1,1,4,1,0,3,3,1,6,12,0,4,0,12,0,1,9,5,4,12,1,2,0,0,
	0,0,0,3,4,3,5,0,2,0,13,1,1,5,4,2,0,1,2,2,3,1,5,7,8,0,0,2,0,0,12,1,7,1,0,0,0,4,8,3,2,8,12,2,0,0,5,5,0,1,5,0,0,6,0,0,8,2,0,2,1,3,4,2,
	2,0,2,1,0,0,2,2,0,9,7,1,0,1,54,0,1,0,3,2,1,4,0,0,0,0,0,2,0,0,2,0,0,2,2,1,0,8,2,2,5,11,3,0,1,1,0,6,0,0,0,2,2,0,1,1,0,6,1,0,0,1,0,0,
	1,1,0,2,0,0,0,0,0,0,8,1,2,0,5,0,2,4,0,2,1,1,0,0,1,0,0,1,1,0,0,0,2,2,3,0,1,1,3,3,0,0,2,2,1,0,1,1,0,1,0,0,0,0,0,2,0,1,1,2,1,17,2,3,
	4,8,8,8,3,18,0,5,4,7,1,5,4,22,19,2,1,22,0,0,0,0,0,3,1,1,4,6,0,1,3,0,1,8,1,0,9,4,3,1,2,1,4,4,5,1,3,1,1,2,4,0,0,1,1,1,4,4,0,0,0,2,0,0,
	0,0,0,6,3,0,5,2,0,13,9,7,5,0,2,2,0,8,21,2,1,5,4,3,0,1,3,7,3,2,3,1,1,10,6,5,1,2,1,11,1,4,1,0,0,16,9,1,21,2,6,10,4,0,2,4,0,4,1,1,9,8,0,7,
	1,5,1,0,2,1,2,0,1,0,2,7,0,15,1,6,6,16,1,1,4,6,1,13,7,1,0,2,12,4,1,10,0,8,4,8,4,0,0,4,3,2,3,43,3,0,0,16,9,0,1,1,9,12,0,0,6,0,2,1,1,13,4,1,
	4,0,1,247,8,1,0,3,1,0,0,3,1,1,0,3,9,0,0,0,0,1,4,4,6,1,0,1,4,3,0,1,0,0,0,6,0,0,1,3,4,0,2,54,4,6,1,3,3,8,3,0,3,6,0,0,2,10,2,0,2,0,0,0,
	4,1,3,2,1,0,1,1,2,7,0,1,1,0,1,0,0,4,0,1,0,0,1,0,3,2,3,0,8,2,2,1,1,0,3,0,2,0,0,0,1,1,0,0,0,2,3,0,2,1,6,0,4,1,0,4,1,3,0,1,1,4,3,1,
	0,2,2,1,1,1,2,2,2,1,8,8,1,6,3,0,3,1,1,1,0,8,3,2,1,0,1,1,0,0,5,0,1,1,3,2,5,1,7,0,0,4,1,1,0,7,3,3,2,2,1,2,1,5,0,5,8,2,4,7,4,2,0,16,
	0,3,0,7,3,1,0,0,1,0,1,3,2,0,0,0,0,3,0,1,6,2,7,0,2,6,0,2,0,0,8,4,0,0,0,0,4,0,0,1,1,0,2,8,3,0,3,0,1,0,51,1,4,1,4,13,22,0,2,2,6,3,0,0,
	2,0,0,7,0,0,4,1,3,0,1,3,1,0,4,2,1,0,1,0,0,1,3,3,1,12,2,3,2,3,1,0,3,0,0,2,1,62,0,0,0,11,2,3,0,0,4,0,12,1,0,0,0,1,7,0,0,2,2,4,1,13,0,2,
	6,2,3,14,0,2,0,5,6,7,7,6,0,5,1,12,0,6,4,0,3,2,1,0,3,0,61,4,2,5,1,3,3,3,10,1,2,3,0,8,0,6,2,0,0,1,2,2,3,10,17,1,4,2,0,1,1,0,0,4,2,0,8,0,
	4,0,0,0,0,7,0,0,1,2,3,1,0,2,4,4,3,2,3,0,4,9,0,9,1,0,0,0,3,6,0,0,7,1,0,0,0,0,2,2,3,0,7,7,0,3,0,1,0,1,1,4,1,3,0,0,0,0,1,0,6,0,0,3,
	2,1,11,1,0,0,1,0,2,1,0,1,0,2,4,2,1,0,0,1,1,3,0,0,0,0,4,0,1,0,0,2,8,0,6,2,0,3,0,1,0,0,0,1,0,6,0,1,0,2,1,1,2,0,1,108,1,1,6,1,0,0,1,12,
	2,0,0,0,4,3,2,5,3,3,2,1,2,0,14,2,0,4,1,9,0,7,2,0,0,0,0,0,2,7,2,2,4,2,1,10,1,3,1,10,14,1,3,2,1,0,2,1,0,5,0,1,4,9,3,14,6,1,2,1,3,0,0,2,
	12,15,0,2,86,2,0,2,0,1,1,5,0,2,6,0,1,1,5,0,0,5,0,1,0,0,1,0,7,2,0,0,0,2,0,4,7,0,0,0,0,1,1,4,1,0,0,0,1,4,9,4,1,6,1,8,5,4,1,10,0,10,2,9,
	0,0,2,11,3,3,12,1,0,0,2,0,2,1,5,3,0,21,7,1,4,
};

static ImWchar baseUnicodeRanges[] =
{
	0x0020, 0x00FF, // Basic Latin + Latin Supplement
	0x3000, 0x30FF, // Punctuations, Hiragana, Katakana
	0x31F0, 0x31FF, // Katakana Phonetic Extensions
	0xFF00, 0xFFEF, // Half-width characters
	0x2191, 0x2191, // Upwards arrow
	0x2193, 0x2193, // Downwards arrow
};
#pragma endregion

    locale_t __glocale_current = LOCALE_EN_US;
    bool __glocale_merge = false;
    unsigned int __glocale_disabled = 0;
    ImWchar* __glocale_jp_glyphrange = nullptr;

    void LocaleSet(locale_t locale)
    {
        __glocale_current = locale;
        if (!__glocale_merge) {
            ImGuiIO& io = ImGui::GetIO();
            io.FontDefault = io.Fonts->Fonts[__glocale_current];
        }
    }
    void LocaleAutoSet()
    {
        auto lang_id = GetUserDefaultUILanguage();
        switch (lang_id & 0x03ff) {
        case 0x4:
            __glocale_current = LOCALE_ZH_CN;
            break;
        case 0x9:
            __glocale_current = LOCALE_EN_US;
            break;
        case 0x11:
            __glocale_current = LOCALE_JA_JP;
            break;
        default:
            __glocale_current = LOCALE_EN_US;
            break;
        }
    }
    inline const char** LocaleGetCurrentGlossary();
    inline const char* LocaleGetStr(th_glossary_t name);
    void LocaleRotate()
    {
        switch (__glocale_current) {
        case LOCALE_ZH_CN:
            LocaleSet(LOCALE_EN_US);
            break;
        case LOCALE_EN_US:
            LocaleSet(LOCALE_JA_JP);
            break;
        case LOCALE_JA_JP:
            LocaleSet(LOCALE_ZH_CN);
            break;
        default:
            break;
        }
    }
    bool LocaleInitFromCfg()
    {
        int language = 0;
        if (LauncherSettingGet("language", language) && language >= 0 && language <= 2) {
            __glocale_current = (Gui::locale_t)language;
            return true;
        }
        return false;
    }

    struct font_info {
        const wchar_t* font_name;
        int font_index;
        float font_scale;
    };

    font_info zhFontsInfo[] = {
        { L"Microsoft YaHei UI Light", 1, 1.0f },
        { L"Microsoft YaHei Light", 0, 1.0f },
        { L"微软雅黑", 0, 1.0f },
        { L"Microsoft YaHei", 0, 1.0f },
        { L"黑体", 0, 0.9f },
        { L"SimHei", 0, 0.9f },
        { L"宋体", 0, 0.9f },
        { L"SimSun", 0, 0.9f },
    };
    font_info enFontsInfo[] = {
        { L"Segoe UI", 0, 1.0f },
        { L"Tahoma", 0, 0.88f },
    };
    font_info jaFontsInfo[] = {
        { L"Yu Gothic UI", 1, 1.0f },
        { L"Meiryo UI", 0, 1.0f },
        { L"MS UI Gothic", 0, 0.85f },
        { L"MS Mincho", 0, 0.85f },
    };

    int CALLBACK __glocale_font_enum_proc([[maybe_unused]] const LOGFONTW* lpelfe, [[maybe_unused]] const TEXTMETRICW* lpntme, [[maybe_unused]] DWORD FontType, LPARAM lParam)
    {
        *((int*)lParam) = 1;
        return 0;
    }
    int CALLBACK FindFirstJapaneseFontProc(const LOGFONTW* logicalFont, const NEWTEXTMETRICEXW* fontMetricEx, DWORD fontType, LPARAM lParam)
    {
        if (fontType != TRUETYPE_FONTTYPE) {
            return TRUE;
        }

        // Supports Japanese encoding
        if (logicalFont->lfCharSet != SHIFTJIS_CHARSET) {
            return TRUE;
        }

        // Variable pitch to save space
        if ((logicalFont->lfPitchAndFamily & 0x3) != VARIABLE_PITCH) {
            return TRUE;
        }

        // Reject font for vertical writing
        if (logicalFont->lfFaceName[0] == '@') {
            return TRUE;
        }

        // Check code page bitfields
        // For complete list of bitfields: https://msdn.microsoft.com/library/windows/desktop/dd317754(v=vs.85).aspx
        static std::uint8_t codePageBitfields[] = {
            0, 1, // Latin,
            17, // Japanese
        };

        for (auto bitToCheck : codePageBitfields) {
            DWORD mask = 1 << (bitToCheck & 32);
            if (!(fontMetricEx->ntmFontSig.fsCsb[bitToCheck / 32] & mask)) {
                return TRUE;
            }
        }

        // Check unicode subset bitfields
        // For complete list of bitfields: https://msdn.microsoft.com/library/windows/desktop/dd374090(v=vs.85).aspx
        static std::uint8_t unicodeSubsetBitfields[] = {
            0, 1, // Basic Latin + Latin Supplement
            48, 49, 50, // Punctuations, Hiragana, Katakana
            68, // Half-width characters
            37, // Arrows
            59, // CJK Unified Ideographs
        };

        for (auto bitToCheck : unicodeSubsetBitfields) {
            DWORD mask = 1 << (bitToCheck % 32);
            if (!(fontMetricEx->ntmFontSig.fsUsb[bitToCheck / 32] & mask)) {
                return TRUE;
            }
        }

        auto& outFontFamily = *reinterpret_cast<std::wstring*>(lParam);

        outFontFamily = logicalFont->lfFaceName;

        return FALSE;
    }
    int CALLBACK FindFirstChineseFontProc(const LOGFONTW* logicalFont, const NEWTEXTMETRICEXW* fontMetricEx, DWORD fontType, LPARAM lParam)
    {
        if (fontType != TRUETYPE_FONTTYPE) {
            return TRUE;
        }

        // Supports Japanese encoding
        if (logicalFont->lfCharSet != GB2312_CHARSET) {
            return TRUE;
        }

        // Variable pitch to save space
        if ((logicalFont->lfPitchAndFamily & 0x3) != VARIABLE_PITCH) {
            return TRUE;
        }

        // Reject font for vertical writing
        if (logicalFont->lfFaceName[0] == '@') {
            return TRUE;
        }

        // Check code page bitfields
        // For complete list of bitfields: https://msdn.microsoft.com/library/windows/desktop/dd317754(v=vs.85).aspx
        static std::uint8_t codePageBitfields[] = {
            0, 1, // Latin,
            18, // SChinese
        };

        for (auto bitToCheck : codePageBitfields) {
            DWORD mask = 1 << (bitToCheck & 32);
            if (!(fontMetricEx->ntmFontSig.fsCsb[bitToCheck / 32] & mask)) {
                return TRUE;
            }
        }

        // Check unicode subset bitfields
        // For complete list of bitfields: https://msdn.microsoft.com/library/windows/desktop/dd374090(v=vs.85).aspx
        static std::uint8_t unicodeSubsetBitfields[] = {
            0, 1, // Basic Latin + Latin Supplement
            //31,
            48, //49, 50, // Punctuations, Hiragana, Katakana
            //68, // Half-width characters
            59, // CJK Unified Ideographs
        };

        for (auto bitToCheck : unicodeSubsetBitfields) {
            DWORD mask = 1 << (bitToCheck % 32);
            if (!(fontMetricEx->ntmFontSig.fsUsb[bitToCheck / 32] & mask)) {
                return TRUE;
            }
        }

        auto& outFontFamily = *reinterpret_cast<std::wstring*>(lParam);

        outFontFamily = logicalFont->lfFaceName;

        return FALSE;
    }
    HFONT CALLBACK CheckFontZh(HDC hdc, font_info& info)
    {
        LOGFONTW font = {};
        int signal = 0;

        font.lfPitchAndFamily = 0;
        font.lfCharSet = GB2312_CHARSET;

        for (auto f : zhFontsInfo) {
            wcsncpy(font.lfFaceName, f.font_name, 32);
            EnumFontFamiliesExW(hdc, &font, __glocale_font_enum_proc, (LPARAM)&signal, 0);
            if (signal) {
                info = f;
                break;
            }
        }

        if (!signal) {
            std::wstring fontFamilyName;
            auto enumFontFamUserData = reinterpret_cast<LPARAM>(&fontFamilyName);
            EnumFontFamiliesExW(hdc, nullptr, (FONTENUMPROCW)&FindFirstChineseFontProc, enumFontFamUserData, 0);
            if (fontFamilyName.empty()) {
                return nullptr;
            }
            info.font_name = L"";
            info.font_index = 0;
            info.font_scale = 1.0f;
            return CreateFontW(0, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0,
                fontFamilyName.c_str());
        }

        return CreateFontW(
            0, 0, 0, 0, 0, 0, 0, 0,
            GB2312_CHARSET,
            0, 0, 0, 0,
            info.font_name);
    }
    HFONT CALLBACK CheckFontEn(HDC hdc, font_info& info)
    {
        LOGFONTW font = {};
        int signal = 0;

        font.lfPitchAndFamily = 0;
        font.lfCharSet = ANSI_CHARSET;

        for (auto f : enFontsInfo) {
            wcsncpy(font.lfFaceName, f.font_name, 32);
            EnumFontFamiliesExW(hdc, &font, __glocale_font_enum_proc, (LPARAM)&signal, 0);
            if (signal) {
                info = f;
                break;
            }
        }

        if (!signal) {
            info.font_name = L"";
            info.font_index = 0;
            info.font_scale = 1.0f;
            return (HFONT)GetStockObject(SYSTEM_FONT);
        }

        return CreateFontW(
            0, 0, 0, 0, 0, 0, 0, 0,
            ANSI_CHARSET,
            0, 0, 0, 0,
            info.font_name);
    }
    HFONT CALLBACK CheckFontJa(HDC hdc, font_info& info)
    {
        LOGFONTW font = {};
        int signal = 0;

        font.lfPitchAndFamily = 0;
        font.lfCharSet = SHIFTJIS_CHARSET;

        for (auto f : jaFontsInfo) {
            wcsncpy(font.lfFaceName, f.font_name, 32);
            EnumFontFamiliesExW(hdc, &font, __glocale_font_enum_proc, (LPARAM)&signal, 0);
            if (signal) {
                info = f;
                break;
            }
        }

        if (!signal) {
            std::wstring fontFamilyName;
            auto enumFontFamUserData = reinterpret_cast<LPARAM>(&fontFamilyName);
            EnumFontFamiliesExW(hdc, nullptr, (FONTENUMPROCW)&FindFirstJapaneseFontProc, enumFontFamUserData, 0);
            if (fontFamilyName.empty()) {
                return nullptr;
            }
            info.font_name = L"";
            info.font_index = 0;
            info.font_scale = 1.0f;
            return CreateFontW(0, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, 0, 0, 0, 0,
                fontFamilyName.c_str());
        }

        return CreateFontW(
            0, 0, 0, 0, 0, 0, 0, 0,
            SHIFTJIS_CHARSET,
            0, 0, 0, 0,
            info.font_name);
    }
    ImWchar* GetGlyphRange(int locale)
    {
        auto& io = ImGui::GetIO();
        ImWchar* glyphRange = nullptr;
        switch (locale) {
        case LOCALE_ZH_CN:
            glyphRange = (ImWchar*)io.Fonts->GetGlyphRangesChineseFull();
            break;
        case LOCALE_EN_US:
            glyphRange = (ImWchar*)io.Fonts->GetGlyphRangesDefault();
            break;
        case LOCALE_JA_JP: {
            if (!__glocale_jp_glyphrange) {
                __glocale_jp_glyphrange = (ImWchar*)malloc((_countof(baseUnicodeRanges) + _countof(offsetsFrom0x4E00) * 2 + 1) * sizeof(ImWchar));
                // Unpack
                int codepoint = 0x4e00;
                memcpy(__glocale_jp_glyphrange, baseUnicodeRanges, sizeof(baseUnicodeRanges));
                ImWchar* dst = __glocale_jp_glyphrange + _countof(baseUnicodeRanges);
                for (int n = 0; n < _countof(offsetsFrom0x4E00); n++, dst += 2) {
                    dst[0] = dst[1] = (ImWchar)(codepoint += (offsetsFrom0x4E00[n] + 1));
                }
                dst[0] = 0;
            }
        }
            glyphRange = (ImWchar*)__glocale_jp_glyphrange;
            break;
        default:
            break;
        }
        return glyphRange;
    }
    typedef HFONT(CALLBACK* font_checker)(HDC hdc, font_info& info);
    font_checker fontCheckers[] = {
        CheckFontZh,
        CheckFontEn,
        CheckFontJa,
    };

    static ImFont* __glocale_fonts[3] {};
    void LocaleFontWarning()
    {
        if (__glocale_disabled) {
            if (__glocale_disabled == 7) {
                MessageBoxW(nullptr, L"No font can be loaded.\nthprac will now terminate.", L"Fatal error", MB_OK | MB_ICONERROR);
                ExitProcess(ERROR_FILE_CORRUPT);
            } else {
                MessageBoxW(nullptr, L"One or more fonts failed to load, which will cause certain non-Latin characters to not render properly.\nIf you are running thprac under wine/Crossover, seek help to link the appropriate fonts or try using wine-staging.", L"Warning", MB_OK | MB_ICONWARNING);
            }
        }
    }
    bool LocaleCreateFont(float font_size)
    {
        auto& io = ImGui::GetIO();
        for (unsigned int locale = 0; locale < 3; locale++) {
            // Var Definition
            void* fontData = nullptr;
            DWORD fontDataSize = 0;

            // Create Font and Device Context
            font_info info = {};
            auto hdc = CreateCompatibleDC(nullptr);
            auto font = fontCheckers[locale](hdc, info);
            if (font == nullptr) {
                __glocale_disabled |= 1 << locale;
                return false;
            }
            SelectObject(hdc, font);

            // Aquiring Font Data
            fontDataSize = GetFontData(hdc, 0x66637474, 0, nullptr, 0);
            if (fontDataSize == GDI_ERROR) {
                fontDataSize = GetFontData(hdc, 0, 0, nullptr, 0);
                if (fontDataSize == GDI_ERROR) {
                    DeleteObject(font);
                    DeleteDC(hdc);
                    __glocale_disabled |= 1 << locale;
                    return false;
                }
                fontData = ImGui::MemAlloc(fontDataSize);
                GetFontData(hdc, 0, 0, fontData, fontDataSize);
            } else {
                fontData = ImGui::MemAlloc(fontDataSize);
                GetFontData(hdc, 0x66637474, 0, fontData, fontDataSize);
            }

            DeleteObject(font);
            DeleteDC(hdc);

            // Add Font
            ImFontConfig fontConfig;
            fontConfig.FontNo = info.font_index;
            fontConfig.RasterizerMultiply = 1.25;
            fontConfig.OversampleH = 5;
            fontConfig.OversampleV = 5;

            float fontFinalSize = font_size * info.font_scale;
            ImWchar* glyphRange = GetGlyphRange(locale);

            __glocale_fonts[locale] = io.Fonts->AddFontFromMemoryTTF(fontData, fontDataSize, fontFinalSize,
                &fontConfig, (ImWchar*)glyphRange);
        }

        ImGuiFreeType::BuildFontAtlas(io.Fonts, 0);
        LocaleSet(LocaleGet());
        LocaleFontWarning();

        return true;
    }
    bool LocalAddMergeFont(float font_size, int locale, bool merge)
    {
        auto& io = ImGui::GetIO();

        // Var Definition
        void* fontData = nullptr;
        DWORD fontDataSize = 0;

        // Create Font and Device Context
        font_info info = {};
        auto hdc = CreateCompatibleDC(nullptr);
        auto font = fontCheckers[locale](hdc, info);
        if (font == nullptr) {
            __glocale_disabled |= 1 << locale;
            return false;
        }
        SelectObject(hdc, font);

        // Aquiring Font Data
        fontDataSize = GetFontData(hdc, 0x66637474, 0, nullptr, 0);
        if (fontDataSize == GDI_ERROR) {
            fontDataSize = GetFontData(hdc, 0, 0, nullptr, 0);
            if (fontDataSize == GDI_ERROR) {
                DeleteObject(font);
                DeleteDC(hdc);
                __glocale_disabled |= 1 << locale;
                return false;
            }
            fontData = ImGui::MemAlloc(fontDataSize);
            GetFontData(hdc, 0, 0, fontData, fontDataSize);
        } else {
            fontData = ImGui::MemAlloc(fontDataSize);
            GetFontData(hdc, 0x66637474, 0, fontData, fontDataSize);
        }

        DeleteObject(font);
        DeleteDC(hdc);

        // Add Font
        ImFontConfig fontConfig;
        fontConfig.MergeMode = merge;
        fontConfig.FontNo = info.font_index;
        fontConfig.RasterizerMultiply = 1.25;

        ImWchar* glyphRange = GetGlyphRange(locale);
        float fontFinalSize = font_size * info.font_scale;

        io.Fonts->AddFontFromMemoryTTF(fontData, fontDataSize, fontFinalSize, &fontConfig, glyphRange);
        return true;
    }
    bool LocaleCreateMergeFont(locale_t locale, float font_size)
    {
        auto& io = ImGui::GetIO();
        __glocale_current = locale;

        switch (locale) {
        case LOCALE_ZH_CN:
            LocalAddMergeFont(font_size, LOCALE_ZH_CN, false);
            LocalAddMergeFont(font_size, LOCALE_JA_JP, true);
            LocalAddMergeFont(font_size, LOCALE_EN_US, true);
            break;
        case LOCALE_EN_US:
            LocalAddMergeFont(font_size, LOCALE_EN_US, false);
            LocalAddMergeFont(font_size, LOCALE_JA_JP, true);
            LocalAddMergeFont(font_size, LOCALE_ZH_CN, true);
            break;
        case LOCALE_JA_JP:
            LocalAddMergeFont(font_size, LOCALE_JA_JP, false);
            LocalAddMergeFont(font_size, LOCALE_ZH_CN, true);
            LocalAddMergeFont(font_size, LOCALE_EN_US, true);
            break;
        default:
            return false;
        }

        ImGuiFreeType::BuildFontAtlas(io.Fonts, 0);
        __glocale_merge = true;
        LocaleFontWarning();

        return true;
    }
    bool LocaleRecreateMergeFont(locale_t locale, float font_size)
    {
        auto& io = ImGui::GetIO();
        io.Fonts->Clear();
        return LocaleCreateMergeFont(locale, font_size);
    }
}
}
