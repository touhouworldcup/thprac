#include "loc_json.h"
#include "json.hpp"
#include "rapidjson/document.h"
#include <cstdint>
#include <cstdio>
//#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <locale>
#include <codecvt>
#include <unordered_map>

enum e1
{
	e1_1,
	e1_2,
	e1_3
};
enum e2
{
	e2_1,
	e2_2,
	e2_3
};

class etest
{
public:
	int operator[](e1 e)
	{
		return 1;
	}
	int operator[](e2 e)
	{
		return 2;
	}
};


#define INPUT_FILE "C:\\Users\\thc\\Documents\\git\\thprac\\thprac\\src\\thprac\\thprac_games_def.json"
#define OUTPUT_FILE "C:\\Users\\thc\\Documents\\git\\thprac\\thprac\\src\\thprac\\thprac_locale_def.h"

#define ENDL "\n"
#define SKIP_IF(statement, warning, ...) \
if (statement) \
{\
	printf_s(warning, __VA_ARGS__); \
	printf_s(ENDL); \
	continue; \
}
#define BREAK_IF(statement, warning, ...) \
if (statement) \
{\
	printf_s(warning, __VA_ARGS__); \
	printf_s(ENDL); \
	break; \
}

using rapidjson::Document;
using namespace std;

string g_current_game;
string g_current_section;

struct loc_str_t
{
	string zh_str;
	string en_str;
	string ja_str;

	loc_str_t() = default;
	loc_str_t(const char* zh, const char* en, const char* ja):
		zh_str(zh), en_str(en), ja_str(ja)
	{

	}
};
struct section_t
{
	int app_id		{ 0 };
	int bgm_id		{ 0 };
	int sec_id		{ -1 };
	int chap_id		{ -1 };
	int spell_id	{ 0 };
	int apperance[3]{ -1, -1, -1 };

	string name;
	string ref;
	loc_str_t loc_str[4];

	bool FillWith(rapidjson::Value& sec);

	section_t() = default;
	section_t(const char* sec_name, rapidjson::Value& sec):
		name(sec_name)
	{
		FillWith(sec);
	}
};
struct game_t
{
	string name;
	string nspace;
	vector<section_t> sections;
	vector<pair< string, rapidjson::Value>> groups;
	
	static map<string, loc_str_t> glossary;
	static set<wchar_t> glyph_range_zh;
	static set<wchar_t> glyph_range_en;
	static set<wchar_t> glyph_range_ja;

	game_t() = default;
};
map<string, loc_str_t> game_t::glossary;
set<wchar_t> game_t::glyph_range_zh;
set<wchar_t> game_t::glyph_range_en;
set<wchar_t> game_t::glyph_range_ja;

void AddGlyphRange(loc_str_t& str)
{
	static wstring u16str;
	static wstring_convert<codecvt_utf8<wchar_t>, wchar_t> conv;

	u16str = conv.from_bytes(str.zh_str);
	for (auto c : u16str) game_t::glyph_range_zh.insert(c);

	u16str = conv.from_bytes(str.en_str);
	for (auto c : u16str) game_t::glyph_range_en.insert(c);

	u16str = conv.from_bytes(str.ja_str);
	for (auto c : u16str) game_t::glyph_range_ja.insert(c);
}


bool ValidateGroup(rapidjson::GenericValue<rapidjson::UTF8<>>& group)
{
	const bool isArray = group.IsArray();
	if (group.IsArray())
		for (auto it = group.Begin(); it != group.End(); ++it)
			if (it->IsArray() ? !ValidateGroup(*it) : !it->IsString())
				return false;
	return true;
}

int GetGroupDepth(rapidjson::Value& value)
{
	return (value.IsArray() ? GetGroupDepth(value[0]) + 1 : 0);
}

void PrintGroupSize(FILE* output, rapidjson::Value& value)
{
	vector<rapidjson::SizeType> result;
	function<void(rapidjson::Value&, unsigned int)> getSizeLimit = 
		[&](rapidjson::Value& value, unsigned int dim)
	{
		if (value.IsArray())
		{
			if (result.size() < dim + 1) result.resize(dim + 1);
			for (auto it = value.Begin(); it != value.End(); ++it)
				getSizeLimit(*it, dim + 1);
			if (result[dim] < value.Size()) result[dim] = value.Size();
		}
	};

	getSizeLimit(value, 0);
	if (result.size() > 0) result.back()++;
	for (auto dim : result)
		fprintf_s(output, "[%d]", dim);

}
void PrintGroup(FILE* output, rapidjson::Value& value, int tab = 0)
{
	if (value.IsArray())
	{
		for (int i = tab; i > 0; --i) putc(' ', output);
		fprintf_s(output, "{" ENDL);
		for (auto v_itr = value.Begin(); v_itr != value.End(); ++v_itr)
			PrintGroup(output, *v_itr, tab + 4);
		for (int i = tab; i > 0; --i) putc(' ', output);
		fprintf_s(output, "}%c" ENDL, !tab ? ';' : ',');
	}
	else
	{
		for (int i = tab; i > 0; --i) putc(' ', output);
		if (!tab)
			fprintf_s(output, "= %s;" ENDL, value.GetString());
		else
			fprintf_s(output, "%s," ENDL, value.GetString());
	}
}

std::string GetEscapedStr(std::string& str)
{
	auto escaped_str = str;
	auto length = escaped_str.length();

	for (size_t i = 0; i < length; ++i)
	{
		if (escaped_str[i] == '\"')
		{
			escaped_str.insert(i, "\\");
			i++;
			length++;
        } 
		else if (escaped_str[i] == '\\') 
		{
            if (i + 1 != length && escaped_str[i + 1] != '0') {
                escaped_str.insert(i, "\\");
                i++;
                length++;
			}
		}
		else if (escaped_str[i] == '\n')
		{

			escaped_str.insert(i, "\\");
			i++;
			length++;
			escaped_str[i] = 'n';
		}
	}
	return escaped_str;
}

void loc_json()
{

	// TODO: Get input;
	FILE* input;
	fopen_s(&input, INPUT_FILE, "rb");
	fseek(input, 0, SEEK_END);
	auto input_size = ftell(input);
	char* input_buf = (char*)malloc(input_size + 1);
	fseek(input, 0, SEEK_SET);
	fread(input_buf, 1, input_size, input);
	input_buf[input_size] = '\0';
	input_buf += 3;


	Document doc;
	if (doc.Parse(input_buf).HasParseError())
	{
		
		printf_s("Error: Parse error: %d at %d." ENDL, doc.GetParseError(), doc.GetErrorOffset());
		getchar();
		return;
	}
	// Iterate through games
	vector<game_t> games;
	for (auto game_itr = doc.MemberBegin(); game_itr != doc.MemberEnd(); ++game_itr)
	{
		games.emplace_back();
		game_t& game_obj = games.back();
		game_obj.name = game_itr->name.GetString();
		g_current_game = game_itr->name.GetString();

		auto& game = game_itr->value;
		SKIP_IF(!game.IsObject(), "Warning: A non-object value for a game has detected, ignoring.");

		// Check namespace
		if (game.HasMember("namespace"))
		{
			if (game["namespace"].IsString())
			{
				game_obj.nspace = game["namespace"].GetString();
			}
			else
			{
				printf_s("Warning: In game \"%s\": Invalid namespace value, ignoring." ENDL, g_current_game.c_str());
			}
		}

		// Parsing Glossary
		if (game.HasMember("glossary"))
		{
			auto& glossary = game["glossary"];

			if (glossary.IsObject())
			{
				for (auto item_itr = glossary.MemberBegin(); item_itr != glossary.MemberEnd(); ++item_itr)
				{
					auto& item = item_itr->value;
					SKIP_IF(!item.IsArray() ||
						item.Size() != 3 ||
						!item[0].IsString() ||
						!item[1].IsString() ||
						!item[2].IsString(),
						"Warning: In game \"%s\": Invalid glossary item: \"%s\", ignoring.",
						g_current_game.c_str(), item_itr->name.GetString());

					auto glossary_key = item_itr->name.GetString();
					game_obj.glossary[glossary_key] =
					{ item[0].GetString(), item[1].GetString(), item[2].GetString() };
					AddGlyphRange(game_obj.glossary[glossary_key]);
				}
			}
			else
			{
				printf_s("Warning: In game \"%s\": Invalid glossary value, ignoring." ENDL, g_current_game.c_str());
			}
		}

		// Parsing sections
		if (game.HasMember("sections"))
		{
			auto& sections = game["sections"];

			if (sections.IsObject())
			{
				// Iterate through sections
				for (auto section_itr = sections.MemberBegin(); section_itr != sections.MemberEnd(); ++section_itr)
				{
					SKIP_IF(!section_itr->value.IsObject(), "Warning: In game \"%s\": Incorrect section: %s",
						g_current_game.c_str(), section_itr->name.GetString());
					game_obj.sections.emplace_back(section_itr->name.GetString(), section_itr->value);
				}
			}
			else
			{
				printf_s("Warning: In game \"%s\": Invalid sections value, ignoring." ENDL, g_current_game.c_str());
			}
		}

		// Parsing groups
		if (game.HasMember("groups"))
		{
			auto& groups = game["groups"];

			if(groups.IsObject())
			{
				// Iterate through sections
				for (auto group_itr = groups.MemberBegin(); group_itr != groups.MemberEnd(); ++group_itr)
				{
					// Validate group
					if (ValidateGroup(group_itr->value))
					{
						game_obj.groups.emplace_back();
						game_obj.groups.back().first = group_itr->name.GetString();
						game_obj.groups.back().second = group_itr->value;
					}
					else
					{
						SKIP_IF(true, "Warning: In game \"%s\": Incorrect group: %s",
							g_current_game.c_str(), group_itr->name.GetString());
					}
				}
			}
		else
		{
			printf_s("Warning: In game \"%s\": Invalid groups value, ignoring." ENDL, g_current_game.c_str());
		}
		}
		
	}


	/******************************************************************************************/
	// Output Init
	/******************************************************************************************/
	FILE* output;
	fopen_s(&output, OUTPUT_FILE, "w");
	putc(0xef, output);
	putc(0xbb, output);
	putc(0xbf, output);
	//fprintf_s(output, "\xef\xbb\xbf");


	/******************************************************************************************/
	// Header
	/******************************************************************************************/
	fprintf_s(output, "#pragma once" ENDL);
	fprintf_s(output, "#include <cstdint>" ENDL ENDL);
	fprintf_s(output, "namespace THPrac {" ENDL ENDL);


	/******************************************************************************************/
	// Glossary
	/******************************************************************************************/
	if (game_t::glossary.size() < 256)
		fprintf_s(output, "enum th_glossary_t : uint8_t" ENDL "{" ENDL "    A0000ERROR_C," ENDL);
	else
		fprintf_s(output, "enum th_glossary_t" ENDL "{" ENDL "    A0000ERROR_C," ENDL);
	for (auto& item : game_t::glossary)
		fprintf_s(output, "    %s," ENDL, item.first.c_str());
	fprintf_s(output, "};" ENDL ENDL);
	fprintf_s(output, "static char* th_glossary_str[3][%d]" ENDL "{" ENDL, game_t::glossary.size() + 1);
	fprintf_s(output, "    {" ENDL "        \"\"," ENDL);
	for (auto& item : game_t::glossary)
		fprintf_s(output, "        u8\"%s\"," ENDL, GetEscapedStr(item.second.zh_str).c_str());
	fprintf_s(output, "    }," ENDL);
	fprintf_s(output, "    {" ENDL "        \"\"," ENDL);
	for (auto& item : game_t::glossary)
		fprintf_s(output, "        u8\"%s\"," ENDL, GetEscapedStr(item.second.en_str).c_str());
	fprintf_s(output, "    }," ENDL);
	fprintf_s(output, "    {" ENDL "        \"\"," ENDL);
	for (auto& item : game_t::glossary)
		fprintf_s(output, "        u8\"%s\"," ENDL, GetEscapedStr(item.second.ja_str).c_str());
	fprintf_s(output, "    }," ENDL);
	fprintf_s(output, "};" ENDL ENDL);


	for (auto& game : games)
	{
		if (game.nspace != "")
		{
			/******************************************************************************************/
			// Namespace start
			/******************************************************************************************/
			fprintf_s(output, "namespace %s {" ENDL ENDL, game.nspace.c_str());


			/******************************************************************************************/
			// Sections enum
			/******************************************************************************************/
			if (game.sections.size() < 256)
				fprintf_s(output, "enum th_sections_t : uint8_t" ENDL "{" ENDL "    A0000ERROR," ENDL);
			else
				fprintf_s(output, "enum th_sections_t" ENDL "{" ENDL "    A0000ERROR," ENDL);
			for (auto& item : game.sections)
				fprintf_s(output, "    %s," ENDL, item.name.c_str());
			fprintf_s(output, "};" ENDL ENDL);


			/******************************************************************************************/
			// Sections str array
			/******************************************************************************************/
			fprintf_s(output, "static char* th_sections_str[3][4][%d]" ENDL "{" ENDL, game.sections.size() + 1);
			fprintf_s(output, "    {" ENDL); // ZH
			for (size_t i = 0; i < 4; ++i)
			{
				fprintf_s(output, "        {" ENDL "            u8\"\"," ENDL);
				for (auto& item : game.sections)
					fprintf(output, "            u8\"%s\"," ENDL, GetEscapedStr(item.loc_str[i].zh_str).c_str());
				fprintf_s(output, "        }," ENDL);
			}
			fprintf_s(output, "    }," ENDL);
			fprintf_s(output, "    {" ENDL); // EN
			for (size_t i = 0; i < 4; ++i)
			{
				fprintf_s(output, "        {" ENDL "            u8\"\"," ENDL);
				for (auto& item : game.sections)
					fprintf(output, "            u8\"%s\"," ENDL, GetEscapedStr(item.loc_str[i].en_str).c_str());
				fprintf_s(output, "        }," ENDL);
			}
			fprintf_s(output, "    }," ENDL);
			fprintf_s(output, "    {" ENDL); // JA
			for (size_t i = 0; i < 4; ++i)
			{
				fprintf_s(output, "        {" ENDL "            u8\"\"," ENDL);
				for (auto& item : game.sections)
					fprintf(output, "            u8\"%s\"," ENDL, GetEscapedStr(item.loc_str[i].ja_str).c_str());
				fprintf_s(output, "        }," ENDL);
			}
			fprintf_s(output, "    }," ENDL);
			fprintf_s(output, "};" ENDL ENDL);


			/******************************************************************************************/
			// Sections BGM
			/******************************************************************************************/
			fprintf_s(output, "static uint8_t th_sections_bgm[]" ENDL "{" ENDL "    0," ENDL);
			for (auto& item : game.sections)
				fprintf_s(output, "    %d," ENDL, item.bgm_id);
			fprintf_s(output, "};" ENDL ENDL);


			/******************************************************************************************/
			// Sections catagory: By appearance
			/******************************************************************************************/
			int dim0 = 1, dim1 = 1, dim2 = 1;
			for (auto& item : game.sections)
			{
				if (item.apperance[0] > dim0) dim0 = item.apperance[0];
				if (item.apperance[1] > dim1) dim1 = item.apperance[1];
				if (item.apperance[2] > dim2) dim2 = item.apperance[2];
			}
			vector<vector<vector<string>>> cba;
			cba.resize(dim0);
			for (auto& item1 : cba)
			{
				item1.resize(dim1);
				for (auto& item2 : item1)
					item2.resize(dim2);
			}
			for (auto& item : game.sections)
				cba[item.apperance[0] - 1][item.apperance[1] - 1][item.apperance[2] - 1] = item.name;
			fprintf_s(output, "static th_sections_t th_sections_cba[%d][%d][%d]" ENDL "{" ENDL, dim0, dim1, dim2 + 1);
			for (auto& i1 : cba)
			{
				fprintf_s(output, "    {" ENDL);
				for (auto& i2 : i1)
				{
					fprintf_s(output, "        { ");
					for (auto& i3 : i2)
					{
						if (i3 == "") break;//fprintf_s(output, "A0000ERROR, ");
						else
							fprintf_s(output, "%s, ", i3.c_str());
					}
					fprintf_s(output, "}," ENDL);
				}
				fprintf_s(output, "    }," ENDL);
			}
			fprintf_s(output, "};" ENDL ENDL);


			/******************************************************************************************/
			// Sections catagory: By type
			/******************************************************************************************/
			vector<vector<vector<string>>> cbt;
			size_t cbt_dim2 = 0;
			cbt.resize(dim0);
			for (auto& i1 : cbt)
				i1.resize(2);
			for (auto& item : game.sections)
			{
				if (!item.spell_id)
				{
					cbt[item.apperance[0] - 1][0].emplace_back(item.name);
					auto sss = cbt[item.apperance[0] - 1][0].size();
					cbt_dim2 = sss > cbt_dim2 ? sss : cbt_dim2;
				}
				else
				{
					cbt[item.apperance[0] - 1][1].emplace_back(item.name);
					auto sss = cbt[item.apperance[0] - 1][1].size();
					cbt_dim2 = sss > cbt_dim2 ? sss : cbt_dim2;
				}
			}
			fprintf_s(output, "static th_sections_t th_sections_cbt[%d][%d][%d]" ENDL "{" ENDL, dim0, 2, cbt_dim2 + 1);
			for (auto& i1 : cbt)
			{
				fprintf_s(output, "    {" ENDL);
				for (auto& i2 : i1)
				{
					fprintf_s(output, "        { ");
					for (auto& i3 : i2)
					{
						if (i3 == "")
							fprintf_s(output, "A0000ERROR, ");
						else
							fprintf_s(output, "%s, ", i3.c_str());
					}
					fprintf_s(output, "}," ENDL);
				}
				fprintf_s(output, "    }," ENDL);
			}
			fprintf_s(output, "};" ENDL ENDL);
		}


		/******************************************************************************************/
		// Groups
		/******************************************************************************************/
		for (auto& group : game.groups)
		{
			fprintf_s(output, "static th_glossary_t %s", group.first.c_str());
			PrintGroupSize(output, group.second);
			fprintf_s(output, ENDL);
			PrintGroup(output, group.second);
			fprintf_s(output, ENDL);
			true;
		}


		/******************************************************************************************/
		// Namespace end
		/******************************************************************************************/
		if (game.nspace != "") fprintf_s(output, "}" ENDL ENDL);

	}

	/******************************************************************************************/
	// Glyph Range
	/******************************************************************************************/
	fprintf_s(output, "static wchar_t __thprac_loc_range_zh[] {" ENDL);		// zh_CN
	fprintf_s(output, "    " "0x0020, 0x00FF," ENDL);
	for (auto code : game_t::glyph_range_zh)
	{
		if (code <= 0xff) continue;
		fprintf_s(output, "    " "%#x, %#x," ENDL, code, code);
	}
	fprintf_s(output, "    0" ENDL "};" ENDL ENDL);

	fprintf_s(output, "static wchar_t __thprac_loc_range_en[] {" ENDL);		// en_US
	fprintf_s(output, "    " "0x0020, 0x00FF," ENDL);
	for (auto code : game_t::glyph_range_en)
	{
		if (code <= 0xff) continue;
		fprintf_s(output, "    " "%#x, %#x," ENDL, code, code);
	}
	fprintf_s(output, "    0" ENDL "};" ENDL ENDL);

	fprintf_s(output, "static wchar_t __thprac_loc_range_ja[] {" ENDL);
	fprintf_s(output, "    " "0x0020, 0x00FF," ENDL);						// ja_JP
	for (auto code : game_t::glyph_range_ja)
	{
		if (code <= 0xff) continue;
		fprintf_s(output, "    " "%#x, %#x," ENDL, code, code);
	}
	fprintf_s(output, "    0" ENDL "};" ENDL ENDL);
	/*
	fprintf_s(output, "static wchar_t* __thprac_loc_range[] {" ENDL
		"    __thprac_loc_range_zh," ENDL
		"    __thprac_loc_range_en," ENDL
		"    __thprac_loc_range_ja," ENDL"};" ENDL ENDL);
	*/



	fprintf_s(output, "}" ENDL ENDL);
	fclose(output);


	printf_s("Complete" ENDL);
	getchar();
	return;
}



bool section_t::FillWith(rapidjson::Value& sec)
{

	enum sec_switch
	{
		SW_BGM,
		SW_APPEARANCE,
		SW_SPELL,
		SW_REF,
	};
	static unordered_map<string, sec_switch> sec_switch_map
	{
		{"bgm", SW_BGM},
		{"appearance", SW_APPEARANCE},
		{"spell", SW_SPELL},
		{"ref", SW_REF},
	};

	for (auto sw_itr = sec.MemberBegin(); sw_itr != sec.MemberEnd(); ++sw_itr)
	{
		auto sw_key = sw_itr->name.GetString();
		auto& sw_value = sw_itr->value;

		if (sw_key[0] == '!')
		{
			loc_str_t lstr;
			if (sw_value.IsArray() && sw_value.Size() == 3 &&
				sw_value[0].IsString() && sw_value[1].IsString() && sw_value[2].IsString())
			{
				lstr = { sw_value[0].GetString(), sw_value[1].GetString(), sw_value[2].GetString() };
				AddGlyphRange(lstr);
			}
			else if (sw_value.IsString())
			{
				auto it = game_t::glossary.find(sw_value.GetString());
				SKIP_IF(it == game_t::glossary.end(), "Warning: Reference not found: %s, ignoring.", sw_value.GetString());
				lstr = it->second;
			}
			else
			{
				SKIP_IF(true, "Warning: Incorrect rank switch value: %s, ignoring.", sw_key);
			}

			for (size_t i = 1; i < strlen(sw_key); ++i)
			{
				auto error = false;
				switch (sw_key[i])
				{
				case 'E':
					loc_str[0] = lstr;
					break;
				case 'N':
					loc_str[1] = lstr;
					break;
				case 'H':
					loc_str[2] = lstr;
					break;
				case 'L':
					loc_str[3] = lstr;
					break;
				case 'X':
					loc_str[0] = loc_str[1] = loc_str[2] = loc_str[3] = lstr;
					break;
				default:
					error = true;
					break;
				}
				BREAK_IF(error, "Warning: Incorrect rank switch: %s, ignoring.", sw_key);
			}
		}
		else
		{
			switch (sec_switch_map[sw_key])
			{
			case SW_BGM:
				BREAK_IF(!sw_value.IsInt(), "Warning: Incorrect property switch: %s, ignoring.", sw_key);
				bgm_id = sw_value.GetInt();
				break;
			case SW_APPEARANCE:
				BREAK_IF(!sw_value.IsArray() ||
					sw_value.Size() != 3 ||
					!sw_value[0].IsInt() ||
					!sw_value[1].IsInt() ||
					!sw_value[2].IsInt(),
					"Warning: Incorrect property switch: %s, ignoring.", sw_key);
				apperance[0] = sw_value[0].GetInt();
				apperance[1] = sw_value[1].GetInt();
				apperance[2] = sw_value[2].GetInt();
				break;
			case SW_SPELL:
				BREAK_IF(!sw_value.IsInt(), "Warning: Incorrect property switch: %s, ignoring.", sw_key);
				spell_id = sw_value.GetInt();
				break;
			case SW_REF:
				BREAK_IF(!sw_value.IsString(), "Warning: Incorrect property switch: %s, ignoring.", sw_key);
				ref = sw_value.GetString();
			default:
				BREAK_IF(true, "Warning: Incorrect property switch: %s, ignoring.", sw_key);
				break;
			}
		}
	}

	return true;
}