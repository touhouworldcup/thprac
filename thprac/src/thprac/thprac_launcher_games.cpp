#define NOMINMAX

#include "thprac_launcher_games.h"
#include "thprac_gui_locale.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_games_def.h"
#include "thprac_launcher_main.h"
#include "thprac_launcher_utils.h"
#include "thprac_launcher_wnd.h"
#include "thprac_load_exe.h"
#include "thprac_main.h"
#include <Windows.h>
#include <shlwapi.h>
#include <cstdint>
#include <cwctype>
#include <functional>
#include <imgui.h>
#include <metrohash128.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <tsl/robin_map.h>
#include <vector>

namespace THPrac {
inline bool HashCompare(uint32_t hash1[4], uint32_t hash2[4])
{
    for (int i = 0; i < 4; ++i) {
        if (hash1[i] != hash2[i]) {
            return false;
        }
    }
    return true;
}
inline bool OepCompare(uint32_t oepCode1[10], uint32_t opeCode2[10])
{
    for (int i = 0; i < 10; ++i) {
        if (oepCode1[i] != opeCode2[i]) {
            return false;
        }
    }
    return true;
}
void CalcMetroHash(void* buffer, size_t size, uint64_t result[2])
{
    if (!buffer)
        return;

    MetroHash128 metro;
    result[0] = 0;
    result[1] = 0;
    metro.Hash((uint8_t*)buffer, size, (uint8_t*)result);
}
bool ReadMemory(void* buffer, void* addr, size_t size)
{
    SIZE_T bytesRead = 0;
    ReadProcessMemory(GetCurrentProcess(), addr, buffer, size, &bytesRead);
    return bytesRead == size;
}
bool GetExeInfo(void* exeBuffer, size_t exeSize, ExeSig& exeSigOut)
{
    if (exeSize < 128)
        return false;

    IMAGE_DOS_HEADER dosHeader;
    if (!ReadMemory(&dosHeader, exeBuffer, sizeof(IMAGE_DOS_HEADER)) || dosHeader.e_magic != 0x5a4d)
        return false;
    IMAGE_NT_HEADERS ntHeader;
    if (!ReadMemory(&ntHeader, (void*)((DWORD)exeBuffer + dosHeader.e_lfanew), sizeof(IMAGE_NT_HEADERS)) || ntHeader.Signature != 0x00004550)
        return false;

    exeSigOut.timeStamp = ntHeader.FileHeader.TimeDateStamp;
    exeSigOut.textSize = 0;
    for (auto& codeBlock : exeSigOut.oepCode) {
        codeBlock = 0;
    }
    for (auto& hashBlock : exeSigOut.metroHash) {
        hashBlock = 0;
    }

    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)((ULONG_PTR)((LONG)exeBuffer + dosHeader.e_lfanew) + ((LONG)(LONG_PTR) & (((IMAGE_NT_HEADERS*)0)->OptionalHeader)) + ntHeader.FileHeader.SizeOfOptionalHeader);
    for (int i = 0; i < ntHeader.FileHeader.NumberOfSections; i++, pSection++) {
        IMAGE_SECTION_HEADER section;
        if (!ReadMemory(&section, pSection, sizeof(IMAGE_SECTION_HEADER)))
            continue;
        if (!strcmp(".text", (char*)section.Name)) {
            exeSigOut.textSize = section.SizeOfRawData;
        }
        DWORD pOepCode = ntHeader.OptionalHeader.AddressOfEntryPoint;
        if (pOepCode >= section.VirtualAddress && pOepCode <= (section.VirtualAddress + section.Misc.VirtualSize)) {
            pOepCode -= section.VirtualAddress;
            pOepCode += section.PointerToRawData;
            pOepCode += (DWORD)exeBuffer;

            uint16_t oepCode[10];
            if (!ReadMemory(&oepCode, (void*)pOepCode, sizeof(oepCode)))
                continue;

            for (unsigned int i = 0; i < 10; ++i) {
                exeSigOut.oepCode[i] = (uint32_t) * (oepCode + i);
                exeSigOut.oepCode[i] ^= (i + 0x41) | ((i + 0x41) << 8);
            }
        }
    }

    MetroHash128 metro;
    if (exeSize < (1 << 23)) {
        metro.Hash((uint8_t*)exeBuffer, exeSize, (uint8_t*)exeSigOut.metroHash);
    }

    return true;
}
bool GetExeInfoEx(size_t process, ExeSig& exeSigOut)
{
    HANDLE hProc = (HANDLE)process;
    DWORD bytesRead;

    IMAGE_DOS_HEADER dosHeader;
    if (!ReadProcessMemory(hProc, (void*)0x400000, &dosHeader, sizeof(IMAGE_DOS_HEADER), &bytesRead)) {
        return false;
    }
    IMAGE_NT_HEADERS ntHeader;
    if (!ReadProcessMemory(hProc, (void*)(0x400000 + dosHeader.e_lfanew), &ntHeader, sizeof(IMAGE_NT_HEADERS), &bytesRead)) {
        return false;
    }

    exeSigOut.timeStamp = ntHeader.FileHeader.TimeDateStamp;
    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)((ULONG_PTR)(0x400000 + dosHeader.e_lfanew) + ((LONG)(LONG_PTR) & (((IMAGE_NT_HEADERS*)0)->OptionalHeader)) + ntHeader.FileHeader.SizeOfOptionalHeader);
    for (int i = 0; i < ntHeader.FileHeader.NumberOfSections; i++, pSection++) {
        IMAGE_SECTION_HEADER section;
        if (!ReadProcessMemory(hProc, (void*)(pSection), &section, sizeof(IMAGE_SECTION_HEADER), &bytesRead)) {
            return false;
        }
        if (!strcmp(".text", (char*)section.Name)) {
            exeSigOut.textSize = section.SizeOfRawData;
            return true;
        }
    }

    return false;
}
bool IfEndWith(std::string str, const char* subStr)
{
    auto subStrLen = strlen(subStr);

    auto it = --(str.end());
    for (unsigned int i = 0; i < subStrLen; ++i) {
        *it = tolower(*it);
        if (it == str.begin()) {
            break;
        }
        --it;
    }

    auto pos = str.rfind(subStr);
    if (pos != std::string::npos && pos == str.size() - subStrLen) {
        return true;
    }
    return false;
}
bool LoadJsonFile(std::wstring& path, void*& buffer, size_t& size)
{
    DWORD openFlag = OPEN_EXISTING;
    DWORD openAccess = GENERIC_READ;
    auto hFile = CreateFileW(path.c_str(), openAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, openFlag, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesProcessed;
    auto fileSize = GetFileSize(hFile, NULL);
    auto fileBuffer = malloc(fileSize + 1);
    memset(fileBuffer, 0, fileSize + 1);
    if (!ReadFile(hFile, fileBuffer, fileSize, &bytesProcessed, NULL)) {
        CloseHandle(hFile);
        free(fileBuffer);
        return false;
    }

    CloseHandle(hFile);
    buffer = fileBuffer;
    size = fileSize;
    return true;
}
bool CheckIsValidJson(std::wstring& jsonPath)
{
    void* fileBuffer;
    size_t fileSize;

    if (LoadJsonFile(jsonPath, fileBuffer, fileSize)) {
        rapidjson::Document json;
        if (!(json.Parse((const char*)fileBuffer, fileSize + 1).HasParseError()) && json.IsObject()) {
            free(fileBuffer);
            return true;
        }
        free(fileBuffer);
    }

    return false;
}
int CheckHasSteamDRM(void* exeBuffer, size_t exeSize)
{
    if (exeSize < 128)
        return 0;

    IMAGE_DOS_HEADER dosHeader;
    if (!ReadMemory(&dosHeader, exeBuffer, sizeof(IMAGE_DOS_HEADER)) || dosHeader.e_magic != 0x5a4d)
        return 0;
    IMAGE_NT_HEADERS ntHeader;
    if (!ReadMemory(&ntHeader, (void*)((DWORD)exeBuffer + dosHeader.e_lfanew), sizeof(IMAGE_NT_HEADERS)) || ntHeader.Signature != 0x00004550)
        return 0;

    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)((ULONG_PTR)((LONG)exeBuffer + dosHeader.e_lfanew) + ((LONG)(LONG_PTR) & (((IMAGE_NT_HEADERS*)0)->OptionalHeader)) + ntHeader.FileHeader.SizeOfOptionalHeader);
    for (int i = 0; i < ntHeader.FileHeader.NumberOfSections; i++, pSection++) {
        IMAGE_SECTION_HEADER section;
        if (!ReadMemory(&section, pSection, sizeof(IMAGE_SECTION_HEADER)))
            continue;
        if (!strcmp(".bind", (char*)section.Name)) {
            static std::string_view ver2_0Sig("\x53\x51\x52\x56\x57\x55\x8B\xEC\x81\xEC\x00\x10\x00\x00\xBE", 15);
            static std::string_view ver2_1Sig("\x53\x51\x52\x56\x57\x55\x8B\xEC\x81\xEC\x00\x10\x00\x00\xC7", 15);
            static std::string_view ver3Sig("\xE8\x00\x00\x00\x00\x50\x53\x51\x52\x56\x57\x55\x8B\x44\x24\x1C\x2D\x05\x00\x00\x00\x8B\xCC\x83\xE4\xF0\x51\x51\x51\x50", 30);
            static std::string_view ver3PatternHead("\x55\x8B\xEC\x81\xEC", 5);
            static std::string_view ver3Sub1_1("\x53", 1);
            static std::string_view ver3Sub1_2("\x68", 1);
            static std::string_view ver3Sub2_1("\x53", 1);
            static std::string_view ver3Sub2_2("\x8D\x83", 2);
            static std::string_view ver3Sub3_1("\x56", 1);
            static std::string_view ver3Sub3_2("\x8D", 1);
            std::string_view exeStr((char*)exeBuffer, exeSize);

            //auto sectionOffset = (DWORD)exeBuffer + section.PointerToRawData;
            if (exeStr.find(ver2_0Sig, section.PointerToRawData) != std::string_view::npos) {
                return 2;
            } else if (exeStr.find(ver2_0Sig, section.PointerToRawData) != std::string_view::npos) {
                return 2;
            } else if (exeStr.find(ver3Sig, section.PointerToRawData) != std::string_view::npos) {
                auto pos = exeStr.find(ver3PatternHead, section.PointerToRawData);
                uint32_t headerSize = 0;
                if (pos != std::string_view::npos) {
                    if (exeStr.find(ver3Sub1_1, pos) == pos + 9 && exeStr.find(ver3Sub1_2, pos) == pos + 15) {
                        headerSize = *(uint32_t*)((uint32_t)exeBuffer + pos + 0x10);
                    } else if (exeStr.find(ver3Sub2_1, pos) == pos + 9 && exeStr.find(ver3Sub2_2, pos) == pos + 15) {
                        headerSize = *(uint32_t*)((uint32_t)exeBuffer + pos + 0x16);
                    } else if (exeStr.find(ver3Sub3_1, pos) == pos + 9 && exeStr.find(ver3Sub3_2, pos) == pos + 20) {
                        headerSize = *(uint32_t*)((uint32_t)exeBuffer + pos + 0x10);
                    }
                }
                if (headerSize == 0xb0 || headerSize == 0xd0 || headerSize == 0xf0) {
                    return 3;
                }
            }
        }
    }

    return 0;
}

struct THGameInst {
    std::string name;
    std::string path;
    THGameType type;

    bool useVpatch = true;
    bool useTHPrac = false;

    THGameInst() = default;
    THGameInst(const char* _name, std::string& _path, THGameType _type)
    {
        name = _name;
        path = _path;
        type = _type;
    }
};
struct THGameScan {
    THGameInst game;
    THGameSig* signature;
    bool checked;

    THGameScan(const char* _name, const char* _path, THGameType _type, THGameSig* _signature, bool _checked = true)
    {
        game.name = _name;
        game.path = _path;
        game.type = _type;
        signature = _signature;
        checked = _checked;
    }
};
struct THGame {
    THGameSig signature;
    std::vector<THGameInst> instances;
    int selected = 0;
    int defaultLaunch = -1;
};

class THGameGui {
#define GAME_SCAN_TYPE_CNT (6)
    enum ScanOption {
        SCAN_OPT_ORIGINAL = 0,
        SCAN_OPT_MODDED = 1,
        SCAN_OPT_THCRAP = 2, // OBSOLETED
        SCAN_OPT_STEAM = 3
    };

private:
    bool CheckIfValidCatagory(const char* str)
    {
        for (auto& gameDef : gGameDefs) {
            if (!strcmp(str, gameDef.idStr)) {
                return true;
            }
        }
        return false;
    }
    bool LoadGameCfg()
    {
        bool result = true;
        int applyThpracDefault = 0;
        LauncherSettingGet("apply_thprac_default", applyThpracDefault);

        auto& cfg = LauncherCfgGet();
        if (cfg.HasMember("games") && cfg["games"].IsObject()) {
            auto& gameJson = cfg["games"];
            for (auto it = gameJson.MemberBegin(); it != gameJson.MemberEnd(); ++it) {
                if (CheckIfValidCatagory(it->name.GetString()) && it->value.IsObject()) {
                    if (it->value.HasMember("instances") && it->value["instances"].IsArray()) {
                        auto& inst = it->value["instances"];
                        for (auto gameIt = inst.Begin(); gameIt != inst.End(); ++gameIt) {
                            auto& game = *gameIt;
                            if (game.IsObject()) {
                                if (game.HasMember("path") && game["path"].IsString()) {
                                    THGameInst gameInst;
                                    gameInst.path = game["path"].GetString();
                                    if (game.HasMember("type") && game["type"].IsInt()) {
                                        gameInst.type = (THGameType)game["type"].GetInt();
                                    } else {
                                        gameInst.type = TYPE_UNCERTAIN;
                                    }
                                    if (game.HasMember("name") && game["name"].IsString()) {
                                        gameInst.name = game["name"].GetString();
                                    }

                                    if (gameInst.type == TYPE_THCRAP && IfEndWith(gameInst.path, ".lnk")) {
                                        continue;
                                    }
                                    if (gameInst.type == TYPE_STEAM) {
                                        gameInst.path = GetCleanedPath(gameInst.path);
                                    }

                                    switch (applyThpracDefault) {
                                    case 1:
                                        gameInst.useTHPrac = IsTHPracApplicable(gameInst.type);
                                        if (!gameInst.useTHPrac && game.HasMember("apply_thprac") && game["apply_thprac"].IsBool()) {
                                            gameInst.useTHPrac = game["apply_thprac"].GetBool();
                                        }
                                        break;
                                    case 2:
                                        gameInst.useTHPrac = false;
                                        break;
                                    default:
                                        if (game.HasMember("apply_thprac") && game["apply_thprac"].IsBool()) {
                                            gameInst.useTHPrac = game["apply_thprac"].GetBool();
                                        }
                                        break;
                                    }

                                    mGames[it->name.GetString()].instances.push_back(gameInst);
                                } else {
                                    result = false;
                                }
                            } else {
                                game.SetObject();
                                result = false;
                            }
                        }
                        if (it->value.HasMember("default_launch") && it->value["default_launch"].IsInt()) {
                            mGames[it->name.GetString()].defaultLaunch = it->value["default_launch"].GetInt();
                        }
                    } else {
                        if (it->value.HasMember("instances")) {
                            result = false;
                            it->value["instances"].SetObject();
                        }
                    }
                } else {
                    result = false;
                    it->value.SetObject();
                }
            }
        } else {
            if (cfg.HasMember("games")) {
                result = false;
                cfg["games"].SetObject();
            }
        }

        WriteGameCfg();
        return result;
    }
    void WriteGameCfg()
    {
        rapidjson::Document gameJson;
        auto& alloc = gameJson.GetAllocator();
        gameJson.SetObject();

        for (auto it = mGames.begin(); it != mGames.end(); ++it) {
            auto& game = it.value();
            rapidjson::Value gameTitleJson;
            gameTitleJson.SetObject();
            JsonAddMember(gameTitleJson, "default_launch", game.defaultLaunch, alloc);

            rapidjson::Value gameInstances;
            gameInstances.SetArray();
            for (auto& inst : game.instances) {
                rapidjson::Value gameInst;
                gameInst.SetObject();
                //gameInst.AddMember("name", inst.name.c_str(), alloc);
                JsonAddMemberA(gameInst, "name", inst.name.c_str(), alloc);
                JsonAddMember(gameInst, "type", (int)inst.type, alloc);
                JsonAddMemberA(gameInst, "path", inst.path.c_str(), alloc);
                JsonAddMember(gameInst, "apply_thprac", inst.useTHPrac, alloc);
                gameInstances.PushBack(gameInst, alloc);
            }
            JsonAddMemberA(gameTitleJson, "instances", gameInstances, alloc);

            JsonAddMemberA(gameJson, game.signature.idStr, gameTitleJson, alloc);
        }

        auto& cfg = LauncherCfgGet();
        if (cfg.HasMember("games")) {
            cfg.RemoveMember("games");
        }
        JsonAddMemberA(cfg, "games", gameJson, cfg.GetAllocator());
        LauncherCfgWrite();
    }
    THGameGui()
    {
        for (auto& gameDef : gGameDefs) {
            mGames[gameDef.idStr].signature = gameDef;
        }
        LoadGameCfg();
        thcrapSetup();
        mGuiUpdFunc = [&]() { GuiMain(); };
    }
    SINGLETON(THGameGui);

public:
    static HINSTANCE WINAPI thcrapLaunchGame(std::wstring& cfg, const char* game, const char* append = nullptr)
    {
        wchar_t thcrapDir[MAX_PATH];
        wchar_t thcrapArg[MAX_PATH];

        swprintf_s(thcrapDir, L"%s\\%s", THGameGui::singleton().mThcrapDir.c_str(), L"thcrap_loader.exe");
        swprintf_s(thcrapArg, L"\"%s\" %s%s", cfg.c_str(), utf8_to_utf16(game).c_str(), append ? utf8_to_utf16(append).c_str() : L"");
        return ShellExecuteW(NULL, L"open", thcrapDir, thcrapArg, THGameGui::singleton().mThcrapDir.c_str(), SW_SHOW);
    }
    void thcrapPathConversion()
    {
        wchar_t currentPath[MAX_PATH];
        wchar_t cvt[MAX_PATH];
        bool isRelative = false;
        LauncherSettingGet("use_relative_path", isRelative);
        GetModuleFileNameW(GetModuleHandleW(NULL), currentPath, MAX_PATH);

        if (isRelative) {
            if (!PathIsRelativeW(mThcrapDir.c_str())) {
                if (PathRelativePathToW(cvt, currentPath, FILE_ATTRIBUTE_DIRECTORY, mThcrapDir.c_str(), FILE_ATTRIBUTE_NORMAL)) {
                    mThcrapDir = cvt;
                    LauncherSettingSet("thcrap", utf16_to_utf8(mThcrapDir));
                }
            }
        } else {
            if (PathIsRelativeW(mThcrapDir.c_str())) {
                if (GetFullPathNameW(mThcrapDir.c_str(), MAX_PATH, cvt, NULL)) {
                    mThcrapDir = cvt;
                    LauncherSettingSet("thcrap", utf16_to_utf8(mThcrapDir));
                }
            }
        }
    }
    bool thcrapLaunch()
    {
        auto configure = mThcrapDir + L"\\thcrap_configure.exe";
        if (ShellExecuteW(NULL, L"open", configure.c_str(), NULL, mThcrapDir.c_str(), SW_SHOW) > (HINSTANCE)32) {
            return true;
        }
        return false;
    }
    void thcrapAdd(const char* gameId, std::string& cfg, bool use_thprac, bool flush = false)
    {
        if (flush) {
            WriteGameCfg();
            return;
        }

        bool repeated = false;
        for (auto& game : mGames[gameId].instances) {
            if (game.path == cfg) {
                repeated = true;
                break;
            }
        }

        if (!repeated) {
            THGameInst inst;

            char nameStr[256];
            std::string subname = cfg;
            if (IfEndWith(subname, ".js")) {
                for (auto& c : subname) {
                    c = tolower(c);
                }
                subname = subname.substr(0, subname.rfind(".js"));
            }
            sprintf_s(nameStr, "%s (%s)", gameId, subname.c_str());
            
            inst.name = nameStr;
            inst.path = cfg;
            inst.type = TYPE_THCRAP;
            inst.useTHPrac = use_thprac == 1 ? IsTHPracApplicable(TYPE_THCRAP) : false;
            mGames[gameId].instances.push_back(inst);
        }
    }
    void thcrapCfgGet(std::vector<std::pair<std::string, bool>>& cfgVec, std::vector<GameRoll> gameVec[4])
    {
        cfgVec.clear();
        for (auto& cfg : mThcrapCfg) {
            cfgVec.push_back(std::pair<std::string, bool>(cfg, false));
        }
        for (int i = 0; i < 4; i++) {
            for (auto& game : gameVec[i]) {
                game.playerSelect = nullptr;
                game.selected = false;
                for (auto& gameAvail : mThcrapGames) {
                    if (gameAvail == game.name) {
                        game.playerSelect = "GAME";
                        break;
                    }
                }
            }
        }
    }
    bool thcrapTest(std::wstring& dir)
    {
        auto loaderPath = dir + L"\\thcrap_loader.exe";
        auto configurePath = dir + L"\\thcrap_configure.exe";
        auto loaderAttr = GetFileAttributesW(loaderPath.c_str());
        auto configureAttr = GetFileAttributesW(configurePath.c_str());

        if (loaderAttr != INVALID_FILE_ATTRIBUTES && !(loaderAttr & FILE_ATTRIBUTE_DIRECTORY)) {
            if (configureAttr != INVALID_FILE_ATTRIBUTES && !(loaderAttr & FILE_ATTRIBUTE_DIRECTORY)) {
                return true;
            }
        }

        return false;
    }
    bool thcrapChkCfgFolder(std::wstring& dir)
    {
        auto jsConfig = dir + L"\\config.js";
        auto jsGames = dir + L"\\games.js";

        if (!CheckIsValidJson(jsConfig) || !CheckIsValidJson(jsGames)) {
            return false;
        }

        return true;
    }
    bool thcrapChkConfig(std::wstring& jsonPath)
    {
        void* jsBuffer;
        size_t jsSize;
        if (LoadJsonFile(jsonPath, jsBuffer, jsSize)) {
            rapidjson::Document json;
            if (!(json.Parse((const char*)jsBuffer, jsSize + 1).HasParseError()) && json.HasMember("patches") && json["patches"].IsArray() && json["patches"].Size()) {
                bool isValid = true;
                auto& patches = json["patches"];
                for (auto patchIt = patches.Begin(); patchIt != patches.End(); ++patchIt) {
                    if (!(*patchIt).IsObject()) {
                        isValid = false;
                        break;
                    }
                }
                if (isValid) {
                    free(jsBuffer);
                    return true;
                }
            }
            free(jsBuffer);
        }
        return false;
    }
    bool thcrapChkGames(std::wstring& cfgDir)
    {
        void* jsBuffer;
        size_t jsSize;
        auto gameCfgPath = cfgDir + L"\\games.js";
        if (LoadJsonFile(gameCfgPath, jsBuffer, jsSize)) {
            rapidjson::Document json;
            if (!(json.Parse((const char*)jsBuffer, jsSize + 1).HasParseError()) && json.IsObject()) {
                for (auto it = json.MemberBegin(); it != json.MemberEnd(); ++it) {
                    if (it->value.IsString() && strcmp(it->value.GetString(), "")) {
                        mThcrapGames.push_back(std::string(it->name.GetString()));
                    }
                }
            }
            free(jsBuffer);
        }
        return true;
    }
    bool thcrapSetup()
    {
        
        mThcrapDir = L"";
        mThcrapCfgDir = L"";
        mThcrapCfg.clear();
        mThcrapGames.clear();

        std::string thcrapDirUtf8;
        std::wstring thcrapDir;
        LauncherSettingGet("thcrap", thcrapDirUtf8);
        thcrapDir = utf8_to_utf16(thcrapDirUtf8);
        if (!thcrapTest(thcrapDir)) {
            LauncherSettingSet("thcrap", std::string(""));
            return false;
        }
        mThcrapDir = thcrapDir;
        thcrapPathConversion();

        auto cfgDir = thcrapDir + L"\\config";
        if (!thcrapChkCfgFolder(cfgDir)) {
            cfgDir = thcrapDir;
            if (!thcrapChkCfgFolder(cfgDir)) {
                return true;
            }
        }
        mThcrapCfgDir = cfgDir;

        std::wstring searchDir = cfgDir + L"\\*.js";
        WIN32_FIND_DATAW findData;
        HANDLE searchHnd = FindFirstFileW(searchDir.c_str(), &findData);
        if (searchHnd == INVALID_HANDLE_VALUE) {
            return true;
        }
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                continue;
            }
            searchDir = cfgDir + L"\\" + std::wstring(findData.cFileName);

            if (thcrapChkConfig(searchDir)) {
                mThcrapCfg.push_back(utf16_to_utf8(findData.cFileName));
            }

        } while (FindNextFileW(searchHnd, &findData));
        FindClose(searchHnd);

        thcrapChkGames(cfgDir);


        return true;
    }

    void PathCoversion()
    {
        wchar_t currentPath[MAX_PATH];
        wchar_t cvt[MAX_PATH];
        bool isRelative = false;
        LauncherSettingGet("use_relative_path", isRelative);
        GetModuleFileNameW(GetModuleHandleW(NULL), currentPath, MAX_PATH);

        auto& gameGui = THGameGui::singleton();
        for (auto it = gameGui.mGames.begin(); it != gameGui.mGames.end(); ++it) {
            for (auto& gameInst : it.value().instances) {
                if (gameInst.type == TYPE_THCRAP) {
                    continue;
                }
                auto u16Path = utf8_to_utf16(gameInst.path);
                if (isRelative) {
                    if (!PathIsRelativeW(u16Path.c_str())) {
                        if (PathRelativePathToW(cvt, currentPath, FILE_ATTRIBUTE_DIRECTORY, u16Path.c_str(), FILE_ATTRIBUTE_NORMAL)) {
                            gameInst.path = utf16_to_utf8(cvt);
                        }
                    }
                } else {
                    if (PathIsRelativeW(u16Path.c_str())) {
                        if (GetFullPathNameW(u16Path.c_str(), MAX_PATH, cvt, NULL)) {
                            gameInst.path = utf16_to_utf8(cvt);
                        }
                    }
                }
            }
        }
    }
    void ForceReload()
    {
        mGames.clear();
        for (auto& gameDef : gGameDefs) {
            mGames[gameDef.idStr].signature = gameDef;
        }
        LoadGameCfg();
        thcrapSetup();
        PathCoversion();
        WriteGameCfg();
    }
    void FinalizeGameScan()
    {
        int applyThpracDefault = 0;
        LauncherSettingGet("apply_thprac_default", applyThpracDefault);
        for (auto& scan : mGameScan) {
            for (auto& gameScanned : scan) {
                if (!gameScanned.checked) {
                    continue;
                }

                auto gameScannedPathCmp = GetUnifiedPath(gameScanned.game.path);
                bool repeated = false;
                for (auto& game : mGames[gameScanned.signature->idStr].instances) {
                    auto gameExistPathCmp = GetUnifiedPath(game.path);
                    if (gameExistPathCmp == gameScannedPathCmp) {
                        repeated = true;
                        break;
                    }
                }

                if (!repeated) {
                    gameScanned.game.useTHPrac = applyThpracDefault == 1 ? IsTHPracApplicable(gameScanned.game.type) : false;
                    mGames[gameScanned.signature->idStr].instances.push_back(gameScanned.game);
                }
            }
        }
        PathCoversion();
    }
    void ColumnsDebug(int columns)
    {
        for (int i = 0; i < columns; ++i) {
            ImGui::TextWrapped("%f", ImGui::GetColumnOffset(i));
            ImGui::NextColumn();
        }
        ImGui::Separator();
    }
    bool ColumnsScanResultPrt(std::vector<THGameScan>& games)
    {
        bool result = true;
        for (auto& game : games) {
            std::string chkBoxId = "##";
            chkBoxId += game.game.path;
            ImGui::Checkbox(chkBoxId.c_str(), &(game.checked));
            result &= game.checked;
            ImGui::NextColumn();
            ImGui::TextWrapped("%s (%s)", game.signature->idStr, XSTR(TH_TYPE_SELECT[game.game.type]));
            ImGui::NextColumn();
            GuiColumnText(game.game.path.c_str());
            ImGui::NextColumn();
            ImGui::Separator();
        }
        return result;
    }
    void ColumnsScanResultHdr(bool& selectAll, std::vector<THGameScan>& games)
    {
        float offset[] = {
            0.0f, 2.0f, 10.0f
        };
        ImGui::Columns(3, "##@__scan_result_col", true);
        for (int i = 0; i < 3; ++i) {
            ImGui::SetColumnOffset(i, offset[i] * ImGui::GetFontSize());
        }
        ImGui::Separator();
        if (ImGui::Checkbox("##@__select_all", &selectAll)) {
            for (auto& game : games) {
                game.checked = selectAll;
            }
        }
        ImGui::NextColumn();
        ImGui::Text(XSTR(THPRAC_SCAN_SCAN_RESULT_C1));
        ImGui::NextColumn();
        ImGui::Text(XSTR(THPRAC_SCAN_SCAN_RESULT_C2));
        ImGui::NextColumn();
        ImGui::Separator();
    }
    void GuiScanCheck(const char* text, int idx, ImVec4* color = nullptr)
    {
        char childId[64];
        sprintf_s(childId, "##@__result_c%d", idx);

        if (GuiButtonAndModalYesNo(XSTR(THPRAC_ABORT), XSTR(THPRAC_SCAN_SCAN_ABORT_TITLE), XSTR(THPRAC_SCAN_SCAN_ABORT_TEXT), 6.0f, XSTR(THPRAC_OK), XSTR(THPRAC_CANCEL))) {
            mGuiUpdFunc = [&]() { GuiMain(); };
        }
        ImGui::SameLine();
        GuiCenteredText(XSTR(THPRAC_GAMES_SCAN_FOLDER));
        ImGui::Separator();

        if (color) {
            ImGui::PushStyleColor(ImGuiCol_Text, *color);
            ImGui::TextWrapped(text);
            ImGui::PopStyleColor();

        } else {
            ImGui::TextWrapped(text);
        }

        ImGui::NewLine();
        ImGui::BeginChild(childId, ImVec2(0, -2.0f * ImGui::GetFontSize()), true);
        if (mGameScanScrollReset[idx]) {
            mGameScanScrollReset[idx] = false;
            ImGui::SetScrollX(0.0f);
            ImGui::SetScrollY(0.0f);
        }
        ColumnsScanResultHdr(mGameScanSelect[idx], mGameScan[idx]);
        mGameScanSelect[idx] = ColumnsScanResultPrt(mGameScan[idx]);
        ImGui::Columns(1);
        ImGui::EndChild();

        auto buttonRes = GuiCornerButton(XSTR(THPRAC_BACK), XSTR(THPRAC_NEXT));
        if (buttonRes == 1) {
            GuiScanCheckMenuSwitch(idx - 1, false);
        } else if (buttonRes == 2) {
            GuiScanCheckMenuSwitch(idx + 1, true);
        }
    }
    void GuiScanCheckFinish()
    {
        GuiSetPosYRel(0.5f);
        GuiCenteredText(XSTR(THPRAC_SCAN_COMPLETE));
        auto buttonRes = GuiCornerButton(XSTR(THPRAC_BACK), XSTR(THPRAC_FINISH));
        if (buttonRes == 1) {
            GuiScanCheckMenuSwitch(GAME_SCAN_TYPE_CNT - 1, false);
        } else if (buttonRes == 2) {
            FinalizeGameScan();
            WriteGameCfg();
            mGuiUpdFunc = [&]() { GuiMain(); };
        }
    }
    void GuiScanCheckUncertain()
    {
        GuiScanCheck(XSTR(THPRAC_SCAN_CONFIRM_UNCERTAIN), 5);
    }
    void GuiScanCheckModded()
    {
        GuiScanCheck(XSTR(THPRAC_SCAN_CONFIRM_MODDED), 4);
    }
    void GuiScanCheckSteam()
    {
        GuiScanCheck(XSTR(THPRAC_SCAN_CONFIRM_STEAM), 3);
    }
    void GuiScanCheckThcrap()
    {
        GuiScanCheck(XSTR(THPRAC_SCAN_CONFIRM_THCRAP), 2);
    }
    void GuiScanCheckValid()
    {
        GuiScanCheck(XSTR(THPRAC_SCAN_CONFIRM_VALID), 1);
    }
    void GuiScanCheckMalicious()
    {
        const char* text = "Malicious";
        int idx = 0;
        ImVec4 color { 255.0f, 0.0f, 0.0f, 255.0f };
        char childId[64];
        sprintf_s(childId, "##@__result_c%d", idx);

        if (GuiButtonAndModalYesNo(XSTR(THPRAC_ABORT), XSTR(THPRAC_SCAN_SCAN_ABORT_TITLE), XSTR(THPRAC_SCAN_SCAN_ABORT_TEXT), 6.0f, XSTR(THPRAC_OK), XSTR(THPRAC_CANCEL))) {
            mGuiUpdFunc = [&]() { GuiMain(); };
        }
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        GuiCenteredText(XSTR(THPRAC_SCAN_MALICIOUS_TITLE));
        ImGui::PopStyleColor();
        ImGui::Separator();

        ImGui::TextWrapped(XSTR(THPRAC_SCAN_CONFIRM_MALICIOUS));

        ImGui::NewLine();
        ImGui::BeginChild(childId, ImVec2(0, -2.0f * ImGui::GetFontSize()), true);
        if (mGameScanScrollReset[idx]) {
            mGameScanScrollReset[idx] = false;
            ImGui::SetScrollX(0.0f);
            ImGui::SetScrollY(0.0f);
        }

        float offset[] = {
            0.0f, 8.0f
        };
        ImGui::Columns(2, "##@__scan_result_col", true);
        for (int i = 0; i < 2; ++i) {
            ImGui::SetColumnOffset(i, offset[i] * ImGui::GetFontSize());
        }
        ImGui::Separator();
        ImGui::Text(XSTR(THPRAC_SCAN_SCAN_RESULT_C1));
        ImGui::NextColumn();
        ImGui::Text(XSTR(THPRAC_SCAN_SCAN_RESULT_C2));
        ImGui::NextColumn();
        ImGui::Separator();
        for (auto& game : mGameScan[idx]) {
            ImGui::TextWrapped("%s (%s)", game.signature->idStr, XSTR(TH_TYPE_SELECT[game.game.type]));
            ImGui::NextColumn();
            GuiColumnText(game.game.path.c_str());
            ImGui::NextColumn();
            ImGui::Separator();
        }
        ImGui::Columns(1);
        ImGui::EndChild();

        auto buttonRes = GuiCornerButton(XSTR(THPRAC_BACK), XSTR(THPRAC_NEXT));
        if (buttonRes == 1) {
            GuiScanCheckMenuSwitch(idx - 1, false);
        } else if (buttonRes == 2) {
            GuiScanCheckMenuSwitch(idx + 1, true);
        }
    }
    void GuiScanCheckMenuSwitch(int idx, bool forward = true)
    {
        int direction = forward ? 1 : -1;
        for (; idx < GAME_SCAN_TYPE_CNT && idx >= 0; idx += direction) {
            if (mGameScan[idx].size()) {
                switch (idx) {
                case 0:
                    mGuiUpdFunc = [&]() { GuiScanCheckMalicious(); };
                    return;
                case 1:
                    mGuiUpdFunc = [&]() { GuiScanCheckValid(); };
                    return;
                case 2:
                    mGuiUpdFunc = [&]() { GuiScanCheckThcrap(); };
                    return;
                case 3:
                    mGuiUpdFunc = [&]() { GuiScanCheckSteam(); };
                    return;
                case 4:
                    mGuiUpdFunc = [&]() { GuiScanCheckModded(); };
                    return;
                case 5:
                    mGuiUpdFunc = [&]() { GuiScanCheckUncertain(); };
                    return;
                }
            }
        }
        if (idx >= GAME_SCAN_TYPE_CNT) {
            mGuiUpdFunc = [&]() { GuiScanCheckFinish(); };
        } else if (idx < 0) {
            mGuiUpdFunc = [&]() { GuiScanFolder(); };
        }
    }

    static bool WINAPI CheckExeName(const std::wstring& dir, const char* idStr)
    {
        if (!strcmp(idStr, "th06")) {
            if (CheckExeName(dir, "“Œ•ûg–‚‹½") || CheckExeName(dir, "–|·½¼tÄ§à_")) {
                return true;
            }
        }

        auto idStr16 = utf8_to_utf16(idStr);
        auto fileName = dir;

        if (fileName.rfind(L"\\") != std::string::npos) {
            fileName = fileName.substr(fileName.rfind(L"\\") + 1);
        }
        fileName = GetUnifiedPath(fileName);
        if (fileName.find(idStr16) == 0) {
            // TODO
            if (fileName == L"th155_log.exe") {
                return false;
            }
            return true;
        }
        return false;
    }
    static THGameType CompareExeSig(ExeSig& sig1, ExeSig& sig2)
    {
        if (HashCompare(sig1.metroHash, sig2.metroHash)) {
            return TYPE_ORIGINAL;
        }
        auto oepCmp = OepCompare(sig1.oepCode, sig2.oepCode);
        if (sig1.timeStamp == sig2.timeStamp && sig1.textSize == sig2.textSize) {
            if (oepCmp) {
                return TYPE_MODDED;
            } else {
                return TYPE_MALICIOUS;
            }
        }
        return TYPE_UNCERTAIN;
    }
    static DWORD WINAPI ScanAddGame(THGameType type, std::string name, std::string& path, THGameSig& sig)
    {
        auto& game = THGameGui::singleton();
        switch (type) {
        case THPrac::TYPE_ORIGINAL:
            if (!game.mScanOption[SCAN_OPT_ORIGINAL]) {
                return 0;
            }
            break;
        case THPrac::TYPE_MODDED:
        case THPrac::TYPE_CHINESE:
        case THPrac::TYPE_SCHINESE:
        case THPrac::TYPE_TCHINESE:
        case THPrac::TYPE_NYASAMA:
            if (!game.mScanOption[SCAN_OPT_MODDED]) {
                return 0;
            }
            break;
        case THPrac::TYPE_MALICIOUS:
        case THPrac::TYPE_UNCERTAIN:
            if (!game.mScanOption[SCAN_OPT_ORIGINAL] && !game.mScanOption[SCAN_OPT_MODDED]) {
                return 0;
            }
            break;
        default:
            break;
        }

        if (name == "" && path != "") {
            name = path;
            auto nameSubpos = name.rfind('\\');
            if (nameSubpos != std::string::npos) {
                name = name.substr(nameSubpos + 1);
            }
            name = name.substr(0, name.size() - 4);
            if (type == TYPE_STEAM) {
                name += " (Steam)";
            }
        }

        switch (type) {
        case THPrac::TYPE_MALICIOUS:
            game.mGameScan[0].emplace_back(name.c_str(), path.c_str(), type, &sig, false);
            break;
        case THPrac::TYPE_ORIGINAL:
        case THPrac::TYPE_CHINESE:
        case THPrac::TYPE_SCHINESE:
        case THPrac::TYPE_TCHINESE:
        case THPrac::TYPE_NYASAMA:
            game.mGameScan[1].emplace_back(name.c_str(), path.c_str(), type, &sig);
            break;
        case THPrac::TYPE_THCRAP:
            game.mGameScan[2].emplace_back(name.c_str(), path.c_str(), type, &sig);
            break;
        case THPrac::TYPE_STEAM:
            game.mGameScan[3].emplace_back(name.c_str(), path.c_str(), type, &sig);
            break;
        case THPrac::TYPE_MODDED:
            game.mGameScan[4].emplace_back(name.c_str(), path.c_str(), type, &sig, false);
            break;
        case THPrac::TYPE_UNCERTAIN:
            game.mGameScan[5].emplace_back(name.c_str(), path.c_str(), type, &sig, false);
            break;
        default:
            return 0;
            break;
        }
        return 1;
    }
    static DWORD WINAPI ScanSetCurrentPath(const std::wstring& dir)
    {
        auto& g = THGameGui::singleton();
        auto nextIdx = (g.mScanCurrentIdx == 0) ? 1 : 0;
        g.mScanCurrent[nextIdx] = utf16_to_utf8(dir);

        // Hopefully this is an atom operation.
        g.mScanCurrentIdx = nextIdx;

        return 0;
    }
    static DWORD WINAPI ScanSteamappPath(std::wstring& path)
    {
        for (auto& gameDef : gGameDefs) {
            if (gameDef.steamId == nullptr) {
                continue;
            }
            std::wstring checkPath = path + L"\\appmanifest_" + gameDef.steamId + L".acf";
            if (GetFileAttributesW(checkPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                ScanAddGame(TYPE_STEAM, "", utf16_to_utf8(path) + "\\common\\" + gameDef.idStr + '\\' + gameDef.idStr + ".exe", gameDef);
            }
        }
        return 0;
    }
    static DWORD WINAPI ScanSteam()
    {
        if (!THGameGui::singleton().mScanOption[SCAN_OPT_STEAM]) {
            return 0;
        }

        std::wstring steamPath = L"";
        DWORD dwType = REG_SZ;
        HKEY hKey = 0;
        wchar_t value[MAX_PATH];
        DWORD value_length = sizeof(value);
        if (RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", &hKey) == ERROR_SUCCESS) {
            if (RegQueryValueEx(hKey, L"InstallPath", NULL, &dwType, (LPBYTE)value, &value_length) == ERROR_SUCCESS) {
                steamPath = value;
            }
            RegCloseKey(hKey);
        }
        if (steamPath == L"") {
            if (RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", &hKey) == ERROR_SUCCESS) {
                if (RegQueryValueEx(hKey, L"InstallPath", NULL, &dwType, (LPBYTE)value, &value_length) == ERROR_SUCCESS) {
                    steamPath = value;
                }
                RegCloseKey(hKey);
            }
        }
        if (steamPath == L"") {
            return 0;
        }
        steamPath = GetCleanedPath(steamPath);

        ScanSteamappPath(steamPath + L"\\steamapps");
        auto libraryCfgPath = steamPath + L"\\steamapps\\libraryfolders.vdf";
        auto hCfg = CreateFileW(libraryCfgPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hCfg != INVALID_HANDLE_VALUE) {
            DWORD bytesRead;
            auto cfgSize = GetFileSize(hCfg, NULL);
            auto cfgBuffer = malloc(cfgSize);
            memset(cfgBuffer, 0, cfgSize);
            ReadFile(hCfg, cfgBuffer, cfgSize, &bytesRead, NULL);
            std::string cfgStr((const char*)cfgBuffer);
            std::string steamappsPath;
            steamappsPath.reserve(512);

            std::string::size_type searchStart = 0;
            std::string::size_type searchPos = 0;
            for (searchPos = cfgStr.find(":\\\\");
                 searchPos != std::string::npos;
                 searchPos = cfgStr.find(":\\\\", searchPos + 1)) {
                if (isalpha(cfgStr[searchPos - 1]) && cfgStr[searchPos - 2] == '\"') {
                    auto endQuotePos = cfgStr.find("\"", searchPos);
                    if (endQuotePos != std::string::npos) {
                        steamappsPath.clear();
                        steamappsPath = cfgStr.substr(searchPos - 1, endQuotePos - searchPos + 1);
                        steamappsPath = GetCleanedPath(steamappsPath);
                        if (utf8_to_utf16(steamappsPath) != steamPath) {
                            ScanSteamappPath(utf8_to_utf16(steamappsPath + "\\steamapps"));
                        }
                    }
                }
            }

            free(cfgBuffer);
            CloseHandle(hCfg);
        }

        return 0;
    }
    static DWORD WINAPI ScanExe(const std::wstring& dir, std::string name = "")
    {
        HANDLE hFile = INVALID_HANDLE_VALUE;
        DWORD fileSize = 0;
        HANDLE hFileMap = NULL;
        void* pFileMapView = nullptr;
        auto& games = THGameGui::singleton().mGames;
        auto utf8Dir = utf16_to_utf8(dir);

        // Open the file.
        hFile = CreateFileW(dir.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            goto end;
        fileSize = GetFileSize(hFile, NULL);
        if (fileSize > (1 << 23))
            goto end; // Pass if the file is too large.
        hFileMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
        if (!hFileMap)
            goto end;
        pFileMapView = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, fileSize);
        if (!pFileMapView)
            goto end;

        ExeSig exeSig;
        GetExeInfo(pFileMapView, fileSize, exeSig);
        THGameSig* ifSig = nullptr;
        THGameType ifType = TYPE_UNCERTAIN;

        for (int i = 0; i < IM_ARRAYSIZE(gGameDefs); ++i) {
            auto& gameSig = gGameDefs[i];
            auto type = CompareExeSig(exeSig, gameSig.exeSig);
            if (gameSig.catagory != CAT_MAIN && gameSig.catagory != CAT_SPINOFF_STG) {
                if (CheckExeName(dir, gameSig.idStr)) {
                    ifSig = &gameSig;
                }
            }
            if (type == TYPE_MODDED) {
                ifSig = &gameSig;
                ifType = TYPE_MODDED;
                break;
            } else if (type == TYPE_MALICIOUS) {
                if (!CheckHasSteamDRM(pFileMapView, fileSize)) {
                    ScanAddGame(type, name, utf8Dir, gameSig);
                }
            } else if (type != TYPE_UNCERTAIN) {
                ScanAddGame(type, name, utf8Dir, gameSig);
                goto end;
            }
        }

        for (auto& known : gKnownGames) {
            if (HashCompare(exeSig.metroHash, known.metroHash)) {
                ScanAddGame(known.type, name, utf8Dir, games[known.idStr].signature);
                goto end;
            }
        }

        if (ifSig) {
            ScanAddGame(ifType, name, utf8Dir, *ifSig);
        }

    end:
        if (pFileMapView)
            UnmapViewOfFile(pFileMapView);
        if (hFileMap)
            CloseHandle(hFileMap);
        if (hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
        return 0;
    }
    static DWORD WINAPI ScanFolder(const std::wstring& dir)
    {
        CoInitializeEx(NULL, COINIT_MULTITHREADED);
        auto scanLnk = THGameGui::singleton().mScanOption[SCAN_OPT_THCRAP];
        std::wstring searchDir = dir + L"\\*";
        WIN32_FIND_DATAW findData;
        HANDLE searchHnd = FindFirstFileW(searchDir.c_str(), &findData);

        if (searchHnd == INVALID_HANDLE_VALUE) {
            return false;
        }

        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if ((!lstrcmpW(findData.cFileName, L".")) || (!lstrcmpW(findData.cFileName, L"..")))
                    continue;
            }

            searchDir = dir + L"\\" + std::wstring(findData.cFileName);
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                ScanFolder(searchDir);
            } else {
                ScanSetCurrentPath(searchDir);
                auto suffix = searchDir.substr(searchDir.size() - 4, 4);
                for (auto& c : suffix) {
                    c = std::towlower(c);
                }
                if (suffix == L".exe") {
                    ScanExe(searchDir);
                }
            }

        } while (FindNextFileW(searchHnd, &findData));
        FindClose(searchHnd);
        CoUninitialize();

        return 0;
    }
    static DWORD WINAPI ScanThreadFunc(_In_ LPVOID lpParameter)
    {
        auto& game = THGameGui::singleton();
        auto& dir = game.mScanPath;
        if (game.mScanOption[SCAN_OPT_ORIGINAL] || game.mScanOption[SCAN_OPT_MODDED] || THGameGui::singleton().mScanOption[SCAN_OPT_THCRAP]) {
            ScanFolder(dir);
        }
        ScanSteam();
        return 0;
    }
    static DWORD WINAPI RescanThreadFunc(_In_ LPVOID lpParameter)
    {
        auto& gameGui = THGameGui::singleton();
        gameGui.thcrapSetup();
        CoInitializeEx(NULL, COINIT_MULTITHREADED);

        for (auto it = gameGui.mGames.begin(); it != gameGui.mGames.end(); ++it) {
            for (auto& gameInst : it.value().instances) {
                ScanSetCurrentPath(utf8_to_utf16(gameInst.path));
                switch (gameInst.type) {
                case THPrac::TYPE_ORIGINAL:
                case THPrac::TYPE_MODDED:
                case THPrac::TYPE_CHINESE:
                case THPrac::TYPE_SCHINESE:
                case THPrac::TYPE_TCHINESE:
                case THPrac::TYPE_MALICIOUS:
                case THPrac::TYPE_NYASAMA:
                    ScanExe(utf8_to_utf16(gameInst.path), gameInst.name);
                    break;
                case THPrac::TYPE_STEAM: {
                    auto attr = GetFileAttributesW(utf8_to_utf16(gameInst.path).c_str());
                    if (attr != INVALID_FILE_ATTRIBUTES && attr != FILE_ATTRIBUTE_DIRECTORY) {
                        ScanAddGame(TYPE_STEAM, gameInst.name, gameInst.path, it.value().signature);
                    }
                } break;
                case THPrac::TYPE_THCRAP:
                    for (auto& cfg : gameGui.mThcrapCfg) {
                        if (cfg == gameInst.path) {
                            ScanAddGame(gameInst.type, gameInst.name, gameInst.path, it.value().signature);
                            continue;
                        }
                    }
                    break;
                case THPrac::TYPE_UNCERTAIN:
                case THPrac::TYPE_UNKNOWN:
                    ScanAddGame(gameInst.type, gameInst.name, gameInst.path, it.value().signature);
                    break;
                default:
                    break;
                }
            }
            it.value().instances.clear();
        }

        for (auto& scan : gameGui.mGameScan) {
            for (auto& gameScanned : scan) {
                gameGui.mGames[gameScanned.signature->idStr].instances.push_back(gameScanned.game);
                gameGui.mGames[gameScanned.signature->idStr].selected = 0;
            }
        }

        gameGui.WriteGameCfg();
        CoUninitialize();
        return 0;
    }
    void ScanClear()
    {
        mScanThread.Stop();
        mScanCurrent[0] = mScanCurrent[1] = "";
        mScanCurrentIdx = 0;
        mScanPath = L"";
        //mScanPath = L"D:\\\\Touhou";
        mScanStatus = 0;
        mScanAnm.Reset();

        mGameScanSelect[0] = false;
        mGameScanSelect[1] = true;
        mGameScanSelect[2] = true;
        mGameScanSelect[3] = false;
        mGameScanSelect[4] = false;

        for (auto& opt : mScanOption) {
            opt = false;
        }
        for (auto& sc : mGameScanScrollReset) {
            sc = true;
        }

        for (auto& v : mGameScan) {
            v.clear();
        }

        mSteamMenuStatus = 0;
    }
    void ScanComplete()
    {
        mScanStatus = 2;
        mScanThread.Stop();
    }
    void ScanAbort()
    {
        mScanStatus = 0;
        mScanThread.Stop();
    }
    void GuiScanFolder()
    {
        if (GuiButtonAndModalYesNo(XSTR(THPRAC_ABORT), XSTR(THPRAC_SCAN_SCAN_ABORT_TITLE), XSTR(THPRAC_SCAN_SCAN_ABORT_TEXT), 6.0f, XSTR(THPRAC_OK), XSTR(THPRAC_CANCEL))) {
            ScanAbort();
            ScanClear();
            mGuiUpdFunc = [&]() { GuiMain(); };
        }
        ImGui::SameLine();
        GuiCenteredText(XSTR(THPRAC_GAMES_SCAN_FOLDER));
        ImGui::Separator();

        if (mScanStatus == 0) {
            ImGui::TextWrapped(XSTR(THPRAC_SCAN_INSTRUCTION));
            ImGui::NewLine();
            ImGui::Text(XSTR(THPRAC_SCAN_SCAN_FOR));
            ImGui::Checkbox(XSTR(THPRAC_SCAN_ORIGINAL), &mScanOption[SCAN_OPT_ORIGINAL]);
            ImGui::Checkbox(XSTR(THPRAC_SCAN_MODDED), &mScanOption[SCAN_OPT_MODDED]);
            ImGui::Checkbox(XSTR(THPRAC_SCAN_STEAM), &mScanOption[SCAN_OPT_STEAM]);

            bool canProceed = !mScanOption[SCAN_OPT_ORIGINAL] && !mScanOption[SCAN_OPT_MODDED] && !mScanOption[SCAN_OPT_THCRAP] && mScanOption[SCAN_OPT_STEAM];
            if (mScanOption[SCAN_OPT_ORIGINAL] || mScanOption[SCAN_OPT_MODDED] || mScanOption[SCAN_OPT_THCRAP]) {
                ImGui::NewLine();
                if (ImGui::Button(XSTR(THPRAC_SCAN_SELECT_FOLDER))) {
                    auto path = LauncherWndFolderSelect();
                    if (path != L"") {
                        mScanPath = path;
                    }
                }
                if (mScanPath != L"") {
                    ImGui::TextWrapped(XSTR(THPRAC_SCAN_FOLDER_SELECTED), utf16_to_utf8(mScanPath).c_str());
                } else {
                    ImGui::TextWrapped(XSTR(THPRAC_SCAN_FOLDER_NOT_SELECTED));
                }
                canProceed = mScanPath != L"";
            }
            if (canProceed) {
                if (GuiCornerButton(XSTR(THPRAC_BEGIN))) {
                    mScanThread.Start();
                    mScanStatus = 1;
                }
            }
        } else if (mScanStatus == 1) {
            GuiSetPosYRel(0.5f);
            GuiSetPosXText(XSTR(THPRAC_SCAN_SCANNING));
            ImGui::TextWrapped(XSTR(THPRAC_SCAN_SCANNING));
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text(mScanAnm.Get().c_str());
            GuiCenteredText(mScanCurrent[mScanCurrentIdx].c_str());

            if (!mScanThread.IsActive()) {
                ScanComplete();
            }
        } else if (mScanStatus == 2) {
            GuiSetPosYRel(0.5f);
            GuiCenteredText(XSTR(THPRAC_SCAN_SCAN_FINISHED));
            if (GuiCornerButton(XSTR(THPRAC_NEXT))) {
                GuiScanCheckMenuSwitch(0);
            }
        }
    }
    bool GuiGameTypeChkBox(const char* text, int idx)
    {
        bool result = ImGui::Checkbox(text, &(mSteamGameTypeOpt[idx]));
        if (result) {
            for (auto& game : mSteamGames[idx]) {
                if (game.playerSelect) {
                    game.selected = mSteamGameTypeOpt[idx];
                }
            }
        }
        return result;
    }
    void GuiScanSteam()
    {
        if (mSteamMenuStatus == 0) {
            if (ImGui::Button(XSTR(THPRAC_BACK))) {
                mGuiUpdFunc = [&]() { GuiMain(); };
            }
            ImGui::SameLine();
            GuiCenteredText(XSTR(THPRAC_STEAM_MNG));
            ImGui::Separator();

            ImGui::TextWrapped(XSTR(THPRAC_STEAM_MNG_DESC));
            ImGui::NewLine();
            if (ImGui::Button(XSTR(THPRAC_STEAM_MNG_AUTO))) {
                mScanOption[SCAN_OPT_STEAM] = true;
                mScanThread.Start();
                mScanStatus = 1;
                mGuiUpdFunc = [&]() { GuiScanFolder(); };
            }
            ImGui::SameLine();
            ImGui::Text(XSTR(THPRAC_STEAM_MNG_OR));
            ImGui::SameLine();
            if (ImGui::Button(XSTR(THPRAC_STEAM_MNG_MANUAL))) {
                for (auto& games : mSteamGames) {
                    games.clear();
                }
                for (auto& game : gGameDefs) {
                    auto gameType = (GameRollType)((int)game.catagory + 1);
                    GameRoll roll { game.idStr, gameType, game.steamId ? "STEAM": nullptr, 1, false };

                    auto it = mGames.find(std::string(roll.name));
                    if (it != mGames.end()) {
                        auto& existingGames = it.value();
                        for (auto& existingGame : existingGames.instances) {
                            if (existingGame.type == TYPE_STEAM) {
                                roll.selected = true;
                                break;
                            }
                        }
                    }

                    mSteamGames[gameType].push_back(roll);
                }
                mSteamMenuStatus = 1;
            }
        } else if (mSteamMenuStatus == 1) {
            if (ImGui::Button(XSTR(THPRAC_BACK))) {
                mSteamMenuStatus = 0;
            }
            ImGui::SameLine();
            GuiCenteredText(XSTR(THPRAC_STEAM_MNG_MANUAL_TITLE));
            ImGui::Separator();

            ImGui::TextWrapped(XSTR(THPRAC_STEAM_MNG_MANUAL_INSTRUCTION));
            ImGui::NewLine();

            int i = 0;
            for (auto& gameType : mSteamGames) {
                if (i) {
                    bool allSelected = true;
                    bool allUnavailable = true;
                    ImGui::Columns(i == 1 ? 7 : 6, 0, false);
                    for (auto& game : gameType) {
                        if (game.playerSelect) {
                            allUnavailable = false;
                            ImGui::Checkbox(game.name, &game.selected);
                            if (!game.selected) {
                                allSelected = false;
                            }
                        } else {
                            ImGui::BeginDisabled();
                            ImGui::Checkbox(game.name, &game.selected);
                            ImGui::EndDisabled();
                        }
                        mSteamGameTypeOpt[i] = allSelected;
                        ImGui::NextColumn();
                    }
                    if (allUnavailable) {
                        mSteamGameTypeOpt[i] = false;
                    }
                    ImGui::Columns(1);
                    if (i != 3) {
                        ImGui::NewLine();
                    }
                }
                ++i;
            }
            ImGui::NewLine();
            ImGui::Indent(ImGui::GetStyle().ItemSpacing.x);
            GuiGameTypeChkBox(XSTR(THPRAC_GAMES_MAIN_SERIES), 1);
            ImGui::SameLine();
            GuiGameTypeChkBox(XSTR(THPRAC_GAMES_SPINOFF_STG), 2);
            ImGui::SameLine();
            GuiGameTypeChkBox(XSTR(THPRAC_GAMES_SPINOFF_OTHERS), 3);
            ImGui::Unindent(ImGui::GetStyle().ItemSpacing.x);

            if (GuiCornerButton(XSTR(THPRAC_APPLY))) {
                for (auto& steamGames : mSteamGames) {
                    for (auto& steamGame : steamGames) {
                        auto it = mGames.find(std::string(steamGame.name));
                        if (it != mGames.end()) {
                            bool hasSteamGame = false;
                            auto& existingGames = it.value().instances;
                            for (auto gameIt = existingGames.begin();
                                gameIt != existingGames.end();
                                ++gameIt) {
                                if ((*gameIt).type == TYPE_STEAM) {
                                    hasSteamGame = true;
                                    if (!steamGame.selected) {
                                        it.value().selected = 0;
                                        existingGames.erase(gameIt);
                                    }
                                    break;
                                }
                            }
                            if (!hasSteamGame && steamGame.selected) {
                                std::string gameName = steamGame.name;
                                gameName += " (Steam)";
                                mGameScan[3].emplace_back(gameName.c_str(), "", TYPE_STEAM, &(it.value().signature));
                            }
                        }
                    }
                }
                
                FinalizeGameScan();
                WriteGameCfg();
                mSteamMenuStatus = 2;
            }
        } else if (mSteamMenuStatus == 2) {
            GuiCenteredText(XSTR(THPRAC_STEAM_MNG_MANUAL_TITLE));
            ImGui::Separator();
            GuiSetPosYRel(0.5f);
            GuiCenteredText(XSTR(THPRAC_STEAM_MNG_MANUAL_COMPLETE));
             if (GuiCornerButton(XSTR(THPRAC_FINISH))) {
                mGuiUpdFunc = [&]() { GuiMain(); };
            }
        }
    }

    static bool LocalApplyTHPrac(HANDLE process)
    {
        void* extraData = nullptr;
        size_t extraSize = 0;

        return THPrac::LoadSelf(process, extraData, extraSize);
    }
    static bool WINAPI CheckProcessOmni(PROCESSENTRY32W& proc)
    {
        if (wcscmp(L"搶曽峠杺嫿.exe", proc.szExeFile) && wcscmp(L"東方紅魔郷.exe", proc.szExeFile)) {
            if (proc.szExeFile[0] != L't' || proc.szExeFile[1] != L'h')
                return nullptr;
            if (proc.szExeFile[2] < 0x30 || proc.szExeFile[2] > 0x39)
                return nullptr;
            if (proc.szExeFile[3] < 0x30 || proc.szExeFile[3] > 0x39)
                return nullptr;
        }

        // Open the related process
        auto hProc = OpenProcess(
            //PROCESS_SUSPEND_RESUME |
            PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
            FALSE,
            proc.th32ProcessID);
        if (!hProc)
            return nullptr;

        // Check THPrac signature
        DWORD sigAddr = 0;
        DWORD sigCheck = 0;
        DWORD bytesReadRPM;
        ReadProcessMemory(hProc, (void*)0x40003c, &sigAddr, 4, &bytesReadRPM);
        if (bytesReadRPM != 4 || !sigAddr) {
            CloseHandle(hProc);
            return nullptr;
        }
        ReadProcessMemory(hProc, (void*)(0x400000 + sigAddr - 4), &sigCheck, 4, &bytesReadRPM);
        if (bytesReadRPM != 4 || sigCheck) {
            CloseHandle(hProc);
            return nullptr;
        }

        ExeSig sig;
        if (GetExeInfoEx((size_t)hProc, sig)) {
            for (auto& gameDef : gGameDefs) {
                if (gameDef.catagory != CAT_MAIN && gameDef.catagory != CAT_SPINOFF_STG) {
                    continue;
                }
                if (gameDef.exeSig.textSize != sig.textSize || gameDef.exeSig.timeStamp != sig.timeStamp) {
                    continue;
                }
                return &gameDef;
            }
        }
        CloseHandle(hProc);
        return nullptr;   
    }
    static DWORD WINAPI CheckProcess(DWORD process, std::wstring& exePath)
    {
        // TODO: THPRAC SIG CHECK & EXE TIME STAMP CHECK
        int result = 0;
        auto currentGame = THGameGui::singleton().mCurrentGame;
        if (!currentGame) {
            return 0;
        }
        auto& currentInst = currentGame->instances[currentGame->selected];
        auto exePathU16 = exePath;

        HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
        MODULEENTRY32W me32;
        me32.dwSize = sizeof(MODULEENTRY32W);
        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process);
        if (Module32FirstW(hModuleSnap, &me32)) {
            auto modulePath = GetUnifiedPath(std::wstring(me32.szExePath));
            if (exePathU16 == modulePath) {
                result = 1;
            } else {
                while (Module32NextW(hModuleSnap, &me32)) {
                    modulePath = GetUnifiedPath(std::wstring(me32.szExePath));
                    if (exePathU16 == modulePath) {
                        result = 1;
                        break;
                    }
                }
            }
        }
        CloseHandle(hModuleSnap);

        if (result) {
            auto hProc = OpenProcess(
                PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
                FALSE,
                process);
            if (hProc) {
                // Check THPrac signature
                DWORD sigAddr = 0;
                DWORD sigCheck = 0;
                DWORD bytesReadRPM;
                ReadProcessMemory(hProc, (void*)0x40003c, &sigAddr, 4, &bytesReadRPM);
                if (bytesReadRPM != 4 || !sigAddr) {
                    CloseHandle(hProc);
                    return 0;
                }
                ReadProcessMemory(hProc, (void*)(0x400000 + sigAddr - 4), &sigCheck, 4, &bytesReadRPM);
                if (bytesReadRPM != 4 || sigCheck) {
                    CloseHandle(hProc);
                    return 0;
                }
                CloseHandle(hProc);
            } else {
                return 0;
            }
        }

        return result;
    }
    static DWORD WINAPI WaitAndApplyTHPrac(std::wstring& exePath)
    {
        auto& gameGui = THGameGui::singleton();
        bool isOmni = exePath == L"" ? true : false;

        do {
            if (CheckIfAnyGame()) {
                // Find ongoing game
                PROCESSENTRY32W procEntry;
                MODULEENTRY32W moduleEntry;
                procEntry.dwSize = sizeof(PROCESSENTRY32W);
                moduleEntry.dwSize = sizeof(MODULEENTRY32W);
                HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
                //bool hasPrompted = false;
                if (Process32FirstW(snapshot, &procEntry)) {
                    do {
                        bool test = isOmni ? CheckProcessOmni(procEntry) : CheckProcess(procEntry.th32ProcessID, exePath);
                        if (test) {
                            auto hProc = OpenProcess(
                                //PROCESS_SUSPEND_RESUME |
                                PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
                                FALSE,
                                procEntry.th32ProcessID);
                            if (hProc) {
                                auto result = (WriteTHPracSig(hProc) && LocalApplyTHPrac(hProc));
                                CloseHandle(hProc);
                                return result ? 1 : 0;
                            }
                        }
                    } while (Process32NextW(snapshot, &procEntry));
                }
            }
            Sleep(500);
        } while (!gameGui.mLaunchAbortInd);
        return 0;
    }
    static DWORD WINAPI CheckDLLFunction(std::wstring& path, const char* funcName)
    {
#define MakePointer(t, p, offset) ((t)((PUINT8)(p) + offset))
        int result = 0;
        HANDLE hFile = INVALID_HANDLE_VALUE;
        DWORD fileSize = 0;
        HANDLE hFileMap = NULL;
        void* pFileMapView = nullptr;
        hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            goto cdf_end;
        fileSize = GetFileSize(hFile, NULL);
        if (fileSize > (1 << 23))
            goto cdf_end; // Pass if the file is too large.
        hFileMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
        if (!hFileMap)
            goto cdf_end;
        pFileMapView = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, fileSize);
        if (!pFileMapView)
            goto cdf_end;

        auto exeSize = fileSize;
        auto exeBuffer = pFileMapView;
        if (exeSize < 128)
            goto cdf_end;
        PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)exeBuffer;
        if (!pDosHeader || pDosHeader->e_magic != 0x5a4d || (size_t)pDosHeader->e_lfanew + 512 >= exeSize)
            goto cdf_end;
        PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)exeBuffer + pDosHeader->e_lfanew);
        if (!pNtHeader || pNtHeader->Signature != 0x00004550)
            goto cdf_end;
        PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeader);
        if (!pSection)
            goto cdf_end;

        if (pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress != 0 && pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size != 0) {
            auto pExportSectionVA = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
            for (DWORD i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++, pSection++) {
                if (pSection->VirtualAddress <= pExportSectionVA && pSection->VirtualAddress + pSection->SizeOfRawData > pExportSectionVA) {
                    auto pSectionBase = (DWORD)exeBuffer - pSection->VirtualAddress + pSection->PointerToRawData;
                    PIMAGE_EXPORT_DIRECTORY pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(pSectionBase + pExportSectionVA);
                    char** pExportNames = (char**)(pSectionBase + pExportDirectory->AddressOfNames);
                    for (DWORD i = 0; i < pExportDirectory->NumberOfNames; ++i) {
                        auto pFunctionName = (char*)(pSectionBase + pExportNames[i]);
                        if (!strcmp(pFunctionName, funcName)) {
                            result = true;
                            goto cdf_end;
                        }
                    }
                }
            }
        }

    cdf_end:
        if (pFileMapView)
            UnmapViewOfFile(pFileMapView);
        if (hFileMap)
            CloseHandle(hFileMap);
        if (hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
        return result;

#undef MakePointer
    }
    static DWORD WINAPI CheckAndLoadVPatch(HANDLE hProcess, std::wstring& dir, const wchar_t* vpatchName)
    {
        auto vpatchPath = dir + vpatchName;
        if (CheckDLLFunction(vpatchPath, "_Initialize@4")) {
            auto vpNameLength = (vpatchPath.size() + 1) * sizeof(wchar_t);
            auto pLoadLibrary = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryW");
            auto remoteStr = VirtualAllocEx(hProcess, NULL, vpNameLength, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            WriteProcessMemory(hProcess, remoteStr, vpatchPath.data(), vpNameLength, NULL);
            auto t = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, remoteStr, 0, NULL);
            WaitForSingleObject(t, INFINITE);
            VirtualFreeEx(hProcess, remoteStr, 0, MEM_RELEASE);
            return 1;
        }

        return 0;
    }
    static std::wstring WINAPI GetThcrapGamePath(const char* idStr)
    {
        void* jsBuffer;
        size_t jsSize;
        auto gameCfgPath = THGameGui::singleton().mThcrapCfgDir+ L"\\games.js";
        if (LoadJsonFile(gameCfgPath, jsBuffer, jsSize)) {
            rapidjson::Document json;
            if (!(json.Parse((const char*)jsBuffer, jsSize + 1).HasParseError()) && json.IsObject()) {
                for (auto it = json.MemberBegin(); it != json.MemberEnd(); ++it) {
                    if (it->value.IsString() && !strcmp(it->name.GetString(), idStr)) {
                        free(jsBuffer);
                        return std::wstring(utf8_to_utf16(it->value.GetString()));
                    }
                }
            }
            free(jsBuffer);
        }

        return std::wstring();
    }
    static HANDLE WINAPI LaunchGameDirectly(THGame* currentGame, int instance)
    {
        bool result = true;
        //auto currentGame = THGameGui::singleton().mCurrentGame;
        if (!currentGame) {
            return 0;
        }
        auto& currentInst = currentGame->instances[instance];
        auto currentInstPath = utf8_to_utf16(currentInst.path);
        auto currentInstDir = GetDirFromFullPath(currentInstPath);

        STARTUPINFOW startup_info;
        PROCESS_INFORMATION proc_info;
        memset(&startup_info, 0, sizeof(STARTUPINFOW));
        startup_info.cb = sizeof(STARTUPINFOW);
        CreateProcessW(currentInstPath.c_str(), NULL, NULL, NULL, NULL, CREATE_SUSPENDED, NULL, currentInstDir.c_str(), &startup_info, &proc_info);

        if (currentInst.useVpatch) {
            auto exeName = GetNameFromFullPath(currentInstPath);
            if (exeName == L"東方紅魔郷.exe") {
                if (!CheckAndLoadVPatch(proc_info.hProcess, currentInstDir, L"vpatch_th06_unicode.dll")) {
                    CheckAndLoadVPatch(proc_info.hProcess, currentInstDir, L"vpatch_th06.dll");
                }
            } else {
                if (currentGame->signature.vPatchStr) {
                    CheckAndLoadVPatch(proc_info.hProcess, currentInstDir, currentGame->signature.vPatchStr);
                }
            }
        }
        if (currentInst.useTHPrac) {
            result = (WriteTHPracSig(proc_info.hProcess) && LocalApplyTHPrac(proc_info.hProcess));
        }

        if (!result) {
            TerminateThread(proc_info.hThread, -1);
        } else {
            ResumeThread(proc_info.hThread);
        }

        CloseHandle(proc_info.hThread);
        if (result) {
            return proc_info.hProcess;
        } else {
            CloseHandle(proc_info.hProcess);
            return 0;
        }
    }
    static DWORD WINAPI LaunchThreadFunc(_In_ LPVOID lpParameter)
    {
        int returnReuslt = 0;

        auto currentGame = THGameGui::singleton().mCurrentGame;
        if (!currentGame) {
            return 0;
        }
        auto& currentInst = currentGame->instances[currentGame->selected];
        auto currentCatagory = currentGame->signature.catagory;
        auto currentInstPath = utf8_to_utf16(currentInst.path);
        auto currentInstExePath = currentInstPath;
        auto currentInstDir = GetDirFromFullPath(currentInstPath);
        HINSTANCE executeResult = (HINSTANCE)100;
        bool useReflectiveLaunch = false;
        LauncherSettingGet("reflective_launch", useReflectiveLaunch);

        switch (currentInst.type) {
        case TYPE_ORIGINAL:
        case TYPE_MODDED:
        case TYPE_MALICIOUS:
        case TYPE_SCHINESE:
        case TYPE_TCHINESE:
            if (currentCatagory == CAT_MAIN || currentCatagory == CAT_SPINOFF_STG) {
                if (useReflectiveLaunch) {
                    if (currentGame->signature.vPatchStr && currentInst.useVpatch) {
                        executeResult = ShellExecuteW(NULL, L"open", (currentInstDir + L"vpatch.exe").c_str(), NULL, currentInstDir.c_str(), SW_SHOW);
                    } else {
                        executeResult = ShellExecuteW(NULL, L"open", currentInstPath.c_str(), NULL, currentInstDir.c_str(), SW_SHOW);
                    }
                    currentInstExePath = currentInstPath.c_str();
                    currentInstExePath = GetUnifiedPath(currentInstExePath);
                } else {
                    auto handle = LaunchGameDirectly(THGameGui::singleton().mCurrentGame, THGameGui::singleton().mCurrentGame->selected);
                    if (handle) {
                        CloseHandle(handle);
                        return 1;
                    }
                }
            } else {
                executeResult = ShellExecuteW(NULL, L"open", currentInstPath.c_str(), NULL, currentInstDir.c_str(), SW_SHOW);
            }
            break;
        case TYPE_NYASAMA:
            executeResult = ShellExecuteW(NULL, L"open", currentInstPath.c_str(), NULL, currentInstDir.c_str(), SW_SHOW);
            currentInstExePath = currentInstDir + utf8_to_utf16(currentGame->signature.idStr) + L".exe";
            currentInstExePath = GetUnifiedPath(currentInstExePath);
            break;
        case TYPE_THCRAP:
            executeResult = thcrapLaunchGame(currentInstPath, currentGame->signature.idStr);
            currentInstExePath = L"";
            break;
        case TYPE_STEAM: {
            if (!currentGame->signature.steamId) {
                return 0;
            }
            std::wstring steamURL = L"steam://rungameid/";
            steamURL += currentGame->signature.steamId;
            ShellExecuteW(NULL, L"open", L"steam://open/games", NULL, NULL, SW_SHOW);
            ShellExecuteW(NULL, L"open", steamURL.c_str(), NULL, NULL, SW_SHOW);
            currentInstExePath = GetUnifiedPath(currentInstExePath);
            executeResult = (HINSTANCE)64;
        }   
            break;
        default:
            executeResult = ShellExecuteW(NULL, L"open", currentInstPath.c_str(), NULL, currentInstDir.c_str(), SW_SHOW);
            break;
        }

        if (executeResult <= (HINSTANCE)32) {
            return 0;
        }

        if ((currentCatagory == CAT_MAIN || currentCatagory == CAT_SPINOFF_STG) && currentInst.useTHPrac) {
            return WaitAndApplyTHPrac(currentInstExePath);
        }

        return 1;
    }
    bool IsTHPracApplicable(THGameType type)
    {
        switch (type) {
        case THPrac::TYPE_ORIGINAL:
        //case THPrac::TYPE_MODDED:
        case THPrac::TYPE_THCRAP:
        //case THPrac::TYPE_SCHINESE:
        //case THPrac::TYPE_TCHINESE:
        case THPrac::TYPE_NYASAMA:
        case THPrac::TYPE_STEAM:
            return true;
        default:
            return false;
        }
    }
    bool TryLaunch(std::wstring& path)
    {
        auto attr = GetFileAttributesW(path.c_str());
        auto dir = GetDirFromFullPath(path);
        if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
            if (ShellExecuteW(NULL, L"open", path.c_str(), NULL, dir.c_str(), SW_SHOW) > (HINSTANCE)32) {
                return true;
            }
        }
        return false;
    }
    bool LaunchCustom(THGameInst& game, const char* idStr)
    {
        auto dirU16 = utf8_to_utf16(GetDirFromFullPath(game.path));

        switch (game.type) {
        case TYPE_ORIGINAL:
        case TYPE_MODDED:
        case TYPE_UNCERTAIN:
        case TYPE_MALICIOUS:
        case TYPE_UNKNOWN:
            if (TryLaunch(dirU16 + L"custom.exe"))
                return true;
            break;
        case TYPE_TCHINESE:
            if (TryLaunch(dirU16 + L"custom_cht.exe"))
                return true;
        case TYPE_CHINESE:
        case TYPE_SCHINESE:
        case TYPE_NYASAMA:
            if (TryLaunch(dirU16 + L"custom_cn.exe"))
                return true;
            if (TryLaunch(dirU16 + L"custom_c.exe"))
                return true;
            if (TryLaunch(dirU16 + L"custom_chs.exe"))
                return true;
            if (TryLaunch(dirU16 + L"custom_cht.exe"))
                return true;
            if (TryLaunch(dirU16 + L"custom.exe"))
                return true;
            break;
        case TYPE_STEAM:
            if (TryLaunch(utf8_to_utf16(GetDirFromFullPath(game.path)) + L"\\custom.exe"))
                return true;
            break;
            // TODO: Launch steam
        case TYPE_THCRAP: {
            if ((int)thcrapLaunchGame(dirU16, idStr, "_custom") > 32) {
                return true;
            }
        } break;
        default:
            break;
        }

        return false;
    }
    void GameLaunchModalOpen()
    {
        ImGui::OpenPopup("Launch##@__launch_t");
        mLaunchFailed = false;
        mLaunchThread.Start();
        mLaunchModalTimeout = 0.5f;
    }
    void GameLaunchModal()
    {
        if (GuiModalFullScreen("Launch##@__launch_t")) {
            if (mLaunchModalTimeout <= 0.0f && !mLaunchThread.IsActive()) {
                mLaunchFailed = mLaunchThread.GetExitCode() == 0;
                mLaunchThread.Stop();
                if (!mLaunchFailed) {
                    mLaunchAnm.Reset();
                    ImGui::CloseCurrentPopup();
                    int action = 0;
                    LauncherSettingGet("after_launch", action);
                    switch (action) {
                    case 1:
                        GuiLauncherMainTrigger(LAUNCHER_CLOSE);
                        break;
                    case 2:
                        break;
                    default:
                        GuiLauncherMainTrigger(LAUNCHER_MINIMIZE);
                        break;
                    }
                }
            }
            mLaunchModalTimeout -= ImGui::GetIO().DeltaTime;
            GuiSetPosYRel(0.5f);
            if (!mLaunchFailed) {
                GuiSetPosYRel(0.5f);
                GuiSetPosXText(XSTR(THPRAC_GAMES_LAUNCHING));
                ImGui::TextWrapped(XSTR(THPRAC_GAMES_LAUNCHING));
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::Text(mLaunchAnm.Get().c_str());
                if (GuiButtonTxtCentered(XSTR(THPRAC_CANCEL), 0.8f)) {
                    mLaunchAbortInd = true;
                    mLaunchThread.Wait();
                    mLaunchThread.Stop();
                    mLaunchAbortInd = false;
                    mLaunchAnm.Reset();
                    ImGui::CloseCurrentPopup();
                }
            } else {
                GuiCenteredText(XSTR(THPRAC_GAMES_LAUNCH_ERR));
                if (GuiButtonTxtCentered(XSTR(THPRAC_CLOSE), 0.8f)) {
                    mLaunchThread.Stop();
                    mLaunchAnm.Reset();
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
    }
    void GameVersionTable()
    {
        static int moveIdx = -1;

        if (!mCurrentGame) {
            return;
        }
        auto& currentGame = mCurrentGame->instances;
        auto& currentInstIdx = mCurrentGame->selected;
        auto currentCatagory = mCurrentGame->signature.catagory;
        int sourceIdx = -1;
        int destIdx = -1;

        float offset[] = {
            0.0f, 10.0f, 15.0f
        };
        ImGui::Text(XSTR(THPRAC_GAMES_SELECT_VER));
        ImGui::BeginChild("##@_game_version", ImVec2(0, ImGui::GetWindowHeight() * 0.35f), true);
        if (mNewGameWnd) {
            mNewGameWnd = false;
            ImGui::SetScrollX(0.0f);
            ImGui::SetScrollY(0.0f);
        }
        ImGui::Columns(3, 0, true);
        for (int i = 0; i < 3; ++i) {
            ImGui::SetColumnOffset(i, offset[i] * ImGui::GetFontSize());
        }
        ImGui::Separator();

        int i = 0;
        for (auto& game : mCurrentGame->instances) {
            std::string selId = "##__version_";
            selId += game.path;

            if (ImGui::Selectable(selId.c_str(), currentInstIdx == i, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, 0), false, true)) {
                currentInstIdx = i;
            }
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                moveIdx = i;
                ImGui::SetDragDropPayload("##@__dnd_gameinst", &(moveIdx), sizeof(moveIdx));
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##@__dnd_gameinst")) {
                    sourceIdx = *(decltype(mCurrentGame->selected)*)payload->Data;
                    destIdx = i;
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::SameLine(0.0f, 0.0f);
            GuiColumnText(game.name.c_str());
            ImGui::NextColumn();
            GuiColumnText(XSTR(TH_TYPE_SELECT[game.type]));
            ImGui::NextColumn();
            GuiColumnText(game.path.c_str());
            ImGui::NextColumn();
            ImGui::Separator();
            ++i;
        }

        ImGui::Columns(1);
        ImGui::EndChild();
        auto& currentInst = currentGame[currentInstIdx];

        if (GuiButtonModal(XSTR(THPRAC_GAMES_RENAME), XSTR(THPRAC_GAMES_RENAME_MODAL))) {
            strcpy_s(mRename, currentInst.name.c_str());
        }
        if (GuiModal(XSTR(THPRAC_GAMES_RENAME_MODAL))) {
            ImGui::InputText("##@__rename_input", mRename, sizeof(mRename));
            if (GuiButtonYesNo(XSTR(THPRAC_OK), XSTR(THPRAC_CANCEL))) {
                currentInst.name = mRename;
                WriteGameCfg();
            }
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        if (GuiButtonAndModalYesNo(XSTR(THPRAC_GAMES_DELETE), XSTR(THPRAC_GAMES_DELETE_MODAL), XSTR(THPRAC_GAMES_DELETE_CONFIRM), 6.0f, XSTR(THPRAC_YES), XSTR(THPRAC_NO))) {
            currentGame.erase(currentGame.begin() + currentInstIdx);
            for (; currentInstIdx > 0 && currentInstIdx >= (int)currentGame.size(); currentInstIdx--)
                ;
            WriteGameCfg();
        }

        ImGui::SameLine();
        if (ImGui::Button(XSTR(THPRAC_GAMES_OPEN_FOLDER))) {
            auto folderPath = GetDirFromFullPath(currentInst.path);
            if (folderPath != currentInst.path) {
                ShellExecuteW(NULL, L"explore", utf8_to_utf16(folderPath).c_str(), NULL, NULL, SW_SHOW);
            }
        }

        ImGui::SameLine();
        if (mCurrentGame->signature.appdataStr) {
            if (ImGui::Button(XSTR(THPRAC_GAMES_OPEN_APPDATA))) {
                std::string dataFolder;
                char* appdata = (char*)malloc(1000);
                GetEnvironmentVariableA("APPDATA", appdata, 1000);
                dataFolder = appdata;
                free(appdata);
                dataFolder += "\\ShanghaiAlice\\";
                dataFolder += mCurrentGame->signature.idStr;
                ShellExecuteA(NULL, "explore", dataFolder.c_str(), NULL, NULL, SW_SHOW);
            }
        }

        ImGui::SameLine();
        auto cat = mCurrentGame->signature.catagory;
        if (cat == CAT_MAIN || cat == CAT_SPINOFF_STG) {
            if (ImGui::Button(XSTR(THPRAC_GAMES_LAUNCH_CUSTOM))) {
                if (!LaunchCustom(currentInst, mCurrentGame->signature.idStr)) {
                    mCustomErrTxtTime = 2.0f;
                }
            }
            if (mCustomErrTxtTime > 0.0f) {
                mCustomErrTxtTime -= ImGui::GetIO().DeltaTime;
                ImGui::SameLine();
                ImGui::Text(XSTR(THPRAC_GAMES_LAUNCH_CUSTOM_ERR));
            }
        }

        ImGui::NewLine();
        if (currentCatagory == CAT_MAIN || currentCatagory == CAT_SPINOFF_STG) {
            switch (currentInst.type) {
            case TYPE_ORIGINAL:
                if (mCurrentGame->signature.vPatchStr) {
                    ImGui::Checkbox(XSTR(THPRAC_GAMES_USE_VPATCH), &currentInst.useVpatch);
                    ImGui::SameLine();
                }
                if (ImGui::Checkbox(XSTR(THPRAC_GAMES_APPLY_THPRAC), &currentInst.useTHPrac)) {
                    WriteGameCfg();
                }
                break;
            case TYPE_THCRAP:
            case TYPE_STEAM:
            case TYPE_NYASAMA:
                if (ImGui::Checkbox(XSTR(THPRAC_GAMES_APPLY_THPRAC), &currentInst.useTHPrac)) {
                    WriteGameCfg();
                }
                break;
            case TYPE_MODDED:
            case TYPE_MALICIOUS:
            case TYPE_SCHINESE:
            case TYPE_TCHINESE:
                if (mCurrentGame->signature.vPatchStr) {
                    ImGui::Checkbox(XSTR(THPRAC_GAMES_USE_VPATCH), &currentInst.useVpatch);
                    ImGui::SameLine();
                }
                if (ImGui::Checkbox(XSTR(THPRAC_GAMES_FORCE_THPRAC), &currentInst.useTHPrac)) {
                    if (currentInst.useTHPrac) {
                        currentInst.useTHPrac = false;
                        ImGui::OpenPopup(XSTR(THPRAC_GAMES_FORCE_THPRAC_MODAL));
                    } else {
                        WriteGameCfg();
                    }
                }
                if (GuiModal(XSTR(THPRAC_GAMES_FORCE_THPRAC_MODAL))) {
                    ImGui::PushTextWrapPos(ImGui::GetIO().DisplaySize.x * 0.9f);
                    ImGui::TextWrapped(XSTR(THPRAC_GAMES_FORCE_THPRAC_CONFIRM));
                    ImGui::PopTextWrapPos();
                    if (GuiButtonYesNo(XSTR(THPRAC_YES), XSTR(THPRAC_NO))) {
                        currentInst.useTHPrac = true;
                        WriteGameCfg();
                    }
                    ImGui::EndPopup();
                }

                break;
            default:
                break;
            }
        }

        bool setDefaultLaunch = mCurrentGame->defaultLaunch == currentInstIdx;
        if (ImGui::Checkbox(XSTR(THPRAC_GAMES_DEFAULT_LAUNCH), &setDefaultLaunch)) {
            if (setDefaultLaunch) {
                mCurrentGame->defaultLaunch = currentInstIdx;
            } else  {
                mCurrentGame->defaultLaunch = -1;
            }
            WriteGameCfg();
        }
        ImGui::SameLine();
        GuiHelpMarker(XSTR(THPRAC_GAMES_DEFAULT_LAUNCH_DESC));

        if (sourceIdx != -1 && destIdx != -1) {
            auto gameTmp = currentGame[sourceIdx];
            if (destIdx > sourceIdx) {
                currentGame.insert(currentGame.begin() + destIdx + 1, gameTmp);
                currentGame.erase(currentGame.begin() + sourceIdx);
            } else if (destIdx < sourceIdx) {
                currentGame.insert(currentGame.begin() + destIdx, gameTmp);
                currentGame.erase(currentGame.begin() + sourceIdx + 1);
            }
            if (mCurrentGame->defaultLaunch == currentInstIdx) {
                mCurrentGame->defaultLaunch = destIdx;
            }
            currentInstIdx = destIdx;
            WriteGameCfg();
        }

        auto needDisabled = currentInst.type != TYPE_ORIGINAL;
        if (needDisabled) {
            ImGui::BeginDisabled();
        }
        if (GuiButtonRelCentered(XSTR(THPRAC_GAMES_LAUNCH_GAME), 0.85f, ImVec2(1.0f, 0.1f))) {
            GameLaunchModalOpen();
        }
        if (needDisabled) {
            ImGui::EndDisabled();
        }
        GameLaunchModal();
    }
    void GuiGame()
    {
        if (ImGui::Button(XSTR(THPRAC_BACK))) {
            mGuiUpdFunc = [&]() { GuiMain(); };
        }
        ImGui::SameLine();
        GuiCenteredText(XSTR(mCurrentGame->signature.refStr));
        ImGui::Separator();

        if (!mCurrentGame || !mCurrentGame->instances.size()) {
            GuiSetPosYRel(0.5f);
            GuiCenteredText(XSTR(THPRAC_GAMES_MISSING));
        } else {
            GameVersionTable();
            ImGui::NewLine();
        }
    }

    bool GuiMainCtxMenu(int type, int* result = nullptr)
    {
        if (type == 0) {
            if (!ImGui::BeginPopupContextWindow()) {
                return false;
            }
        } else {
            if (!ImGui::BeginPopupContextItem()) {
                return false;
            }
        }
        if (type == 1) {
            if (ImGui::Selectable(XSTR(THPRAC_GAMES_DETAILS_PAGE))) {
                if (result) {
                    *result = 1;
                }
            }
            ImGui::Separator();
        } 

        if (ImGui::Selectable(XSTR(THPRAC_GAMES_SCAN_FOLDER))) {
            ScanClear();
            mGuiUpdFunc = [&]() { GuiScanFolder(); };
        }
        if (ImGui::Selectable(XSTR(THPRAC_STEAM_MNG_BUTTON))) {
            ScanClear();
            mGuiUpdFunc = [&]() { GuiScanSteam(); };
        }
        ImGui::Separator();
        if (ImGui::Selectable(XSTR(THPRAC_GAMES_RESCAN))) {
            mNeedRescan = true;
        }
        ImGui::EndPopup();

        return true;
    }
    void SwitchToGame(const char* idStr)
    {
        for (auto it = mGames.begin(); it != mGames.end(); ++it) {
            auto& game = it.value();
            if (!strcmp(game.signature.idStr, idStr)) {
                mCurrentGame = &game;
                mNewGameWnd = true;
                mGuiUpdFunc = [&]() { GuiGame(); };
                return;
            }
        }

        mGuiUpdFunc = [&]() { GuiMain(); };
    }
    bool SelectableWrapped(const char* label, bool disabled, bool selected)
    {
        bool result = false;

        result = ImGui::Selectable(label, selected, disabled ? ImGuiSelectableFlags_Disabled : 0, ImVec2(0, 0), true);

        return result;
    }
    void GameTable(const char* title, THGameCatagory catagory)
    {
        int columns = 1;
        int i = 0;

        ImGui::Text(title);
        ImGui::Columns(columns);
        for (auto& gameRef : gGameDefs) {
            if (gameRef.catagory == catagory) {
                auto& game = mGames[gameRef.idStr];
                if (!(i % columns)) {
                    ImGui::Separator();
                }

                auto disabled = !game.instances.size();
                if (SelectableWrapped(XSTR(game.signature.refStr), disabled, mSelectedGameTmp == &game)) {
                    bool autoDefaultLaunch = false;
                    LauncherSettingGet("auto_default_launch", autoDefaultLaunch);
                    auto autoLaunch = game.defaultLaunch;
                    if (autoLaunch < 0 && autoDefaultLaunch) {
                        autoLaunch = 0;
                    }
                    if (autoLaunch >= 0 && autoLaunch < (int)game.instances.size()) {
                        mCurrentGame = &game;
                        mCurrentGame->selected = autoLaunch;
                        GameLaunchModalOpen();
                    } else {
                        mCurrentGame = &game;
                        mNewGameWnd = true;
                        mGuiUpdFunc = [&]() { GuiGame(); };
                    }
                }
                if (!disabled) {
                    int result = 0;
                    if (GuiMainCtxMenu(1, &result)) {
                        mSelectedGameTmp = &game;
                    } else if (mSelectedGameTmp == &game) {
                        mSelectedGameTmp = nullptr;
                    }
                    if (result) {
                        mCurrentGame = &game;
                        mNewGameWnd = true;
                        mGuiUpdFunc = [&]() { GuiGame(); };
                    }
                } else if (game.signature.steamId && ImGui::BeginPopupContextItem(NULL, ImGuiPopupFlags_MouseButtonLeft)) {
                    if (ImGui::Selectable(XSTR(THPRAC_GOTO_STEAM_PAGE))) {
                        std::wstring steamURL { L"https://store.steampowered.com/app/" };
                        steamURL += game.signature.steamId;
                        ShellExecuteW(NULL, L"open", steamURL.c_str(), NULL, NULL, SW_SHOW);
                    }
                    ImGui::EndPopup();
                }

                ImGui::NextColumn();
                i++;
            }
        }
        ImGui::Columns(1);
        ImGui::Separator();
    }
    void GuiMain()
    {
        mCurrentGame = nullptr;
        mCustomErrTxtTime = 0.0f;
        mNeedRescan = false;

        GuiMainCtxMenu(0);

        GameTable(XSTR(THPRAC_GAMES_MAIN_SERIES), CAT_MAIN);
        ImGui::NewLine();
        GameTable(XSTR(THPRAC_GAMES_SPINOFF_STG), CAT_SPINOFF_STG);
        ImGui::NewLine();
        GameTable(XSTR(THPRAC_GAMES_SPINOFF_OTHERS), CAT_SPINOFF_OTHERS);

        if (mNeedRescan) {
            ImGui::OpenPopup("rescan##@__rescan");
            mRescanModalTimeout = 1.0f;
            ScanClear();
            for (auto& opt : mScanOption) {
                opt = true;
            }
            mRescanThread.Start();
        }
        if (GuiModalFullScreen("rescan##@__rescan")) {
            if (mRescanModalTimeout <= 0.0f && !mRescanThread.IsActive()) {
                mRescanThread.Stop();
                ScanClear();
                ImGui::CloseCurrentPopup();
            }
            mRescanModalTimeout -= ImGui::GetIO().DeltaTime;
            GuiSetPosYRel(0.5f);
            GuiSetPosXText(XSTR(THPRAC_GAMES_RESCANNING));
            ImGui::TextWrapped(XSTR(THPRAC_GAMES_RESCANNING));
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text(mScanAnm.Get().c_str());
            GuiCenteredText(mScanCurrent[mScanCurrentIdx].c_str());
            ImGui::EndPopup();
        }

        GameLaunchModal();
    }
    void GuiUpdate()
    {
        mGuiUpdFunc();
    }

private:
    std::function<void(void)> mGuiUpdFunc = []() {};
    tsl::robin_map<std::string, THGame> mGames;
    THGame* mCurrentGame = nullptr;
    bool mNewGameWnd = false;
    char mRename[256];
    float mCustomErrTxtTime = 0.0f;
    GuiThread mLaunchThread { THGameGui::LaunchThreadFunc };
    float mLaunchModalTimeout = 0;
    bool mLaunchFailed = false;
    bool mLaunchAbortInd = false;
    GuiWaitingAnm mLaunchAnm;
    bool mNeedRescan = false;
    THGame* mSelectedGameTmp = nullptr;

    std::wstring mThcrapDir = L"";
    std::wstring mThcrapCfgDir = L"";
    std::vector<std::string> mThcrapCfg;
    std::vector<std::string> mThcrapGames;

    // Scanning
    float mRescanModalTimeout = 0;
    GuiThread mScanThread { THGameGui::ScanThreadFunc };
    GuiThread mRescanThread { THGameGui::RescanThreadFunc };
    int mScanStatus = 0;
    bool mScanOption[4];
    GuiWaitingAnm mScanAnm;
    std::wstring mScanPath = L"";
    std::vector<THGameScan> mGameScan[GAME_SCAN_TYPE_CNT];
    bool mGameScanScrollReset[GAME_SCAN_TYPE_CNT] { true, true, true, true, true, true };
    bool mGameScanSelect[GAME_SCAN_TYPE_CNT] { false, true, true, true, false, false };
    std::string mScanCurrent[2];
    int mScanCurrentIdx = 0;
    int mSteamMenuStatus = 0;
    std::vector<GameRoll> mSteamGames[4];
    bool mSteamGameTypeOpt[4];
};

bool LauncherGamesGuiUpd()
{
    THGameGui::singleton().GuiUpdate();
    return true;
}

void LauncherGamesGuiSwitch(const char* idStr)
{
    THGameGui::singleton().SwitchToGame(idStr);
}

void LauncherGamesForceReload()
{
    THGameGui::singleton().ForceReload();
}

bool LauncherGamesThcrapTest(std::wstring& dir)
{
    return THGameGui::singleton().thcrapTest(dir);
}

bool LauncherGamesThcrapSetup()
{
    return THGameGui::singleton().thcrapSetup();
}

void LauncherGamesThcrapCfgGet(std::vector<std::pair<std::string, bool>>& cfgVec, std::vector<GameRoll> gameVec[4])
{
    THGameGui::singleton().thcrapCfgGet(cfgVec, gameVec);
}

void LauncherGamesThcrapAdd(const char* gameId, std::string& cfg, bool use_thprac, bool flush)
{
    THGameGui::singleton().thcrapAdd(gameId, cfg, use_thprac, flush);
}

bool LauncherGamesThcrapLaunch()
{
    return THGameGui::singleton().thcrapLaunch();
}
}