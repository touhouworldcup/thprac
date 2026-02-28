#include "thprac_games.h"
#include "thprac_utils.h"
#include <format>
#include <math.h>

#include "utils/wininternal.h"
#include <fstream>

#pragma warning(disable : 4706)
// WTF Microsoft
#undef hyper

namespace THPrac {
    bool g_show_att_hitbox = false;
namespace TH20 {
    using std::pair;
    constexpr int32_t stoneGaugeInitialValue[] = { 1100, 1200, 1300, 1400, 1400, 1400, 1400 }; 
    
    class TH20Save {
        SINGLETON(TH20Save)
    public:
        enum TH20SpellState {
            Capture = 0,
            Attempt = 1,
            Timeout = 2
        };
        const int cSaveVersion = 1;
        const char* cSaveFile = "score20pyra.dat";
        struct PyraState
        {
            DWORD lastPyraAddr = 0;
            bool isInPyra = false;
            int lastPyraSpellId = 0;
            bool isPyraFailed = false;
            bool isPyraTimeout = false;
            void Reset()
            {
                lastPyraAddr = 0;
                isInPyra = false;
                lastPyraSpellId = 0;
                isPyraFailed = false;
                isPyraTimeout = false;
            }
        }pyraState;
    static int32_t GetPyraSpellId(int stage, int is_stage4_mid_1,int stone1)
    {
        if (is_stage4_mid_1)
            return 0;
        if (stage == 4)
            if (stone1 >= 0 && stone1 < 8)//stone1 = 01234567
                return stone1 / 2 + 1; //1 2 3 4
        return 5;
    }
    static int32_t GetPlayerType(int main_player_type, int stone1)
    {
        return main_player_type * 8 + stone1;
    }
    public:
        struct Save {
            int32_t ScHistory[6][5][16][3] = { 0 }; // (st4 0 A B C D /st5 pyra), diff, playertype, [capture/attempt/timeout]
            int32_t ScHistoryPrac[6][5][16][3] = { 0 };
        };
        Save saveCurrent;
        Save saveTotal;
        bool isSaveLoaded;
        TH20Save()
        {
            saveCurrent = { 0 };
            saveTotal = { 0 };
            isSaveLoaded = false;
            LoadSave();
        }

    public:
        void LoadSave()
        {
            if (isSaveLoaded)
                return;
            PushCurrentDirectory(L"%appdata%\\ShanghaiAlice\\th20");
            auto fs_new = ::std::fstream(cSaveFile, ::std::ios::in | ::std::ios::binary);
            if (fs_new.is_open()) {
                isSaveLoaded = true;
                int version = 1;
                fs_new.read((char*)&version, sizeof(version));
                switch (version) {
                default:
                case 1:
                    fs_new.read((char*)(&saveTotal), sizeof(saveTotal));
                    break;
                }
                fs_new.close();
            }
            PopCurrentDirectory();
        }

        void SaveSave()
        {
            PushCurrentDirectory(L"%appdata%\\ShanghaiAlice\\th20");
            auto fs = ::std::fstream(cSaveFile, ::std::ios::out | ::std::ios::binary);
            if (fs.is_open()) {
                fs.write((char*)(&cSaveVersion), sizeof(cSaveVersion));
                fs.write((char*)(&saveTotal), sizeof(saveTotal));
                fs.close();
            }
            PopCurrentDirectory();
        }

         void AddTimeout(int spell_id, byte diff, byte player_type,bool isPrac)
        {
            LoadSave();
            if (isPrac)
            {
                saveTotal.ScHistoryPrac[spell_id][diff][player_type][2]++;
                saveCurrent.ScHistoryPrac[spell_id][diff][player_type][2]++;
            }
            else
            {
                saveTotal.ScHistory[spell_id][diff][player_type][2]++;
                saveCurrent.ScHistory[spell_id][diff][player_type][2]++;
            }
            SaveSave();
        }

        void AddAttempt(int spell_id, int diff, int player_type, bool isPrac)
        {
            LoadSave();
            if (isPrac) {
                saveTotal.ScHistoryPrac[spell_id][diff][player_type][1]++;
                saveCurrent.ScHistoryPrac[spell_id][diff][player_type][1]++;
            } else {
                saveTotal.ScHistory[spell_id][diff][player_type][1]++;
                saveCurrent.ScHistory[spell_id][diff][player_type][1]++;
            }
            SaveSave();
        }

        void AddCapture(int spell_id, int diff, int player_type, bool isPrac)
        {
            LoadSave();
            if (isPrac) {
                saveTotal.ScHistoryPrac[spell_id][diff][player_type][0]++;
                saveCurrent.ScHistoryPrac[spell_id][diff][player_type][0]++;
            } else {
                saveTotal.ScHistory[spell_id][diff][player_type][0]++;
                saveCurrent.ScHistory[spell_id][diff][player_type][0]++;
            }
            SaveSave();
        }

        int GetTotalSpellCardCount(int spell_id, int diff, int player_type, TH20SpellState state)
        {
            return saveTotal.ScHistory[spell_id][diff][player_type][state];
        }
        int GetCurrentSpellCardCount(int spell_id, int diff, int player_type, TH20SpellState state)
        {
            return saveCurrent.ScHistory[spell_id][diff][player_type][state];
        }

        void ShowDetail()
        {
            static bool is_comb = false;
            static bool is_prac = false;
            static bool is_total = true;
            static int32_t schistory_tot[6][5][16][3]; 
            static int32_t schistory_cur[6][5][16][3]; 

            const char* players_strs[2] = { S(THPRAC_IGI_PL_Reimu), S(THPRAC_IGI_PL_Marisa) };
            const char* spells_str[6] = {
                S(THPRAC_INGAMEINFO_20_PYRAMID_ST4_MID1),
                S(THPRAC_INGAMEINFO_20_PYRAMID_ST4_MID2R),
                S(THPRAC_INGAMEINFO_20_PYRAMID_ST4_MID2B),
                S(THPRAC_INGAMEINFO_20_PYRAMID_ST4_MID2Y),
                S(THPRAC_INGAMEINFO_20_PYRAMID_ST4_MID2G),
                S(THPRAC_INGAMEINFO_20_PYRAMID_ST5_MID1),
            };
            const ImVec4 r = { 1.0f, 0.5f, 0.5f, 1.0f };
            const ImVec4 g = { 0.5f, 1.0f, 0.5f, 1.0f };
            const ImVec4 b = { 0.5f, 0.75f, 1.0f, 1.0f };
            const ImVec4 y = { 1.0f, 1.0f, 0.3f, 1.0f };
            const ImVec4 w = { 1.0f, 1.0f, 1.0f, 1.0f };
            const ImVec4 p = { 1.0f, 0.5f, 1.0f, 1.0f };
            const ImVec4 rk = { 0.9f, 0.2f, 0.2f, 1.0f };
            const ImVec4 gk = { 0.2f, 0.7f, 0.2f, 1.0f };
            const ImVec4 bk = { 0.1f, 0.5f, 1.0f, 1.0f };
            const ImVec4 yk = { 0.8f, 0.8f, 0.2f, 1.0f };
            const ImVec4 spells_colors[] = { p, r, b, y, g, w };
            const ImVec4 stones_colors[] = { r, rk, b, bk, y, yk, g, gk, w };

            if (ImGui::Checkbox(S(THPRAC_INGAMEINFO_20_TOTAL_CAPTURE_RATE), &is_total)) {
                is_comb = is_prac = false;
            }
            ImGui::SameLine();
            if (ImGui::Checkbox(S(THPRAC_INGAMEINFO_20_COMBAT_CAPTURE_RATE), &is_comb))
            {
                is_prac = is_total = false;
            }
            ImGui::SameLine();
            if (ImGui::Checkbox(S(THPRAC_INGAMEINFO_20_PRAC_CAPTURE_RATE), &is_prac)) {
                is_comb = is_total = false;
            }
            for (int i = 0; i < 6; i++)
                for (int j = 0; j < 5; j++)
                    for (int k = 0; k < 16; k++)
                        for (int l = 0; l < 3; l++) {
                            if (is_comb)
                            {
                                schistory_tot[i][j][k][l] = saveTotal.ScHistory[i][j][k][l];
                                schistory_cur[i][j][k][l] = saveCurrent.ScHistory[i][j][k][l];
                            } else if (is_prac)
                            {
                                schistory_tot[i][j][k][l] = saveTotal.ScHistoryPrac[i][j][k][l];
                                schistory_cur[i][j][k][l] = saveCurrent.ScHistoryPrac[i][j][k][l];
                            }
                            else
                            {
                                schistory_tot[i][j][k][l] = saveTotal.ScHistory[i][j][k][l] + saveTotal.ScHistoryPrac[i][j][k][l];
                                schistory_cur[i][j][k][l] = saveCurrent.ScHistory[i][j][k][l] + saveCurrent.ScHistoryPrac[i][j][k][l];
                            }
                        }
            
            ImGui::BeginTabBar("Detail Spell");
            {
                const char* tabs_diff_strs[4] = { S(THPRAC_IGI_DIFF_E), S(THPRAC_IGI_DIFF_N), S(THPRAC_IGI_DIFF_H), S(THPRAC_IGI_DIFF_L)};
                for (int diff = 0; diff < 4; diff++) {
                    if (ImGui::BeginTabItem(tabs_diff_strs[diff])) {
                        ImGui::BeginTabBar("Player Type");
                        for (int pl = 0; pl < 2; pl++) {
                            if (ImGui::BeginTabItem(players_strs[pl])) {
                                // spell capture
                                ImGui::BeginTable(std::format("{}{}sptable", tabs_diff_strs[diff], players_strs[pl]).c_str(), 6, ImGuiTableFlags_::ImGuiTableFlags_Resizable);
                                ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_SPELL_NAME), 0, 45.0f);
                                ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_20_STONE_NAME), 0, 50.0f);
                                ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_20_PASS_TOT), 0, 30.0f);
                                ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_TIMEOUT_TOT), 0, 24.0f);
                                ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_20_PASS_CUR), 0, 30.0f);
                                ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_TIMEOUT_CUR), 0, 24.0f);
                                ImGui::TableHeadersRow();

                                for (int stone = 0; stone < 8; stone++)
                                {
                                    for (int spell = 0; spell < 6; spell++) {
                                        int plstone_type = GetPlayerType(pl, stone);
                                        if (schistory_tot[spell][diff][plstone_type][TH20Save::Attempt] == 0)
                                        {
                                            if ((spell >= 1 && spell <= 4) && (stone / 2 != spell - 1)) // mismatched spell/stone
                                                continue;
                                        }
                                        ImVec4 spell_color = spells_colors[spell];
                                        ImVec4 stone_color = stones_colors[stone];
                                        if (spell == 5 && g_adv_igi_options.th12_chromatic_ufo) // st5 pyra
                                        {
                                            static float t = 0;
                                            t += 0.003f;
                                            if (t >= 1.0f)
                                                t = 0.0f;
                                            float rr, gg, bb;
                                            ImGui::ColorConvertHSVtoRGB(fmodf(t, 1.0f), 0.4f, 1.0f, rr, gg, bb);
                                            spell_color = { rr, gg, bb, 1.0f };
                                            stone_color.x = rr * 0.6f + stone_color.x * 0.4f;
                                            stone_color.y = gg * 0.6f + stone_color.y * 0.4f;
                                            stone_color.z = bb * 0.6f + stone_color.z * 0.4f;
                                        }

                                        ImGui::TableNextRow();
                                        ImGui::TableNextColumn();

                                        ImGui::TextColored(spell_color, "%s", spells_str[spell]);
                                        ImGui::TableNextColumn();

                                        if (Gui::LocaleGet() == Gui::LOCALE_EN_US)
                                            ImGui::TextColored(stone_color, "%s\"%s\"", S(IGI_PL_20_SUB[stone]), S(IGI_PL_20_SUB_FULL[stone]));
                                        else
                                            ImGui::TextColored(stone_color, "%s「%s」", S(IGI_PL_20_SUB[stone]), S(IGI_PL_20_SUB_FULL[stone]));
                                        ImGui::TableNextColumn();
                                        
                                        // (st4 0 A B C D /st5 pyra), diff, playertype, [capture/attempt/timeout]
                                        ImGui::TextColored(stone_color, "%d/%d(%.1f%%)", 
                                            schistory_tot[spell][diff][plstone_type][TH20Save::Capture] + schistory_tot[spell][diff][plstone_type][TH20Save::Timeout],
                                            schistory_tot[spell][diff][plstone_type][TH20Save::Attempt],
                                            ((float)(schistory_tot[spell][diff][plstone_type][TH20Save::Capture] + schistory_tot[spell][diff][plstone_type][TH20Save::Timeout]) / std::fmaxf(1.0f, ((float)schistory_tot[spell][diff][plstone_type][TH20Save::Attempt])) * 100.0f));
                                        ImGui::TableNextColumn();

                                        ImGui::TextColored(stone_color, "%d", schistory_tot[spell][diff][plstone_type][TH20Save::Timeout]);
                                        ImGui::TableNextColumn();

                                        ImGui::TextColored(stone_color, "%d/%d(%.1f%%)",
                                            schistory_cur[spell][diff][plstone_type][TH20Save::Capture] + schistory_cur[spell][diff][plstone_type][TH20Save::Timeout],
                                            schistory_cur[spell][diff][plstone_type][TH20Save::Attempt],
                                            ((float)(schistory_cur[spell][diff][plstone_type][TH20Save::Capture] + schistory_cur[spell][diff][plstone_type][TH20Save::Timeout]) / std::fmaxf(1.0f, ((float)schistory_cur[spell][diff][plstone_type][TH20Save::Attempt])) * 100.0f));
                                        ImGui::TableNextColumn();

                                        ImGui::TextColored(stone_color, "%d", schistory_cur[spell][diff][plstone_type][TH20Save::Timeout]);
                                        ImGui::TableNextColumn();
                                    }
                                    ImGui::TableNextRow();
                                    ImGui::TableNextColumn();
                                    ImGui::NewLine();
                                }
                                ImGui::EndTable();
                                ImGui::EndTabItem();
                            }
                        }
                        ImGui::EndTabBar();
                        ImGui::EndTabItem();
                    }
                }
            }
            ImGui::EndTabBar();
        }
    };


    enum rel_addrs {
        DIFFICULTY = 0x1b0a60,
        WINDOW_PTR = 0x1b6758,
        GAME_SIDE0 = 0x1ba568,
        PLAYER_PTR = GAME_SIDE0 + 0x4,
        DMG_SRC_MGR_PTR = GAME_SIDE0 + 0x28,
        MODEFLAGS = GAME_SIDE0 + 0x6c,
        STAGE_NUM = GAME_SIDE0 + 0x88 + 0x1f4,
        GAME_THREAD_PTR = 0x1ba828,
        TRANSITION_STG_PTR = 0x1c06a0,
        REPLAY_MGR_PTR = 0x1c60fc,
        ENM_STONE_MGR_PTR = 0x1c6118,
        MAIN_MENU_PTR = 0x1c6124,
        SET_TIMER_FUNC = 0x23520,
        ADD_TIMER_FUNC = 0x297A0,
        ALLOCATE_DMG_SRC_FUNC = 0xc0e60,
        DELETE_DMG_SRC_FUNC = 0xc1f30,
    };

    struct AnmVM {
        char gap0[0x20];
        uint32_t sprite_id; // 0x20
        uint32_t script_id; // 0x24
        char gap1[0x48];
        Float2 sprite_dims; // 0x70
        char gap2[0x420];
        uint16_t anm_draw_mode; // 0x498, contains both render & blend mode
    };

     struct PlayerDamageSource {
        PlayerDamageSource* self_ptr; // 0x0
        PlayerDamageSource* list_next; // 0x4
        PlayerDamageSource* list_prev; // 0x8
        void* iterable_list; // 0xc
        void* cur_iterator; // 0x10
        uint32_t flags; // 0x14
        float radius; // 0x18
        float unknown_float_1c; // 0x1c
        float radius_delta; // 0x20
        float angle; // 0x24
        float angular_velocity; // 0x28
        Float2 size; // 0x2c
        Float2 position; // 0x34
        char otherMotionData[0x40]; // 0x3c
        Timer20 duration; // 0x7c
        uint32_t id; // 0x8c
        uint32_t player_bullet_id; // 0x90
        int32_t damage; // 0x94
        int32_t damage_dealt; // 0x98
        int32_t damage_cap; // 0x9c
        int32_t hit_freq; // 0xa0
        char variousUnknowns[0x18]; // 0xa4
        uint32_t side_index; // 0xbc
        void* game_side; // 0xc0
    };

        struct GlobalsSide {
        uint64_t score; // 0x0
        int32_t character; // 0x8
        int32_t story_stone; // 0xc
        int32_t narrow_shot_stone; // 0x10
        int32_t wide_shot_stone; // 0x14
        int32_t assist_stone; // 0x18
        int32_t story_stone_raw; // 0x1c
        int32_t narrow_shot_stone_raw; // 0x20
        int32_t wide_shot_stone_raw; // 0x24
        int32_t assist_stone_raw; // 0x28
        uint8_t story_stone_copy; // 0x2c
        uint8_t narrow_shot_stone_copy; // 0x2d
        uint8_t wide_shot_stone_copy; // 0x2e
        uint8_t assist_stone_copy; // 0x2f
        int32_t current_power; // 0x30
        int32_t max_power; // 0x34
        int32_t power_per_level; // 0x38
        int32_t __int_3C; // 0x3c
        int32_t piv_base; // 0x40
        int32_t piv; // 0x44
        int32_t piv_divisor; // 0x48
        int32_t hyper_meter; // 0x4c
        int32_t hyper_meter_max; // 0x50
        int32_t hyper_duration; // 0x54
        int32_t __dword_58; // 0x58
        int32_t summon_meter; // 0x5c
        int32_t summon_meter_max; // 0x60
        int32_t stone_priority_red; // 0x64
        int32_t stone_priority_blue; // 0x68
        int32_t stone_priority_yellow; // 0x6c
        int32_t stone_priority_green; // 0x70
        int32_t stone_level_red; // 0x74
        int32_t stone_level_blue; // 0x78
        int32_t stone_level_yellow; // 0x7c
        int32_t stone_level_green; // 0x80
        int32_t stone_enemy_count_red; // 0x84
        int32_t stone_enemy_count_blue; // 0x88
        int32_t stone_enemy_count_yellow; // 0x8c
        int32_t stone_enemy_count_green; // 0x90
        int32_t stone_enemy_count_total; // 0x94
        int32_t next_stone_enemy_type; // 0x98
        char unknowns_9c[0xc]; // 0x9c
        int32_t g2_passive_factor; // 0xa8
        char unknowns_ac[0xc]; // 0xac
        int32_t life_stocks; // 0xb8
        int32_t life_stock_max; // 0xbc
        int32_t life_fragments; // 0xc0
        int32_t lives_added; // 0xc4
        int32_t __int_C8; // 0xc8
        int32_t bomb_stocks; // 0xcc
        int32_t bomb_fragments; // 0xd0
        int32_t bomb_stocks_for_new_life; // 0xd4
        int32_t bomb_stock_max; // 0xd8
        int32_t __chapter_total; // 0xdc
        int32_t __chapter_destroy; // 0xe0
        int32_t graze; // 0xe4
        int32_t hitbox_scale; // 0xe8
        uint32_t side_index; // 0xec
    };


    // maps original resolution option id to the corresponding height of the default (sprite -1) anm sprite
    // used for Nina Sp4 desync fix
    float defaultSpriteHeights[4] = { 0.0f, 6.0f, 10.0f, 14.0f };

    struct THPracParam {
        int32_t mode;
        int32_t stage;
        int32_t section;
        int32_t phase;

        int64_t score;
        int32_t life;
        int32_t life_fragment;
        int32_t bomb;
        int32_t bomb_fragment;
        int32_t cycle;
        int32_t power;
        int32_t value;

        float hyper;
        float stone;
        bool hyperActive;
        bool stoneActive;

        int32_t stoneMax;
        int32_t levelR;
        int32_t priorityR;
        int32_t levelB;
        int32_t priorityB;
        int32_t levelY;
        int32_t priorityY;
        int32_t levelG;
        int32_t priorityG;

        int32_t reimuR2Timer[7];
        int32_t passiveMeterTimer[7];
        int32_t yellow2CycleAngle[7][4];
        int32_t yellow2CycleTimer[7][4];
        float resolutionSpriteHeight;
        std::vector<PlayerDamageSource> rogueDmgSrcs[7];
        uint32_t nextDmgID[7];
        std::vector<uint32_t> expStoneColors[7];
        bool dlg;

        bool _playLock = false;
        void Reset()
        {
            for (size_t st = 0; st < elementsof(rogueDmgSrcs); ++st)
                rogueDmgSrcs[st].clear();

            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th20");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValueEx(dlg, Bool);

            GetJsonValue(score);
            GetJsonValue(life);
            GetJsonValue(life_fragment);
            GetJsonValue(bomb);
            GetJsonValue(bomb_fragment);
            GetJsonValue(cycle);
            GetJsonValue(power);
            GetJsonValue(value);

            GetJsonValue(hyper);
            GetJsonValue(stone);
            GetJsonValueEx(hyperActive, Bool);
            GetJsonValueEx(stoneActive, Bool);
            GetJsonValue(stoneMax);
            GetJsonValue(levelR);
            GetJsonValue(priorityR);
            GetJsonValue(levelB);
            GetJsonValue(priorityB);
            GetJsonValue(levelY);
            GetJsonValue(priorityY);
            GetJsonValue(levelG);
            GetJsonValue(priorityG);
            GetJsonArray(reimuR2Timer, elementsof(reimuR2Timer));
            GetJsonArray(passiveMeterTimer, elementsof(passiveMeterTimer));
            GetJsonArray2D(yellow2CycleAngle, elementsof(yellow2CycleAngle), elementsof(yellow2CycleAngle[0]));
            GetJsonArray2D(yellow2CycleTimer, elementsof(yellow2CycleTimer), elementsof(yellow2CycleTimer[0]));
            GetJsonValue(resolutionSpriteHeight);

            // deserializing damage source data (for Y1 lingering hitbox desync fix)
            GetJsonVectorArray(rogueDmgSrcs, {
                if (!el.IsArray() || el.Size() * sizeof(int32_t) != sizeof(PlayerDamageSource))
                    return std::nullopt;

                PlayerDamageSource dmgSrc {};
                int32_t* p = (int32_t*)&dmgSrc;
                for (rapidjson::SizeType i = 0; i < el.Size(); i++)
                    p[i] = el[i].GetInt();

                return dmgSrc;
            });
            GetJsonArray(nextDmgID, elementsof(nextDmgID));
            GetJsonVectorArray(expStoneColors, {
                if (el.IsArray())
                    return std::nullopt;
                return (uint32_t)el.GetInt();
            });

            return true;
        }
        std::string GetJson()
        {
            if (mode == 0) { // vanilla run mode
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th20", jalloc);
                AddJsonValue(mode);

                AddJsonArray(reimuR2Timer, elementsof(reimuR2Timer));
                AddJsonArray(passiveMeterTimer, elementsof(passiveMeterTimer));
                AddJsonValue(resolutionSpriteHeight);

                // lotta zeros we don't always need to store
                GlobalsSide* globals = (GlobalsSide*)RVA(GAME_SIDE0 + 0x88);
                if (globals->narrow_shot_stone_raw == 5 || globals->wide_shot_stone_raw == 5 ||
                    (globals->story_stone_raw == 5 && (globals->narrow_shot_stone_copy || globals->wide_shot_stone_copy))) {
                    AddJsonArray2D(yellow2CycleAngle, elementsof(yellow2CycleAngle), elementsof(yellow2CycleAngle[0]));
                    AddJsonArray2D(yellow2CycleTimer, elementsof(yellow2CycleTimer), elementsof(yellow2CycleTimer[0]));
                }

                AddJsonVectorArray(rogueDmgSrcs, {
                    rapidjson::Value dmgSrcArray(rapidjson::kArrayType);

                    int32_t* p = (int32_t*)&el;
                    size_t count = sizeof(PlayerDamageSource) / sizeof(int32_t);
                    for (size_t i = 0; i < count; ++i)
                        dmgSrcArray.PushBack(p[i], jalloc);

                    return dmgSrcArray;
                });
                AddJsonArray(nextDmgID, elementsof(nextDmgID));

                AddJsonVectorArray(expStoneColors, {
                    return rapidjson::Value(el);
                });

                ReturnJson();
            } else { // thprac mode
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th20", jalloc);
                AddJsonValue(mode);
                AddJsonValue(stage);
                if (section)
                    AddJsonValue(section);
                if (phase)
                    AddJsonValue(phase);
                if (dlg)
                    AddJsonValue(dlg);

                AddJsonValue(score);
                AddJsonValue(life);
                AddJsonValue(life_fragment);
                AddJsonValue(bomb);
                AddJsonValue(bomb_fragment);
                AddJsonValue(cycle);
                AddJsonValue(stoneMax);
                AddJsonValue(power);
                AddJsonValue(value);

                AddJsonValue(hyper);
                AddJsonValue(hyperActive);
                AddJsonValue(stoneActive);
                AddJsonValue(stone);
                AddJsonValue(levelR);
                AddJsonValue(priorityR);
                AddJsonValue(levelB);
                AddJsonValue(priorityB);
                AddJsonValue(levelY);
                AddJsonValue(priorityY);
                AddJsonValue(levelG);
                AddJsonValue(priorityG);
                AddJsonValue(resolutionSpriteHeight);

                ReturnJson();
            }

            CreateJson();
            jalloc; // Dummy usage to silence C4189
            ReturnJson();
        }

        bool HasTransitionSyncData(int st = 0)
        {
            if (!st) { // default: has ANY data (these can be 0 on st1 but not on later stages)
                st = 1;
                for (size_t s = st; s < 6; ++s)
                    if (yellow2CycleTimer[s][0] || rogueDmgSrcs[s].size())
                        return true;
            }

            // timer values should be non-zero on 1st stage transition if recorded
            // y2 opt timer checked per stage because you could otherwise make an adversarial replay (dont bring out y2 options until later stage)
            return reimuR2Timer[st] || passiveMeterTimer[st] || yellow2CycleTimer[st][0] || rogueDmgSrcs[st].size();
        }

        bool HasExpStoneData()
        {
            for (auto& stgData : expStoneColors)
                if (stgData.size())
                    return true;

            return false;
        }

        void MarkExpStoneFixed(bool extra)
        {
            if (HasExpStoneData())
                return;
            expStoneColors[extra ? 0 : 6].push_back(999999);
        }
    };
    THPracParam thPracParam {};
    uint32_t replayStones[4] {};
    size_t stage_expired_summon_cnt = 0;

    constexpr uint32_t minTimerOffsets[5] = { 39, 40, 49, 43, 44 }; // # of anm entries loaded on transition (max 1 per frame)
    constexpr uint32_t TIMER_OFFSET_MAX = 300;
    size_t advExtraFixResOpt = 0;

    bool advFixTimerOffsets = false;
    int32_t deterministicTransitionR2TimerVal = 0; // used to normalize variable delay before stage transition
    Gui::GuiDrag<uint32_t, ImGuiDataType_S32> advFixedTimerOffsets[5] = {
        Gui::GuiDrag<uint32_t, ImGuiDataType_S32> { "##mainrpyfix_st2", 0, (uint32_t)TIMER_OFFSET_MAX, 1, 100 },
        Gui::GuiDrag<uint32_t, ImGuiDataType_S32> { "##mainrpyfix_st3", 0, (uint32_t)TIMER_OFFSET_MAX, 1, 100 },
        Gui::GuiDrag<uint32_t, ImGuiDataType_S32> { "##mainrpyfix_st4", 0, (uint32_t)TIMER_OFFSET_MAX, 1, 100 },
        Gui::GuiDrag<uint32_t, ImGuiDataType_S32> { "##mainrpyfix_st5", 0, (uint32_t)TIMER_OFFSET_MAX, 1, 100 },
        Gui::GuiDrag<uint32_t, ImGuiDataType_S32> { "##mainrpyfix_st6", 0, (uint32_t)TIMER_OFFSET_MAX, 1, 100 }
    };
    std::vector<uint32_t> advExpStoneColors[7];
    THPracParam repFixParamCopy {};

    void ResetFixToolsSharedState()
    {
        // reset replay fix tools shared state iff replay selection changed or went back to title screen
        // shared state & not AdvWnd public members to avoid ugly forward declaration
        advExtraFixResOpt = 0;
        advFixTimerOffsets = false;
        deterministicTransitionR2TimerVal = 0;
        for (auto& off : advFixedTimerOffsets) *off = 0;
        for (auto& c : advExpStoneColors) c.clear();
        repFixParamCopy.Reset();
    }

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mMode = 1;
            *mLife = 7;
            *mBomb = 7;
            *mPower = 400;
            *mValue = 0;
            *mLevelR = 1;
            *mLevelB = 1;
            *mLevelY = 1;
            *mLevelG = 1;
            *mPriorityR = 0;
            *mPriorityB = 0;
            *mPriorityY = 0;
            *mPriorityG = 0;

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiPrac)

    public:
        __declspec(noinline) void State(int state)
        {
            switch (state) {
            case 0:
                break;
            case 1:
                mDifficulty = GetMemContent(RVA(DIFFICULTY));
                SetFade(0.8f, 0.1f);
                Open();
                thPracParam.Reset();
            case 2:
                break;
            case 3:
                SetFade(0.8f, 0.1f);
                Close();

                // Fill Param
                thPracParam.mode = *mMode;
                thPracParam.stage = *mStage;
                thPracParam.section = CalcSection();
                thPracParam.phase = SpellPhase() ? *mPhase : 0;
                if (SectionHasDlg(thPracParam.section))
                    thPracParam.dlg = *mDlg;

                thPracParam.score = *mScore;
                thPracParam.life = *mLife;
                thPracParam.life_fragment = *mLifeFragment;
                thPracParam.bomb = *mBomb;
                thPracParam.bomb_fragment = *mBombFragment;
                thPracParam.cycle = *mCycle;
                thPracParam.stoneMax = std::min(5000, stoneGaugeInitialValue[*mStage] + *stoneMax * 150);
                thPracParam.power = *mPower;
                thPracParam.value = *mValue;

                thPracParam.hyper = *mHyper / 10000.0f;
                thPracParam.stone = *mStone / 10000.0f;
                thPracParam.levelR = *mLevelR - 1;
                thPracParam.priorityR = *mPriorityR;
                thPracParam.levelB = *mLevelB - 1;
                thPracParam.priorityB = *mPriorityB;
                thPracParam.levelG = *mLevelG - 1;
                thPracParam.priorityG = *mPriorityG;
                thPracParam.levelY = *mLevelY - 1;
                thPracParam.priorityY = *mPriorityY;

                thPracParam.hyperActive = *mHyperActive;
                thPracParam.stoneActive = *mStoneActive;
                break;
            case 4:
                Close();
                break;
            default:
                break;
            }
        }

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_MENU));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSizeRel(0.55f, 0.7f);
                SetPosRel(0.215f, 0.18f);
                // SetSizeRel(0.65f, 0.81f);
                // SetPosRel(0.20f, 0.1f);
                SetItemWidthRel(-0.15f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(0.6f, 0.79f);
                SetPosRel(0.215f, 0.18f);
                SetItemWidthRel(-0.100f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(0.56f, 0.81f);
                SetPosRel(0.230f, 0.18f);
                SetItemWidthRel(-0.150f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(S(TH_MENU));
            ImGui::Separator();

            PracticeMenu();
        }
        const th_glossary_t* SpellPhase()
        {
            auto section = CalcSection();
            if (section == TH20_ST7_BOSS17)
                return TH20_SPELL_PHASE_TIMEOUT;
            if (section == TH20_ST6_BOSS12 || section == TH20_ST7_BOSS18) {
                return TH_SPELL_PHASE2;
            }
            return nullptr;
        }
        void PracticeMenu()
        {
            mMode();
            if (mStage())
                *mSection = *mChapter = 0;
            if (*mMode == 1) {
                int mbs = -1;
                if (*mStage == 2 || *mStage == 5) { // Counting from 0
                    mbs = 2;
                    if (*mWarp == 2)
                        *mWarp = 0;
                }

                if (mWarp(mbs))
                    *mSection = *mChapter = *mPhase = 0;
                if (*mWarp) {
                    SectionWidget();
                    mPhase(TH_PHASE, SpellPhase());
                }

                mLife();
                mLifeFragment();
                mBomb();
                mBombFragment();
                auto power_str = std::to_string((float)(*mPower) / 100.0f).substr(0, 4);
                mPower(power_str.c_str());
                auto value_str = std::format("{:.2f}", (float)(*mValue) / 5000.0f);
                mValue(value_str.c_str());

                
                ImGui::Columns(2, 0, false);
                if (mHyperActive()) {
                    if (*mHyperActive && *mHyper == 0)
                        *mHyper = 10000;
                    if (!*mHyperActive && *mHyper == 10000)
                        *mHyper = 0;
                }
                ImGui::NextColumn();
                if (mStoneActive()) {
                    if (*mStoneActive && *mStone == 0)
                        *mStone = 10000;
                    if (!*mStoneActive && *mStone == 10000)
                        *mStone = 0;
                }
                ImGui::Columns(1);

                mHyper(std::format("{:.2f} %%", (float)(*mHyper) / 100.0f).c_str());
                mStone(std::format("{:.2f} %%", (float)(*mStone) / 100.0f).c_str());
                stoneMax(std::format("{}({})", std::min(5000, stoneGaugeInitialValue[*mStage] + *stoneMax * 150), *stoneMax).c_str());
                ImGui::SameLine();
                HelpMarker(S(TH20_STONE_GAUGE_INITIAL_DESC));
                mCycle();

                ImGui::Columns(2);
                auto& style = ImGui::GetStyle();
                auto old_col = style.Colors[ImGuiCol_SliderGrab];
                auto old_col_active = style.Colors[ImGuiCol_SliderGrabActive];

                style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.25f, 0.25f, 0.40f);
                style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.25f, 0.25f, 1.0f);
                mLevelR();
                ImGui::NextColumn();
                mPriorityR();

                style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.25f, 1.0f, 0.40f);
                style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.25f, 0.25f, 1.0f, 1.0f);
                ImGui::NextColumn();
                mLevelB();
                ImGui::NextColumn();
                mPriorityB();

                style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 1.0f, 0.25f, 0.40f);
                style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 1.0f, 0.25f, 1.0f);
                ImGui::NextColumn();
                mLevelY();
                ImGui::NextColumn();
                mPriorityY();

                style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 1.0f, 0.25f, 0.40f);
                style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 1.0f, 0.25f, 1.0f);
                ImGui::NextColumn();
                mLevelG();
                ImGui::NextColumn();
                mPriorityG();

                style.Colors[ImGuiCol_SliderGrab] = old_col;
                style.Colors[ImGuiCol_SliderGrabActive] = old_col_active;
                ImGui::Columns(1);

                mScore();
                mScore.RoundDown(10);
            }
        }
        int CalcSection()
        {
            int chapterId = 0;
            switch (*mWarp) {
            case 1: // Chapter
                // Chapter Id = 10000 + Stage * 100 + Section
                chapterId += (*mStage + 1) * 100;
                chapterId += *mChapter;
                chapterId += 10000; // Base of chapter ID is 1000.
                return chapterId;
                break;
            case 2:
            case 3: // Mid boss & End boss
                return th_sections_cba[*mStage][*mWarp - 2][*mSection];
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                return th_sections_cbt[*mStage][*mWarp - 4][*mSection];
                break;
            default:
                return 0;
                break;
            }
        }
        bool SectionHasDlg(int32_t section)
        {
            switch (section) {
            case TH20_ST1_BOSS1:
            case TH20_ST2_BOSS1:
            case TH20_ST3_BOSS1:
            case TH20_ST4_BOSS1:
            case TH20_ST5_BOSS1:
            case TH20_ST6_BOSS1:
            case TH20_ST6_BOSS5:
            case TH20_ST6_BOSS9:
            case TH20_ST7_BOSS1:
            case TH20_ST7_MID1:
                return true;
            default:
                return false;
            }
        }
        void SectionWidget()
        {
            static char chapterStr[256] {};
            auto& chapterCounts = mChapterSetup[*mStage];

            switch (*mWarp) {
            case 1: // Chapter
                mChapter.SetBound(1, chapterCounts[0] + chapterCounts[1]);

                if (chapterCounts[1] == 0 && chapterCounts[0] != 0) {
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_N), *mChapter);
                } else if (*mChapter <= chapterCounts[0]) {
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_1), *mChapter);
                } else {
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_2), *mChapter - chapterCounts[0]);
                };

                mChapter(chapterStr);
                break;
            case 2:
            case 3: // Mid boss & End boss
                if (mSection(TH_WARP_SELECT[*mWarp],
                        th_sections_cba[*mStage][*mWarp - 2],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDifficulty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cba[*mStage][*mWarp - 2][*mSection]))
                    mDlg();
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                if (mSection(TH_WARP_SELECT[*mWarp],
                        th_sections_cbt[*mStage][*mWarp - 4],
                         th_sections_str[::THPrac::Gui::LocaleGet()][mDifficulty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cbt[*mStage][*mWarp - 4][*mSection]))
                    mDlg();
                break;
            default:
                break;
            }
        }

        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage { TH_STAGE, TH_STAGE_SELECT };
        Gui::GuiCombo mWarp { TH_WARP, TH_WARP_SELECT };
        Gui::GuiCombo mSection { TH_MODE };
        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiCheckBox mDlg { TH_DLG };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 7 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLifeFragment { TH_LIFE_FRAGMENT, 0, 2 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 7 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBombFragment { TH_BOMB_FRAGMENT, 0, 2 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 100, 400 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mValue { TH_VALUE, 0, 1000000 };

        Gui::GuiCheckBox mHyperActive { TH20_HYPER_ACTIVE };
        Gui::GuiCheckBox mStoneActive { TH20_STONE_ACTIVE };
        Gui::GuiSlider<int, ImGuiDataType_S32> mHyper { TH20_HYPER, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mStone { TH20_STONE_GAUGE, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> stoneMax { TH20_STONE_GAUGE_INITIAL, 0, 26 };
        Gui::GuiCombo mCycle { TH20_CYCLE, TH20_CYCLE_LIST };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelR { TH20_STONE_LEVEL_R, 1, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityR { TH20_STONE_PRIORITY_R, 0, 1000 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelB { TH20_STONE_LEVEL_B, 1, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityB { TH20_STONE_PRIORITY_B, 0, 1000 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelY { TH20_STONE_LEVEL_Y, 1, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityY { TH20_STONE_PRIORITY_Y, 0, 1000 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelG { TH20_STONE_LEVEL_G, 1, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityG { TH20_STONE_PRIORITY_G, 0, 1000 };

        int mChapterSetup[7][2] {
            { 3, 2 },
            { 3, 3 },
            { 4, 3 },
            { 5, 2 },
            { 5, 2 },
            { 4, 1 },
            { 4, 3 },
        };

        int mDifficulty = 0;
    };
    class THGuiRep : public Gui::GameGuiWnd {
        THGuiRep() noexcept
        {
            wchar_t appdata[MAX_PATH];
            GetEnvironmentVariableW(L"APPDATA", appdata, MAX_PATH);
            mAppdataPath = appdata;
        }
        SINGLETON(THGuiRep)

    public:
        THPracParam mRepParam;
        bool mRepSelected = false;
        uint32_t mSelectedRepStartStage;
        uint32_t mSelectedRepEndStage;
        uint32_t mSelectedRepPlaybackStartStage;
        std::wstring mSelectedRepName;
        std::wstring mSelectedRepDir;
        std::wstring mSelectedRepPath;

        void CheckReplay()
        {
            uintptr_t main_menu_ptr = GetMemContent(RVA(MAIN_MENU_PTR));
            uint32_t rep_offset = GetMemContent(main_menu_ptr + 0x5738) * 0x4 + 0x5740;
            std::wstring repName = mb_to_utf16(GetMemAddr<char*>(main_menu_ptr + rep_offset, 0x260), 932);
            std::wstring repDir(mAppdataPath);
            repDir.append(L"\\ShanghaiAlice\\th20\\replay\\");
            mSelectedRepName = repName;
            mSelectedRepDir = repDir;
            if (mSelectedRepPath != repDir + repName) {
                mSelectedRepPath = repDir + repName;
                ResetFixToolsSharedState();
            }

            std::string param;
            if (ReplayLoadParam(mSelectedRepPath.c_str(), param) && mRepParam.ReadJson(param))
                mParamStatus = true;
            else
                mRepParam.Reset();

            uint32_t* savedStones = GetMemAddr<uint32_t*>(main_menu_ptr + rep_offset, 0x1C, 0xDC);
            memcpy(replayStones, savedStones, sizeof(replayStones));

            for (int st = 1; st <= 7; ++st) {
                if (GetMemContent(main_menu_ptr + rep_offset, 0xf8 + 0x2c * st)) {
                    if (!mSelectedRepStartStage)
                        mSelectedRepStartStage = st;
                    mSelectedRepEndStage = st;
                }
            }
            if (advFixTimerOffsets && mSelectedRepPlaybackStartStage
                && repFixParamCopy.HasTransitionSyncData(mSelectedRepPlaybackStartStage))
                *(uint32_t*)(main_menu_ptr + 0x24) = mSelectedRepPlaybackStartStage;
        }

        bool mRepStatus = false;
        void State(int state)
        {
            switch (state) {
            case 1:
                mRepStatus = false;
                mParamStatus = false;
                mRepSelected = false;
                mSelectedRepStartStage = 0;
                mSelectedRepEndStage = 0;

                if (advFixTimerOffsets && thPracParam.HasTransitionSyncData())
                    repFixParamCopy = thPracParam;
                thPracParam.Reset();
                break;
            case 2:
                CheckReplay();
                mRepSelected = true;
                break;
            case 3:
                mRepStatus = true;
                mSelectedRepPlaybackStartStage = GetMemContent(RVA(MAIN_MENU_PTR), 0x24);
                stage_expired_summon_cnt = 0;

                if (advFixTimerOffsets && repFixParamCopy.HasTransitionSyncData())
                    thPracParam = repFixParamCopy;
                else if (mParamStatus)
                    thPracParam = mRepParam;

                if (advExtraFixResOpt && !thPracParam.resolutionSpriteHeight)
                    thPracParam.resolutionSpriteHeight = defaultSpriteHeights[advExtraFixResOpt];

                if (!mRepParam.HasExpStoneData()) // overwrite cached vectors from repFixParamCopy
                    for (size_t s = 0; s < elementsof(advExpStoneColors); ++s)
                        thPracParam.expStoneColors[s] = advExpStoneColors[s];

                break;
            default:
                break;
            }
        }

    protected:
        std::wstring mAppdataPath;
        bool mParamStatus = false;
    };
    class THOverlay : public Gui::GameGuiWnd {
        THOverlay() noexcept
        {
            SetTitle("Mod Menu");
            SetFade(0.5f, 0.5f);
            SetPos(10.0f, 10.0f);
            SetSize(0.0f, 0.0f);
            SetWndFlag(
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
            OnLocaleChange();
        }
        SINGLETON(THOverlay)

    public:
    protected:
        virtual void OnLocaleChange() override
        {
            float x_offset_1 = 0.0f;
            float x_offset_2 = 0.0f;
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                x_offset_1 = 0.1f;
                x_offset_2 = 0.14f;
                break;
            case Gui::LOCALE_EN_US:
                x_offset_1 = 0.1f;
                x_offset_2 = 0.14f;
                break;
            case Gui::LOCALE_JA_JP:
                x_offset_1 = 0.1f;
                x_offset_2 = 0.14f;
                break;
            default:
                break;
            }

            mMenu.SetTextOffsetRel(x_offset_1, x_offset_2);
            mMuteki.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfLives.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfBombs.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfPower.SetTextOffsetRel(x_offset_1, x_offset_2);
            mHyperGLock.SetTextOffsetRel(x_offset_1, x_offset_2);
            mWonderStGLock.SetTextOffsetRel(x_offset_1, x_offset_2);
            mTimeLock.SetTextOffsetRel(x_offset_1, x_offset_2);
            mAutoBomb.SetTextOffsetRel(x_offset_1, x_offset_2);
            mElBgm.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInGameInfo.SetTextOffsetRel(x_offset_1, x_offset_2);
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mInfLives();
            mInfBombs();
            mInfPower();
            mHyperGLock();
            mWonderStGLock();
            mAutoBomb();
            mTimeLock();
            mElBgm();
            mInGameInfo();
        }
        virtual void OnPreUpdate() override
        {
            if (mMenu(false) && !ImGui::IsAnyItemActive()) {
                if (*mMenu) {
                    Open();
                } else {
                    Close();
                }
            }
        }

        Gui::GuiHotKeyChord mMenu { "ModMenuToggle", "BACKSPACE", Gui::GetBackspaceMenuChord() };

        HOTKEY_DEFINE(mMuteki, TH_MUTEKI, "F1", VK_F1)
        PATCH_HK(0xF87FC, "01"),
        PATCH_HK(0xF8734, "EB")
        HOTKEY_ENDDEF();

        // HOTKEY_DEFINE(mInfLives, TH_INFLIVES, "F2", VK_F2)
        // PATCH_HK(0xE1288, "660F1F440000")
        // HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfBombs, TH_INFBOMBS, "F3", VK_F3)
        PATCH_HK(0xE1722, "0F1F00")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfPower, TH_INFPOWER, "F4", VK_F4)
        PATCH_HK(0xE16A2, NOP(3)) // 0xE16A8
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mWonderStGLock, TH20_WST_LOCK, "F6", VK_F6)
        PATCH_HK(0x77F75, NOP(3)),
        PATCH_HK(0x1127AC, "E9AA000000CCCC"),
        PATCH_HK(0x112028, "EB"),
        PATCH_HK(0x112072, NOP(5)),
        PATCH_HK(0x1125c6, NOP(5)),
        PATCH_HK(0x112AA0, "C3CCCC")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mAutoBomb, TH_AUTOBOMB, "F7", VK_F7)
        PATCH_HK(0xF79AC, "0F"),
        PATCH_HK(0xF79B3, "0F")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F8", VK_F8)
        PATCH_HK(0x86FDD, "EB"),
            PATCH_HK(0xA871E, "31")
                HOTKEY_ENDDEF();

    public:
        HOTKEY_DEFINE(mHyperGLock, TH20_HYP_LOCK, "F5", VK_F5)
        PATCH_HK(0x133935, NOP(3)),
        PATCH_HK(0x12FE5B, NOP(19)),
        PATCH_HK(0x1309A9, NOP(19)),
        PATCH_HK(0x1313DF, NOP(19)),
        PATCH_HK(0x1319C7, NOP(19)),
        PATCH_HK(0x131FFF, NOP(19)),
        PATCH_HK(0x1352EB, NOP(19)),
        PATCH_HK(0x13858B, NOP(19)),
        PATCH_HK(0x13652C, NOP(25)),
        PATCH_HK(0x1379DC, NOP(25))
        HOTKEY_ENDDEF();

        Gui::GuiHotKey mInfLives { TH_INFLIVES2, "F2", VK_F2 };
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F9", VK_F9 };
        Gui::GuiHotKey mInGameInfo { THPRAC_INGAMEINFO, "F10", VK_F10 };
    };

    // TODO(?)
    // static constinit HookCtx listIterUnlinkFix = { .addr = 0x11AB2, .data = PatchCode("e800000000") };
    EHOOK_ST(th20_piv_overflow_fix, 0xC496B, 2, {
        GlobalsSide* globals = (GlobalsSide*)RVA(GAME_SIDE0 + 0x88);

        int32_t half_piv_base = globals->piv_base / 2;
        int64_t uh_oh = (int64_t)globals->piv_base * globals->piv;
        pCtx->Esi = (int32_t)(uh_oh / globals->piv_divisor) + half_piv_base;
        pCtx->Eip = RVA(0xC49C4);
    });
    PATCH_ST(th20_piv_uncap_1, 0xA9FE5, "89D00F1F00");
    PATCH_ST(th20_piv_uncap_2, 0xB82E6, "89D00F1F00");
    PATCH_ST(th20_score_uncap, 0xE14F2, "EB");
    EHOOK_ST(th20_score_uncap_stage_tr, 0x10910F, 2, {
        pCtx->Esi += 8;
        pCtx->Edi += 8;
        pCtx->Ecx -= 2;
    });
    PATCH_ST(th20_infinite_stones, 0x11784B, "EB");
    PATCH_ST(th20_hitbox_scale_fix, 0xFF490, "B864000000C3");
    EHOOK_ST(th20_cleanup_stone_active, 0x11269f, 1, {
        // to make stoneActive work, we set the meter to max in init, let the game do its thing & adjust the meter/timer to the specified value
        GlobalsSide* globals = (GlobalsSide*)RVA(GAME_SIDE0 + 0x88);
        globals->summon_meter = (int32_t)(thPracParam.stone * thPracParam.stoneMax);

        // float interp_timer_val = 1320 - (1320 * thPracParam.stone);                         // option A: duration-accurate (50% stone = 50% of the duration)
        float interp_timer_val = 1320.0f * (1.0f - cbrtf(fmaxf(0.0001f, thPracParam.stone))); // option B: visuals-accurate (the drain meter is cubicly interpolated) (dont let it be 0) (just dont)
        
        Timer20* stone_interp_timer = GetMemAddr<Timer20*>(RVA(ENM_STONE_MGR_PTR), 0x84 + 0x14);
        asm_call_rel<SET_TIMER_FUNC, Thiscall>(stone_interp_timer, (int32_t)interp_timer_val);

        th20_cleanup_stone_active.Toggle(false);
    });

    PATCH_ST(th20_bullet_hitbox_fix_1, 0x3Efd2, "F30F108080000000F30F5C4224");
    PATCH_ST(th20_bullet_hitbox_fix_2, 0x3EFEA, "F30F108284000000F30F5C4128");

    PATCH_ST(th20_decrease_graze_effect, 0x9DC60, "C21000");

    float g_bossMoveDownRange = BOSS_MOVE_DOWN_RANGE_INIT;
    EHOOK_ST(th20_bossmovedown, 0x90953, 5, {
        float* y_pos = (float*)(pCtx->Eax + 0x17C);
        float* y_range = (float*)(pCtx->Eax + 0x184);
        if (g_bossMoveDownRange >= 0.0f)
        {
            float y_max = (*y_pos) + (*y_range) * 0.5f;
            float y_min2 = y_max - (*y_range) * (1.0f - g_bossMoveDownRange);
            *y_pos = (y_max + y_min2) * 0.5f;
            *y_range = (y_max - y_min2);
        }else{
            float y_min = (*y_pos) - (*y_range) * 0.5f;
            float y_max2 = y_min + (*y_range) * (1.0f - (-g_bossMoveDownRange));
            *y_pos = (y_min + y_max2) * 0.5f;
            *y_range = (y_max2 - y_min);
        }
    });
    HOOKSET_DEFINE(th20_master_disable)
    PATCH_DY(th20_master_disable1a, 0x87736, "eb")
    PATCH_DY(th20_master_disable1b, 0x8778F, "eb")
    PATCH_DY(th20_master_disable1c, 0x875E0, "03")
    HOOKSET_ENDDEF()

        
    class TH20InGameInfo : public Gui::GameGuiWnd {
        TH20InGameInfo() noexcept
        {
            SetTitle("igi");
            SetFade(0.9f, 0.9f);
            SetPosRel(890.0f / 1280.0f, 450.0f / 960.0f);
            SetSizeRel(360.0f / 1280.0f, 0.0f);
            SetWndFlag(ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
            OnLocaleChange();
        }
        SINGLETON(TH20InGameInfo)

    public:
        int32_t mMissCount;
        int32_t mBombCount;
        int32_t mHyperCount;
        int32_t mPyramidShotDownCount;
        int32_t mHyperBreakCount;

    protected:
        virtual void OnLocaleChange() override
        {
            float x_offset_1 = 0.0f;
            float x_offset_2 = 0.0f;
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                x_offset_1 = 0.12f;
                x_offset_2 = 0.172f;
                break;
            case Gui::LOCALE_EN_US:
                x_offset_1 = 0.12f;
                x_offset_2 = 0.16f;
                break;
            case Gui::LOCALE_JA_JP:
                x_offset_1 = 0.18f;
                x_offset_2 = 0.235f;
                break;
            default:
                break;
            }
        }

        virtual void OnContentUpdate() override
        {
            uintptr_t player_stats = RVA(0x1BA5F0);

            int32_t cur_player_type = (*(int32_t*)(player_stats + 0x8));
            int32_t main_stone = (*(int32_t*)(player_stats + 0x1C));
            int32_t substone_1 = (*(int32_t*)(player_stats + 0x20));
            int32_t substone_2 = (*(int32_t*)(player_stats + 0x24));
            int32_t substone_3 = (*(int32_t*)(player_stats + 0x28));

            int32_t diff = *((int32_t*)(RVA(0x1BA7D0)));
            auto diff_pl = std::format("{} ({})", S(IGI_DIFF[diff]), S(IGI_PL_20[cur_player_type]));
            auto diff_pl_sz = ImGui::CalcTextSize(diff_pl.c_str());
            ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - diff_pl_sz.x * 0.5f);
            ImGui::Text(diff_pl.c_str());


            auto sub_pl = std::format("{}({}/{}/{})", S(IGI_PL_20_SUB[main_stone]), S(IGI_PL_20_SUB[substone_1]), S(IGI_PL_20_SUB[substone_3]), S(IGI_PL_20_SUB[substone_2]));

            auto sub_pl_sz = ImGui::CalcTextSize(sub_pl.c_str());
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.0f, ImGui::GetWindowSize().x * 0.5f - sub_pl_sz.x * 0.5f));

            const ImVec4 r = { 1.0f, 0.5f, 0.5f, 1.0f };
            const ImVec4 g = { 0.5f, 1.0f, 0.5f, 1.0f };
            const ImVec4 b = { 0.5f, 0.75f, 1.0f, 1.0f };
            const ImVec4 y = { 1.0f, 1.0f, 0.3f, 1.0f };
            const ImVec4 w = { 1.0f, 1.0f, 1.0f, 1.0f };
            const ImVec4 p = { 1.0f, 0.5f, 1.0f, 1.0f };
            const ImVec4 rk = { 0.9f, 0.2f, 0.2f, 1.0f };
            const ImVec4 gk = { 0.2f, 0.7f, 0.2f, 1.0f };
            const ImVec4 bk = { 0.1f, 0.5f, 1.0f, 1.0f };
            const ImVec4 yk = { 0.8f, 0.8f, 0.2f, 1.0f };
            const ImVec4 spells_colors[] = { p, r, b, y, g, w };
            const ImVec4 stones_colors[] = { r, rk, b, bk, y, yk, g, gk, w };

            ImGui::TextColored(stones_colors[main_stone], S(IGI_PL_20_SUB[main_stone]));
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("(");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(stones_colors[substone_1], S(IGI_PL_20_SUB[substone_1]));
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("/");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(stones_colors[substone_3], S(IGI_PL_20_SUB[substone_3]));
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("/");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(stones_colors[substone_2], S(IGI_PL_20_SUB[substone_2]));
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text(")");


            ImGui::Columns(2);
            ImGui::Text(S(THPRAC_INGAMEINFO_MISS_COUNT));
            ImGui::NextColumn();
            ImGui::Text("%9d", mMissCount);
            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_BOMB_COUNT));
            ImGui::NextColumn();
            ImGui::Text("%9d", mBombCount);
            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_20_HYPER_COUNT));
            ImGui::NextColumn();
            ImGui::Text("%9d", mHyperCount);
            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_20_HYPER_BREAK_COUNT));
            ImGui::NextColumn();
            if (main_stone == 4) // yellow 1
            {
                ImGui::TextColored(y, "%9d", mHyperBreakCount);
            }else{
                ImGui::Text("%9d", mHyperBreakCount);
            }
            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_20_PYRAMID_COUNT));
            ImGui::NextColumn();
            ImGui::Text("%9d", mPyramidShotDownCount);

            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_20_PYRAMID_SUMMONED_COUNT));
            ImGui::NextColumn();
            int32_t pyramids_summoned = *(int32_t*)(player_stats + 0x94);
            if (pyramids_summoned % 3 == 0)
                ImGui::TextColored(r, "%9d", pyramids_summoned);
            else
                ImGui::TextColored(g, "%9d", pyramids_summoned);
            
            auto lvR = *(int32_t*)(player_stats + 0x74);
            auto lvB = *(int32_t*)(player_stats + 0x78);
            auto lvY = *(int32_t*)(player_stats + 0x7C);
            auto lvG = *(int32_t*)(player_stats + 0x80);

            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_20_PYRAMID_LV));
            ImGui::NextColumn();
            ImGui::TextColored(r, "%d", lvR+1);
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("/");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(b, "%d", lvB+1);
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("/");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(y, "%d", lvY+1);
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("/");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(g, "%d", lvG+1);
            ImGui::SameLine(0.0f, 0.0f);

            int32_t stage = GetMemContent(RVA(STAGE_NUM));
            if ((stage == 4 || stage == 5)
                && TH20Save::singleton().pyraState.isInPyra) {
                ImGui::Columns(1);
                const char* spells_str[6] = {
                    S(THPRAC_INGAMEINFO_20_PYRAMID_ST4_MID1),
                    S(THPRAC_INGAMEINFO_20_PYRAMID_ST4_MID2R),
                    S(THPRAC_INGAMEINFO_20_PYRAMID_ST4_MID2B),
                    S(THPRAC_INGAMEINFO_20_PYRAMID_ST4_MID2Y),
                    S(THPRAC_INGAMEINFO_20_PYRAMID_ST4_MID2G),
                    S(THPRAC_INGAMEINFO_20_PYRAMID_ST5_MID1),
                };
                int spid = TH20Save::singleton().pyraState.lastPyraSpellId;
                int plstone_type = TH20Save::GetPlayerType(cur_player_type, main_stone);
                ImGui::NewLine();
                

                ImVec4 spell_color = spells_colors[spid];
                if (spid == 5 && g_adv_igi_options.th12_chromatic_ufo) // st5 pyra
                {
                    static float t = 0;
                    t += 0.003f;
                    if (t >= 1.0f)
                        t = 0.0f;
                    float rr, gg, bb;
                    ImGui::ColorConvertHSVtoRGB(fmodf(t, 1.0f), 0.4f, 1.0f, rr, gg, bb);
                    spell_color = { rr, gg, bb, 1.0f };
                }
                if (TH20Save::singleton().pyraState.isPyraFailed)
                {
                    float rr, gg, bb, hh, ss, vv;
                    ImGui::ColorConvertRGBtoHSV(spell_color.x, spell_color.y, spell_color.z, hh, ss, vv);
                    ImGui::ColorConvertHSVtoRGB(hh, ss * 0.5f, 0.5f, rr, gg, bb);
                    spell_color = { rr, gg, bb, 1.0f };
                }

                ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - ImGui::CalcTextSize(spells_str[spid]).x * 0.5f);
                auto cpos = ImGui::GetCursorScreenPos();
                ImGui::TextColored(spell_color, spells_str[spid]);
                auto cpos2 = ImGui::GetCursorScreenPos();
                if (TH20Save::singleton().pyraState.isPyraFailed) {
                    auto textsz = ImGui::CalcTextSize(spells_str[spid]);
                    auto p = ImGui::GetWindowDrawList();
                    p->AddLine({ cpos.x, (cpos.y + cpos2.y) * 0.5f }, { cpos.x + textsz.x, (cpos.y + cpos2.y) * 0.5f },ImGui::ColorConvertFloat4ToU32(spell_color), 1.0f);
                }
                ImGui::Columns(2);
                if (thPracParam.mode) {
                    ImGui::TextColored(spell_color, S(THPRAC_INGAMEINFO_20_PASS_CUR));
                    ImGui::NextColumn();
                    ImGui::TextColored(spell_color, "%4d/%d(%.1f%%)",
                        TH20Save::singleton().saveCurrent.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Capture] + TH20Save::singleton().saveCurrent.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Timeout],
                        TH20Save::singleton().saveCurrent.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Attempt],
                        ((float)(TH20Save::singleton().saveCurrent.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Capture] + TH20Save::singleton().saveCurrent.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Timeout])
                            / std::fmaxf(1.0f, ((float)TH20Save::singleton().saveCurrent.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Attempt])) * 100.0f));
                    ImGui::NextColumn();
                    ImGui::TextColored(spell_color, S(THPRAC_INGAMEINFO_20_PASS_TOT));
                    ImGui::NextColumn();
                    ImGui::TextColored(spell_color, "%4d/%d(%.1f%%)",
                        TH20Save::singleton().saveTotal.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Capture] + TH20Save::singleton().saveTotal.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Timeout],
                        TH20Save::singleton().saveTotal.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Attempt],
                        ((float)(TH20Save::singleton().saveTotal.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Capture] + TH20Save::singleton().saveTotal.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Timeout])
                            / std::fmaxf(1.0f, ((float)TH20Save::singleton().saveTotal.ScHistoryPrac[spid][diff][plstone_type][TH20Save::Attempt])) * 100.0f));
                }
                else
                {
                    ImGui::TextColored(spell_color, S(THPRAC_INGAMEINFO_20_PASS_CUR));
                    ImGui::NextColumn();
                    ImGui::TextColored(spell_color, "%4d/%d(%.1f%%)",
                        TH20Save::singleton().saveCurrent.ScHistory[spid][diff][plstone_type][TH20Save::Capture] + TH20Save::singleton().saveCurrent.ScHistory[spid][diff][plstone_type][TH20Save::Timeout],
                        TH20Save::singleton().saveCurrent.ScHistory[spid][diff][plstone_type][TH20Save::Attempt],
                        ((float)(TH20Save::singleton().saveCurrent.ScHistory[spid][diff][plstone_type][TH20Save::Capture] + TH20Save::singleton().saveCurrent.ScHistory[spid][diff][plstone_type][TH20Save::Timeout])
                            / std::fmaxf(1.0f, ((float)TH20Save::singleton().saveCurrent.ScHistory[spid][diff][plstone_type][TH20Save::Attempt])) * 100.0f));
                    ImGui::NextColumn();
                    ImGui::TextColored(spell_color, S(THPRAC_INGAMEINFO_20_PASS_TOT));
                    ImGui::NextColumn();
                    ImGui::TextColored(spell_color, "%4d/%d(%.1f%%)",
                        TH20Save::singleton().saveTotal.ScHistory[spid][diff][plstone_type][TH20Save::Capture] + TH20Save::singleton().saveTotal.ScHistory[spid][diff][plstone_type][TH20Save::Timeout],
                        TH20Save::singleton().saveTotal.ScHistory[spid][diff][plstone_type][TH20Save::Attempt],
                        ((float)(TH20Save::singleton().saveTotal.ScHistory[spid][diff][plstone_type][TH20Save::Capture] + TH20Save::singleton().saveTotal.ScHistory[spid][diff][plstone_type][TH20Save::Timeout])
                            / std::fmaxf(1.0f, ((float)TH20Save::singleton().saveTotal.ScHistory[spid][diff][plstone_type][TH20Save::Attempt])) * 100.0f));
                }
            }
        }

        virtual void OnPreUpdate() override
        {
            if (*(DWORD*)(RVA(0x1ba56c))) {
                GameUpdateInner(20);
            } else {
            }
            if (*(THOverlay::singleton().mInGameInfo) && *(DWORD*)(RVA(0x1ba56c))) {
                SetPosRel(890.0f / 1280.0f, 450.0f / 960.0f);
                SetSizeRel(360.0f / 1280.0f, 0.0f);
                Open();
            } else {
                Close();
            }
        }

    public:
    };

    class THAdvOptWnd : public Gui::PPGuiWnd {
        SINGLETON(THAdvOptWnd)

    public:
        bool forceBossMoveDown = false;
    private:
    private:
        bool plHitboxScaleFix = false;
        bool showExpiredPyramidFixTool = false;

        void MasterDisableInit()
        {
            for (int i = 0; i < 3; i++)
                th20_master_disable[i].Setup();
            for (int i = 0; i < 3; i++)
                th20_master_disable[i].Toggle(g_adv_igi_options.disable_master_autoly);
        }
        void FpsInit()
        {
            mOptCtx.fps_dbl = 60.0;

            if (GetMemContent<uint8_t>(RVA(0x1C4F89)) == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)RVA(0x19EAF), 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)RVA(0x19EAF) = &mOptCtx.fps_dbl;
                VirtualProtect((void*)RVA(0x19EAF), 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = (double)mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
        }
        void GameplaySet()
        {
        }

        // From zero318
        struct ZUNListIter;
        struct ZUNListIterable;

        struct ZUNList {
            void* data; // 0x0
            ZUNList* next; // 0x4
            ZUNList* prev; // 0x8
            ZUNListIterable* list; // 0xC
            ZUNListIter* current_iter; // 0x10
        };

        struct ZUNListIter {
            ZUNList* current; // 0x0
            ZUNList* next; // 0x4
        };

        struct ZUNListIterable : ZUNList {
            ZUNList* tail; // 0x14
            ZUNListIter iter; // 0x18
        };

        static void __fastcall UnlinkNodeHook(ZUNListIterable* self, void*, ZUNList* node)
        {
            memset(&self->iter, 0, sizeof(self->iter));
            asm_call_rel<0x11AD0, Thiscall>(self, node);
        }

        THAdvOptWnd() noexcept
        {
            SetWndFlag(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
            SetFade(0.8f, 0.8f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);

            InitUpdFunc([&]() { ContentUpdate(); },
                [&]() { LocaleUpdate(); },
                [&]() {},
                []() {});

            OnLocaleChange();
            FpsInit();
            GameplayInit();
            MasterDisableInit();

            
            th20_bossmovedown.Setup();
            th20_piv_overflow_fix.Setup();
            th20_piv_uncap_1.Setup();
            th20_piv_uncap_2.Setup();
            th20_score_uncap.Setup();
            th20_score_uncap_stage_tr.Setup();
            th20_infinite_stones.Setup();
            th20_hitbox_scale_fix.Setup();
            th20_cleanup_stone_active.Setup();
            th20_bullet_hitbox_fix_1.Setup();
            th20_bullet_hitbox_fix_2.Setup();
            th20_decrease_graze_effect.Setup();

            th20_piv_overflow_fix.Toggle(g_adv_igi_options.th20_piv_overflow_fix);
            th20_piv_uncap_1.Toggle(g_adv_igi_options.th20_piv_uncap);
            th20_piv_uncap_2.Toggle(g_adv_igi_options.th20_piv_uncap);
            th20_score_uncap.Toggle(g_adv_igi_options.th20_score_uncap);
            th20_score_uncap_stage_tr.Toggle(g_adv_igi_options.th20_score_uncap);
            th20_infinite_stones.Toggle(g_adv_igi_options.th20_fake_unlock_stone);
            th20_bullet_hitbox_fix_1.Toggle(g_adv_igi_options.th20_fix_bullet_hitbox);
            th20_bullet_hitbox_fix_2.Toggle(g_adv_igi_options.th20_fix_bullet_hitbox);
            th20_decrease_graze_effect.Toggle(g_adv_igi_options.th20_decrease_graze_effect);
            
            // TODO(?)
            /*
            // thcrap base_tsa already patches this to fix the crash, don't try to rehook it if it's being used
            if (*(uint32_t*)RVA(0x11AD0) == 0x51EC8B55) {
                *(uintptr_t*)((uintptr_t)listIterUnlinkFix.data.buffer.ptr + 1) = (uintptr_t)&UnlinkNodeHook - RVA(0x11AB2 + 5);
                listIterUnlinkFix.Setup();
                listIterUnlinkFix.Enable();
            }
            */
        }

    public:
        __declspec(noinline) static bool StaticUpdate()
        {
            auto& advOptWnd = THAdvOptWnd::singleton();

            if (Gui::GetChordPressed(Gui::GetAdvancedMenuChord())) {
                if (advOptWnd.IsOpen())
                    advOptWnd.Close();
                else
                    advOptWnd.Open();
            }
            advOptWnd.Update();

            return advOptWnd.IsOpen();
        }

    protected:
        void LocaleUpdate()
        {
            SetTitle("AdvOptMenu");
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.0f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.0f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.0f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }
        }
        bool CloneSelectedReplayWithParams(THPracParam repParams)
        {
            // setup open file prompt
            OPENFILENAMEW ofn;
            wchar_t szFile[512];
            wcscpy_s(szFile, L"th20_ud----.rpy");
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = *(HWND*)RVA(WINDOW_PTR);
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Replay File\0*.rpy\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = nullptr;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = THGuiRep::singleton().mSelectedRepDir.c_str();
            ofn.lpstrDefExt = L".rpy";
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

            if (GetSaveFileNameW(&ofn)) {
                bool existingFile = (GetFileAttributesW(szFile) != INVALID_FILE_ATTRIBUTES);
                bool samePath = (GetUnifiedPath(szFile) == GetUnifiedPath(THGuiRep::singleton().mSelectedRepPath));

                // copy original replay to the selected path, overwriting if existing (unless same path)
                if (!samePath && !CopyFileW(THGuiRep::singleton().mSelectedRepPath.c_str(), szFile, FALSE)) {
                    MsgBox(MB_ICONERROR | MB_OK, S(TH_ERROR), S(TH_REPFIX_SAVE_ERROR_DEST), nullptr, ofn.hwndOwner);
                    return false;
                }

                // clear thprac params if present (no impact otherwise)
                if (ReplayClearParam(szFile) == ReplayClearResult::Error) {
                    MsgBox(MB_ICONERROR | MB_OK, S(TH_ERROR), S(TH_REPFIX_SAVE_ERROR_CLEAR_PARAMS), nullptr, ofn.hwndOwner);
                    if (!existingFile)
                        DeleteFileW(szFile);
                    return false;
                }

                // save params & notify
                if (!ReplaySaveParam(szFile, repParams.GetJson())) {
                    MsgBox(MB_ICONINFORMATION | MB_OK, S(TH_REPFIX_SAVE_SUCCESS), S(TH_REPFIX_SAVE_SUCCESS_DESC), utf16_to_utf8(szFile).c_str(), ofn.hwndOwner);
                    return true;

                } else { // delete copy if params didn't save
                    MsgBox(MB_ICONERROR | MB_OK, S(TH_ERROR), S(TH_REPFIX_SAVE_ERROR_PARAMS), nullptr, ofn.hwndOwner);
                    if (!existingFile)
                        DeleteFileW(szFile);
                }
            }

            return false;
        }

        void CloneWithParamsAndRefresh(THPracParam newRepParam)
        {
            if (CloneSelectedReplayWithParams(newRepParam) && !THGuiRep::singleton().mRepStatus)
                THGuiRep::singleton().CheckReplay(); // refresh for if user overwrote og file in menu
        }

        void DisableTooltip(bool disabled, th_glossary_t hint, bool keepDisabled = true)
        {
            if (disabled) {
                ImGui::EndDisabled();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip(S(hint));
                if (keepDisabled)
                    ImGui::BeginDisabled();
            }
        }

        void ContentUpdate()
        {
            bool wndFocus = true;
            float dpiScale = ImGui::GetFrameHeight() / 22.0f;
            ImGui::TextUnformatted(S(TH_ADV_OPT));
            ImGui::Separator();
            ImGui::BeginChild("Adv. Options", ImVec2(0.0f, 0.0f));

            if (BeginOptGroup<TH_GAME_SPEED>()) {
                if (GameFPSOpt(mOptCtx))
                    FpsSet();
                EndOptGroup();
            }

            if (BeginOptGroup<TH_GAMEPLAY>()) {
                // ImGui::Checkbox("show att hitbox", &g_show_att_hitbox);

                if (ImGui::Checkbox(S(TH_BOSS_FORCE_MOVE_DOWN), &forceBossMoveDown)) {
                    th20_bossmovedown.Toggle(forceBossMoveDown);
                }
                ImGui::SameLine();
                HelpMarker(S(TH_BOSS_FORCE_MOVE_DOWN_DESC));
                ImGui::SameLine();
                ImGui::SetNextItemWidth(200.0f);
                if (ImGui::DragFloat(S(TH_BOSS_FORCE_MOVE_DOWN_RANGE), &g_bossMoveDownRange, 0.004f, -1.0f, 1.0f))
                    g_bossMoveDownRange = std::clamp(g_bossMoveDownRange, -1.0f, 1.0f);

                if (ImGui::Checkbox(S(TH_DISABLE_MASTER), &g_adv_igi_options.disable_master_autoly)) {
                    for (int i = 0; i < 3; i++)
                        th20_master_disable[i].Toggle(g_adv_igi_options.disable_master_autoly);
                }
                ImGui::SameLine();
                HelpMarker(S(TH_DISABLE_MASTER_DESC));

                DisableKeyOpt();
                KeyHUDOpt();
                InfLifeOpt();

                
                th20_piv_overflow_fix.Toggle(g_adv_igi_options.th20_piv_overflow_fix);
                th20_piv_uncap_1.Toggle(g_adv_igi_options.th20_piv_uncap);
                th20_piv_uncap_2.Toggle(g_adv_igi_options.th20_piv_uncap);
                th20_score_uncap.Toggle(g_adv_igi_options.th20_score_uncap);
                th20_score_uncap_stage_tr.Toggle(g_adv_igi_options.th20_score_uncap);
                th20_infinite_stones.Toggle(g_adv_igi_options.th20_fake_unlock_stone);
                th20_bullet_hitbox_fix_1.Toggle(g_adv_igi_options.th20_fix_bullet_hitbox);
                th20_bullet_hitbox_fix_2.Toggle(g_adv_igi_options.th20_fix_bullet_hitbox);

                if (ImGui::Checkbox(S(TH20_PIV_OVERFLOW_FIX), &g_adv_igi_options.th20_piv_overflow_fix))
                {
                    th20_piv_overflow_fix.Toggle(g_adv_igi_options.th20_piv_overflow_fix);
                    th20_score_uncap_stage_tr.Toggle(g_adv_igi_options.th20_score_uncap || g_adv_igi_options.th20_piv_overflow_fix || g_adv_igi_options.th20_piv_uncap);
                }
                ImGui::SameLine();
                if (ImGui::Checkbox(S(TH20_UNCAP_PIV), &g_adv_igi_options.th20_piv_uncap)) {
                    th20_piv_uncap_1.Toggle(g_adv_igi_options.th20_piv_uncap);
                    th20_piv_uncap_2.Toggle(g_adv_igi_options.th20_piv_uncap);
                    th20_score_uncap_stage_tr.Toggle(g_adv_igi_options.th20_score_uncap || g_adv_igi_options.th20_piv_overflow_fix || g_adv_igi_options.th20_piv_uncap);
                }
                ImGui::SameLine();
                if (ImGui::Checkbox(S(TH20_UNCAP_SCORE), &g_adv_igi_options.th20_score_uncap))
                {
                    th20_score_uncap.Toggle(g_adv_igi_options.th20_score_uncap);
                    th20_score_uncap_stage_tr.Toggle(g_adv_igi_options.th20_score_uncap || g_adv_igi_options.th20_piv_overflow_fix || g_adv_igi_options.th20_piv_uncap);
                }
                if (ImGui::Checkbox(S(TH20_FAKE_UNLOCK_STONES), &g_adv_igi_options.th20_fake_unlock_stone))
                    th20_infinite_stones.Toggle(g_adv_igi_options.th20_fake_unlock_stone);
                ImGui::SameLine();
                HelpMarker(S(TH20_FAKE_UNLOCK_STONES_DESC));
                ImGui::SameLine();
                if (ImGui::Checkbox(S(TH20_FIX_HITBOX), &plHitboxScaleFix))
                    th20_hitbox_scale_fix.Toggle(plHitboxScaleFix);
                ImGui::SameLine();
                HelpMarker(S(TH20_FIX_HITBOX_DESC));
                if (ImGui::Checkbox(S(TH20_FIX_BULLETHITBOX), &g_adv_igi_options.th20_fix_bullet_hitbox))
                {
                    th20_bullet_hitbox_fix_1.Toggle(g_adv_igi_options.th20_fix_bullet_hitbox);
                    th20_bullet_hitbox_fix_2.Toggle(g_adv_igi_options.th20_fix_bullet_hitbox);
                }
                ImGui::SameLine();
                if (ImGui::Checkbox(S(TH20_DECREASE_EFF), &g_adv_igi_options.th20_decrease_graze_effect)) {
                    th20_decrease_graze_effect.Toggle(g_adv_igi_options.th20_decrease_graze_effect);
                }
                
                ImGui::SetNextItemWidth(180.0f);
                EndOptGroup();
            }
            {
                ImGui::SetNextWindowCollapsed(false);
                if (ImGui::CollapsingHeader(S(THPRAC_INGAMEINFO_06_SHOWDETAIL_COLLAPSE))) {
                    TH20Save::singleton().ShowDetail();
                    ImGui::NewLine();
                    ImGui::Separator();
                    ImGui::Separator();
                    ImGui::Separator();
                    ImGui::NewLine();
                }
            }
             if (BeginOptGroup<TH_REPLAY_FIX>()) {

                // Main story replay fixes
                CustomMarker(S(TH_REPFIX_NEED_THPRAC), S(TH_REPFIX_NEED_THPRAC_DESC));
                ImGui::SameLine();
                ImGui::TextUnformatted(S(TH20_MAIN_STORY_FIXES));
                ImGui::SameLine();
                HelpMarker(S(TH20_MAIN_STORY_FIXES_DESC));

                bool hasTransitions = THGuiRep::singleton().mSelectedRepStartStage != THGuiRep::singleton().mSelectedRepEndStage;
                bool hasTransitionSyncData = THGuiRep::singleton().mRepParam.HasTransitionSyncData();

                if (hasTransitions) {
                    if (!hasTransitionSyncData) {
                        ImGui::Text(S(TH_REPFIX_SELECTED), THGuiRep::singleton().mSelectedRepName.c_str());

                        ImGui::BeginDisabled(THGuiRep::singleton().mRepStatus);
                        if(ImGui::Button(S(advFixTimerOffsets ? TH20_MAINRPYFIX_DISABLE_TIMERS_FIX : TH20_MAINRPYFIX_ENABLE_TIMERS_FIX)))
                            advFixTimerOffsets = !advFixTimerOffsets;
                        if (THGuiRep::singleton().mRepStatus) {
                            ImGui::EndDisabled();
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip(S(TH20_MAINRPYFIX_TIMERS_FIX_DISABLE_HINT));
                        }

                        ImGui::SameLine();
                        HelpMarker(S(TH20_MAINRPYFIX_TIMERS_FIX_DESC));
                        ImGui::SameLine();
                        CustomMarker(S(TH20_MAINRPYFIX_TIPS), S(TH20_MAINRPYFIX_TIMERS_FIX_TIPS_DESC));
                        ImGui::SameLine();
                        CustomMarker(S(TH20_MAINRPYFIX_TIPS2), S(TH20_MAINRPYFIX_TIMERS_FIX_TIPS2_DESC));

                        int32_t stage = THGuiRep::singleton().mRepStatus ? (GetMemContent(RVA(STAGE_NUM)) - 1) : -1;
                        uint32_t totalTransitions = THGuiRep::singleton().mSelectedRepEndStage - 1;

                        if (advFixTimerOffsets) {
                            ImGui::Columns(2, 0, false);
                            for (size_t s = 0; s < totalTransitions; s++) {
                                ImGui::Text(S(TH_STAGE_NUM), s + 2);
                                ImGui::SameLine();

                                bool disabled = ((int32_t)s < stage);
                                bool maxed = *advFixedTimerOffsets[s] == TIMER_OFFSET_MAX;
                                bool mined = !*advFixedTimerOffsets[s];

                                ImGui::BeginDisabled(disabled);
                                ImGui::SetNextItemWidth(175 * dpiScale);
                                advFixedTimerOffsets[s]("%df");
                                DisableTooltip(disabled, TH20_MAINRPYFIX_STAGE_TIMER_DISABLE_HINT);
                                ImGui::PushID(s);

                                ImGui::SameLine();
                                ImGui::BeginDisabled(!disabled && maxed);
                                if (ImGui::Button("+", ImVec2(20 * dpiScale, 20 * dpiScale)))
                                    *advFixedTimerOffsets[s] += 1;
                                ImGui::EndDisabled(!disabled && maxed);
                                DisableTooltip(disabled, TH20_MAINRPYFIX_STAGE_TIMER_DISABLE_HINT);

                                ImGui::SameLine();
                                ImGui::BeginDisabled(!disabled && mined);
                                if (ImGui::Button("-", ImVec2(20 * dpiScale, 20 * dpiScale)))
                                    *advFixedTimerOffsets[s] -= 1;
                                ImGui::EndDisabled(!disabled && mined);
                                DisableTooltip(disabled, TH20_MAINRPYFIX_STAGE_TIMER_DISABLE_HINT);

                                ImGui::PopID();
                                ImGui::EndDisabled(disabled);
                                if (s == 2) ImGui::NextColumn();
                            }
                        }

                        bool startedOnSt1 = (!stage || thPracParam.HasTransitionSyncData(1)); // either on st1 or recorded 1st transition
                        if (!startedOnSt1 && stage == 1 && GetMemContent(RVA(TRANSITION_STG_PTR))) startedOnSt1 = true; //(or in s1->2 transition)

                        uint32_t remainingTransitions = totalTransitions - stage;
                        bool mainRpyFixDisableSave = (!startedOnSt1 || remainingTransitions);

                        // checking no stage was skipped for stage timer fix mode (where s1 may have data from param copy mechanic)
                        if (!mainRpyFixDisableSave) {
                            for (size_t s = 2; s < THGuiRep::singleton().mSelectedRepEndStage; s++) {
                                if (!thPracParam.HasTransitionSyncData(s)) {
                                    mainRpyFixDisableSave = true;
                                    break;
                                }
                            }
                        }

                        char buttonLabel[64];
                        snprintf(buttonLabel, sizeof(buttonLabel), "%s", S(TH_REPFIX_SAVE));

                       if (remainingTransitions && startedOnSt1)
                            snprintf(buttonLabel, sizeof(buttonLabel), S(TH_REPFIX_SAVE_PROGRESS), stage, totalTransitions);

                        ImGui::BeginDisabled(mainRpyFixDisableSave);
                        bool saveClicked = ImGui::Button(buttonLabel);
                        if (mainRpyFixDisableSave) {
                            ImGui::EndDisabled();
                            if (ImGui::IsItemHovered()) {
                                if (!startedOnSt1) ImGui::SetTooltip(S(TH20_MAINRPYFIX_SAVE_NO_ST1_HINT));
                                else if (remainingTransitions) ImGui::SetTooltip(S(TH20_MAINRPYFIX_SAVE_PROGRESS_HINT), remainingTransitions, remainingTransitions > 1 ? "s" : "");
                                else ImGui::SetTooltip(S(TH20_MAINRPYFIX_SAVE_SKIPPED_STAGE_HINT));
                            }
                        } else if (saveClicked) {
                            THPracParam newRepParam = THGuiRep::singleton().mRepParam;
                            memcpy(newRepParam.reimuR2Timer, thPracParam.reimuR2Timer, sizeof(newRepParam.reimuR2Timer));
                            memcpy(newRepParam.passiveMeterTimer, thPracParam.passiveMeterTimer, sizeof(newRepParam.passiveMeterTimer));
                            memcpy(newRepParam.yellow2CycleAngle, thPracParam.yellow2CycleAngle, sizeof(newRepParam.yellow2CycleAngle));
                            memcpy(newRepParam.yellow2CycleTimer, thPracParam.yellow2CycleTimer, sizeof(newRepParam.yellow2CycleTimer));
                            memcpy(newRepParam.nextDmgID, thPracParam.nextDmgID, sizeof(newRepParam.nextDmgID));
                            for (size_t i = 0; i < elementsof(newRepParam.rogueDmgSrcs); ++i)
                                newRepParam.rogueDmgSrcs[i] = thPracParam.rogueDmgSrcs[i];
                            for (size_t s = 0; s < elementsof(advExpStoneColors); ++s)
                                newRepParam.expStoneColors[s] = advExpStoneColors[s];
                            newRepParam.MarkExpStoneFixed(false);

                            CloneSelectedReplayWithParams(newRepParam);
                            //can't save in menu, thPracParam is reset
                        }

                        if (advFixTimerOffsets) ImGui::Columns(1);

                    } else {
                        ImGui::TextDisabled(S(TH_REPFIX_SELECTED_ALREADY_FIXED));
                        ImGui::SameLine();

                        if (ImGui::Button(S(TH_REPFIX_RESET_DATA))) {
                            THPracParam newRepParam = THGuiRep::singleton().mRepParam;
                            memset(newRepParam.reimuR2Timer, 0, sizeof(newRepParam.reimuR2Timer));
                            memset(newRepParam.passiveMeterTimer, 0, sizeof(newRepParam.passiveMeterTimer));
                            memset(newRepParam.yellow2CycleAngle, 0, sizeof(newRepParam.yellow2CycleAngle));
                            memset(newRepParam.yellow2CycleTimer, 0, sizeof(newRepParam.yellow2CycleTimer));
                            memset(newRepParam.nextDmgID, 0, sizeof(newRepParam.nextDmgID));
                            for (size_t st = 0; st < elementsof(newRepParam.rogueDmgSrcs); ++st)
                                newRepParam.rogueDmgSrcs[st].clear();

                            CloneWithParamsAndRefresh(newRepParam);
                        }
                    }
                } else {
                    if (THGuiRep::singleton().mSelectedRepStartStage >= 1 && THGuiRep::singleton().mSelectedRepEndStage <= 6)
                         ImGui::TextDisabled(S(TH20_MAINRPYFIX_SELECTED_NO_TRANSITIONS));
                    else ImGui::TextDisabled(S(TH_REPFIX_MAIN_SELECTED_NONE));
                }

                // Extra replay fix
                ImGui::Separator();
                CustomMarker(S(TH_REPFIX_NEED_THPRAC), S(TH_REPFIX_NEED_THPRAC_DESC));
                ImGui::SameLine();
                ImGui::TextUnformatted(S(TH20_EXTRA_RESOLUTION_FIX));
                ImGui::SameLine();
                HelpMarker(S(TH20_EXTRA_RESOLUTION_FIX_DESC));
                bool extraResFixDisableSave = true;

                if (THGuiRep::singleton().mSelectedRepEndStage == 7) {
                    if (!THGuiRep::singleton().mRepParam.resolutionSpriteHeight) {
                        ImGui::Text(S(TH_REPFIX_SELECTED), THGuiRep::singleton().mSelectedRepName.c_str());

                        // don't allow changing the resolution fix option while playing the replay (would be confusing)
                        bool disableSelection = (bool)THGuiRep::singleton().mRepStatus;
                        ImGui::BeginDisabled(disableSelection);
                        ImGui::TextUnformatted(S(TH20_EXRESFIX_RESOLUTION));
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(200 * dpiScale);
                        Gui::ComboSelect(advExtraFixResOpt, (th_glossary_t*)TH20_EXRESFIX_RESOLUTION_OPT, elementsof(TH20_EXRESFIX_RESOLUTION_OPT) - 1, "##exresfix_res");
                        DisableTooltip(disableSelection, TH20_EXRESFIX_RESOLUTION_DISABLE_HINT, false);

                        if (ImGui::IsPopupOpen("##exresfix_res") && !disableSelection)
                            wndFocus = false;

                        ImGui::SameLine();

                        // don't allow saving without specifying a resolution
                        extraResFixDisableSave = !advExtraFixResOpt;
                        ImGui::BeginDisabled(extraResFixDisableSave);
                        if (ImGui::Button(S(TH_REPFIX_SAVE))) {
                            THPracParam newRepParam = THGuiRep::singleton().mRepParam;
                            newRepParam.resolutionSpriteHeight = defaultSpriteHeights[advExtraFixResOpt];

                            for (size_t s = 0; s < elementsof(advExpStoneColors); ++s)
                                newRepParam.expStoneColors[s] = advExpStoneColors[s];
                            newRepParam.MarkExpStoneFixed(true);

                            CloneWithParamsAndRefresh(newRepParam);
                        };
                        DisableTooltip(extraResFixDisableSave, TH20_EXRESFIX_SAVE_DISABLE_HINT, false);

                    } else {
                        ImGui::TextDisabled(S(TH_REPFIX_SELECTED_ALREADY_FIXED));
                        ImGui::SameLine();
                        if (ImGui::Button(S(TH_REPFIX_RESET_DATA))) {
                            THPracParam newRepParam = THGuiRep::singleton().mRepParam;
                            newRepParam.resolutionSpriteHeight = 0;

                            CloneWithParamsAndRefresh(newRepParam);
                        }
                    }
                }
                else ImGui::TextDisabled(S(TH_REPFIX_EXTRA_SELECTED_NONE));

                // Expired pyramid bug replay fix
                ImGui::Separator();
                CustomMarker(S(TH_REPFIX_NEED_THPRAC), S(TH_REPFIX_NEED_THPRAC_DESC));
                ImGui::SameLine();
                ImGui::TextUnformatted(S(TH20_EXPIRED_STONE_FIX));
                ImGui::SameLine();
                HelpMarker(S(TH20_EXPIRED_STONE_FIX_DESC));
                ImGui::SameLine();
                ImGui::Checkbox(S(TH_TOOL_SHOW_TOGGLE), &showExpiredPyramidFixTool);

                if (showExpiredPyramidFixTool) {
                    if (THGuiRep::singleton().mRepSelected) {
                        if (!THGuiRep::singleton().mRepParam.HasExpStoneData()) {
                            ImGui::Text(S(TH_REPFIX_SELECTED), THGuiRep::singleton().mSelectedRepName.c_str());

                            uint32_t maxCtrlsPerLine = (THGuiRep::singleton().mSelectedRepEndStage - THGuiRep::singleton().mSelectedRepStartStage > 2) ? 3 : 6;
                            float leftPad = (THGuiRep::singleton().mSelectedRepEndStage == 7) ? 58.0f : 38.0f;
                            bool disableChangingExpStoneData = THGuiRep::singleton().mRepStatus;

                            if (disableChangingExpStoneData)
                                ImGui::Text(S(TH20_EXPSTONEFIX_STAGE_COUNTER), stage_expired_summon_cnt);

                            ImGui::BeginDisabled(disableChangingExpStoneData);
                            ImGui::Columns(2, 0, false);

                            for (size_t s = THGuiRep::singleton().mSelectedRepStartStage;
                                s <= THGuiRep::singleton().mSelectedRepEndStage; ++s) {
                                ImGui::EndDisabled(disableChangingExpStoneData);
                                ImGui::Text(S(s == 7 ? TH_EXTRA_STAGE : TH_STAGE_NUM), s);
                                ImGui::BeginDisabled(disableChangingExpStoneData);
                                ImGui::SameLine();

                                std::vector<uint32_t>& stageVec = advExpStoneColors[s - 1];

                                for (size_t i = 0; i < stageVec.size(); i++) {
                                    char label[32];
                                    snprintf(label, sizeof(label), "##epc_st%zu_%d", s, i);

                                    ImGui::SetNextItemWidth(70 * dpiScale);
                                    Gui::ComboSelect(stageVec[i], (th_glossary_t*)TH20_EXPIRED_PYRAMID_FIX_OPT, elementsof(TH20_EXPIRED_PYRAMID_FIX_OPT) - 1, label);
                                    DisableTooltip(disableChangingExpStoneData, TH20_EXPSTONEFIX_DISABLE_HINT);

                                    if (ImGui::IsPopupOpen(label) && !THGuiRep::singleton().mRepStatus)
                                        wndFocus = false;

                                    if (i % maxCtrlsPerLine == maxCtrlsPerLine - 1)
                                        ImGui::Dummy(ImVec2(leftPad * dpiScale, 0));
                                    ImGui::SameLine();
                                }

                                ImGui::PushID(s+10);
                                if (ImGui::Button("+", ImVec2(20 * dpiScale, 20 * dpiScale)))
                                    stageVec.push_back(0);
                                DisableTooltip(disableChangingExpStoneData, TH20_EXPSTONEFIX_DISABLE_HINT);

                                if (stageVec.size()) {
                                    ImGui::SameLine();
                                    if (ImGui::Button("-", ImVec2(20 * dpiScale, 20 * dpiScale)))
                                        stageVec.pop_back();
                                    DisableTooltip(disableChangingExpStoneData, TH20_EXPSTONEFIX_DISABLE_HINT);
                                }
                                ImGui::PopID();
                                if (s == 3) ImGui::NextColumn();
                            }
                            ImGui::Columns(1);
                            ImGui::EndDisabled(disableChangingExpStoneData);

                            bool expStoneDataSet = false;
                            for (auto& stgData : advExpStoneColors) {
                                if (stgData.size()) {
                                    expStoneDataSet = true;
                                    break;
                                }
                            }

                            // disable if can save with exfix tool OR need to use mainrpyfix
                            ImGui::BeginDisabled(!extraResFixDisableSave || (hasTransitions && !hasTransitionSyncData));

                            if (expStoneDataSet) {
                                if (ImGui::Button(S(TH_REPFIX_SAVE))) {
                                    THPracParam newRepParam = THGuiRep::singleton().mRepParam;
                                    for (size_t s = 0; s < elementsof(advExpStoneColors); ++s)
                                        newRepParam.expStoneColors[s] = advExpStoneColors[s];

                                    CloneWithParamsAndRefresh(newRepParam);
                                }
                            }
                            else {
                                if (ImGui::Button(S(TH20_EXPSTONEFIX_MARK_FIXED))) {
                                    THPracParam newRepParam = THGuiRep::singleton().mRepParam;
                                    newRepParam.MarkExpStoneFixed(THGuiRep::singleton().mSelectedRepEndStage == 7);

                                    CloneWithParamsAndRefresh(newRepParam);
                                }
                            }

                            if (!extraResFixDisableSave)
                                DisableTooltip(true, TH20_EXPSTONEFIX_SAVE_USE_EXRESFIX_HINT, false);
                            else if (hasTransitions && !hasTransitionSyncData)
                                DisableTooltip(true, TH20_EXPSTONEFIX_SAVE_USE_MAINRPYFIX_HINT, false);

                        } else {
                            ImGui::TextDisabled(S(TH_REPFIX_SELECTED_ALREADY_FIXED));
                            ImGui::SameLine();
                            if (ImGui::Button(S(TH_REPFIX_RESET_DATA2))) {
                                THPracParam newRepParam = THGuiRep::singleton().mRepParam;
                                for (auto& stgData : newRepParam.expStoneColors)
                                    stgData.clear();
                                CloneWithParamsAndRefresh(newRepParam);
                            }
                        }
                    } else ImGui::TextDisabled(S(TH_REPFIX_SELECTED_NONE));
                }

                EndOptGroup();
            }
             SSS::SSS_UI(20);

            AboutOpt(S(TH20_CREDITS));
            ImGui::EndChild();
            if(wndFocus) ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;
    };
    void ECLStdExec(ECLHelper& ecl, unsigned int start, int std_id, int ecl_time = 0)
    {
        if (start)
            ecl.SetPos(start);
        ecl << ecl_time << 0x0014026e << 0x01ff0000 << 0x00000000 << std_id;
    }
    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }

    constexpr unsigned int st1PostMaple = 0x7eec;
    constexpr unsigned int st2PostMaple = 0xa45c;
    constexpr unsigned int st3PostMaple = 0xb5bc;
    constexpr unsigned int st4PostMaple = 0xa740;
    constexpr unsigned int st5PostMaple = 0x5058;
    constexpr unsigned int st6PostMaple = 0x4b80;
    constexpr unsigned int st7PostMaple = 0x6b28;
    constexpr unsigned int stdInterruptSize = 0x14;
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            constexpr unsigned int st1MainFront = 0x84e4;
            constexpr unsigned int st1MainSub00 = 0x5ad8;
            constexpr unsigned int st1MainLatter = 0x85ac;
            constexpr unsigned int st1MainLatterWait = 0x5d28;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub02Call = 0x5b98;
                ECLJump(ecl, st1PostMaple, st1MainFront, 60, 90);
                ECLJump(ecl, st1MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub03Call = 0x5bf8;
                ECLJump(ecl, st1PostMaple, st1MainFront, 60, 90);
                ECLJump(ecl, st1MainSub00, mainSub03Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub07Call = 0x5d3c;
                ECLStdExec(ecl, st1PostMaple, 1, 1);
                ECLJump(ecl, st1PostMaple + stdInterruptSize, st1MainLatter, 60, 90);
                ECLJump(ecl, st1MainLatterWait, mainSub07Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub09Call = 0x5de8;
                ECLStdExec(ecl, st1PostMaple, 1, 1);
                ECLJump(ecl, st1PostMaple + stdInterruptSize, st1MainLatter, 60, 90);
                ECLJump(ecl, st1MainLatterWait, mainSub09Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 2) {
            constexpr unsigned int st2MainFront = 0xaa54;
            constexpr unsigned int st2MainSub00 = 0x7778;
            constexpr unsigned int st2MainLatter = 0xab1c;
            constexpr unsigned int st2MainLatterWait = 0x798c;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub02Call = 0x7838;
                ECLJump(ecl, st2PostMaple, st2MainFront, 60, 90);
                ECLJump(ecl, st2MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub04Call = 0x78f8;
                ECLJump(ecl, st2PostMaple, st2MainFront, 60, 90);
                ECLJump(ecl, st2MainSub00, mainSub04Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub07Call = 0x79a0;
                ECLStdExec(ecl, st2PostMaple, 1, 1);
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2MainLatter, 60, 90);
                ECLJump(ecl, st2MainLatterWait, mainSub07Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub08Call = 0x79ec;
                ECLStdExec(ecl, st2PostMaple, 1, 1);
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2MainLatter, 60, 90);
                ECLJump(ecl, st2MainLatterWait, mainSub08Call, 0, 0);
                break;
            }
            case 6: {
                constexpr unsigned int mainSub09Call = 0x7a4c;
                ECLStdExec(ecl, st2PostMaple, 1, 1);
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2MainLatter, 60, 90);
                ECLJump(ecl, st2MainLatterWait, mainSub09Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 3) {
            constexpr unsigned int st3MainFront = 0xbbb4;
            constexpr unsigned int st3MainSub00 = 0x904c;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub01Call = 0x90ac;
                ECLJump(ecl, st3PostMaple, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub01Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub03Call = 0x916c;
                ECLJump(ecl, st3PostMaple, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub03Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub04Call = 0x91cc;
                ECLJump(ecl, st3PostMaple, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub04Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub05Call = 0x922c;
                ECLStdExec(ecl, st3PostMaple, 1, 1);
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub05Call, 0, 0);
                break;
            }
            case 6: {
                constexpr unsigned int mainSub06Call = 0x928c;
                ECLStdExec(ecl, st3PostMaple, 1, 1);
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub06Call, 0, 0);
                break;
            }
            case 7: {
                constexpr unsigned int mainSub07Call = 0x92ec;
                ECLStdExec(ecl, st3PostMaple, 1, 1);
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub07Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 4) {
            constexpr unsigned int st4MainFront = 0xad38;
            constexpr unsigned int st4MainSub00 = 0x3a5c;
            constexpr unsigned int st4MainLatter = 0xae00;
            constexpr unsigned int st4MainLatterWait = 0x3d04;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub01Call = 0x3abc;
                ECLJump(ecl, st4PostMaple, st4MainFront, 60, 90);
                ECLJump(ecl, st4MainSub00, mainSub01Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub02Call = 0x3b1c;
                ECLJump(ecl, st4PostMaple, st4MainFront, 60, 90);
                ECLJump(ecl, st4MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub04Call = 0x3bdc;
                ECLJump(ecl, st4PostMaple, st4MainFront, 60, 90);
                ECLJump(ecl, st4MainSub00, mainSub04Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub05Call = 0x3c3c;
                ECLJump(ecl, st4PostMaple, st4MainFront, 60, 90);
                ECLJump(ecl, st4MainSub00, mainSub05Call, 0, 0);
                break;
            }
            case 6: {
                constexpr unsigned int mainSub07Call = 0x3d18;
                ECLStdExec(ecl, st4PostMaple, 1, 1);
                ECLJump(ecl, st4PostMaple + stdInterruptSize, st4MainLatter, 60, 90);
                ECLJump(ecl, st4MainLatterWait, mainSub07Call, 0, 0);
                break;
            }
            case 7: {
                constexpr unsigned int mainSub08Call = 0x3d78;
                ECLStdExec(ecl, st4PostMaple, 1, 1);
                ECLJump(ecl, st4PostMaple + stdInterruptSize, st4MainLatter, 60, 90);
                ECLJump(ecl, st4MainLatterWait, mainSub08Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 5) {
            constexpr unsigned int st5MainFront = 0x5650;
            constexpr unsigned int st5MainSub00 = 0x29c0;
            constexpr unsigned int st5MainLatter = 0x5718;
            constexpr unsigned int st5MainLatterWait = 0x2c08;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub01Call = 0x2a20;
                ECLJump(ecl, st5PostMaple, st5MainFront, 60, 90);
                ECLJump(ecl, st5MainSub00, mainSub01Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub02Call = 0x2a80;
                ECLJump(ecl, st5PostMaple, st5MainFront, 60, 90);
                ECLJump(ecl, st5MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub03Call = 0x2ae0;
                ECLJump(ecl, st5PostMaple, st5MainFront, 60, 90);
                ECLJump(ecl, st5MainSub00, mainSub03Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub04Call = 0x2b40;
                ECLJump(ecl, st5PostMaple, st5MainFront, 60, 90);
                ECLJump(ecl, st5MainSub00, mainSub04Call, 0, 0);
                break;
            }
            case 6: {
                constexpr unsigned int mainSub06Call = 0x2c1c;
                ECLJump(ecl, st5PostMaple + stdInterruptSize, st5MainLatter, 60, 90);
                ECLJump(ecl, st5MainLatterWait, mainSub06Call, 0, 0);
                break;
            }
            case 7: {
                constexpr unsigned int mainSub07Call = 0x2cb4;
                ECLStdExec(ecl, st5PostMaple, 1, 1);
                ECLJump(ecl, st5PostMaple + stdInterruptSize, st5MainLatter, 60, 90);
                ECLJump(ecl, st5MainLatterWait, mainSub07Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 6) {
            constexpr unsigned int st6MainFront = 0x5178;
            constexpr unsigned int st6MainSub00 = 0x20bc;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub01Call = 0x211c;
                ECLJump(ecl, st6PostMaple, st6MainFront, 60, 90);
                ECLJump(ecl, st6MainSub00, mainSub01Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub02Call = 0x217c;
                ECLJump(ecl, st6PostMaple, st6MainFront, 60, 90);
                ECLJump(ecl, st6MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub03Call = 0x21dc;
                ECLJump(ecl, st6PostMaple, st6MainFront, 60, 90);
                ECLJump(ecl, st6MainSub00, mainSub03Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub05Call = 0x229c;
                ECLStdExec(ecl, st6PostMaple, 1, 1);
                ECLJump(ecl, st6PostMaple + stdInterruptSize, st6MainFront, 60, 90);
                ECLJump(ecl, st6MainSub00, mainSub05Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 7) {
            constexpr unsigned int st7MainFront = 0x72b8;
            constexpr unsigned int st7MainSub00 = 0x3954;
            constexpr unsigned int st7MainLatter = 0x73b0;
            constexpr unsigned int st7MainLatterWait = 0x3af4;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub01Call = 0x39b4;
                ECLJump(ecl, st7PostMaple, st7MainFront, 60, 90);
                ECLJump(ecl, st7MainSub00, mainSub01Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub02Call = 0x3a14;
                ECLJump(ecl, st7PostMaple, st7MainFront, 60, 90);
                ECLJump(ecl, st7MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub03Call = 0x3a74;
                ECLJump(ecl, st7PostMaple, st7MainFront, 60, 90);
                ECLJump(ecl, st7MainSub00, mainSub03Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub04Call = 0x3b08;
                ECLStdExec(ecl, st7PostMaple, 1, 1);
                ECLJump(ecl, st7PostMaple + stdInterruptSize, st7MainLatter, 60, 90);
                ECLJump(ecl, st7MainLatterWait, mainSub04Call, 0, 0);
                break;
            }
            case 6: {
                constexpr unsigned int mainSub05Call = 0x3b68;
                ECLStdExec(ecl, st7PostMaple, 1, 1);
                ECLJump(ecl, st7PostMaple + stdInterruptSize, st7MainLatter, 60, 90);
                ECLJump(ecl, st7MainLatterWait, mainSub05Call, 0, 0);
                break;
            }
            case 7: {
                constexpr unsigned int mainSub06Call = 0x3bec;
                ECLStdExec(ecl, st7PostMaple, 1, 1);
                ECLJump(ecl, st7PostMaple + stdInterruptSize, st7MainLatter, 60, 90);
                ECLJump(ecl, st7MainLatterWait, mainSub06Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } 
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        constexpr unsigned int st1BossCreateCall = 0x8680;
        constexpr unsigned int st1bsPrePushSpellID = 0x3b8;
        constexpr unsigned int st1bsPostNotSpellPracCheck = 0x4a0;
        constexpr unsigned int st1bsSpellHealthVal = 0x4b0;
        constexpr unsigned int st1bsSpellSubCallOrd = 0x4d0;

        constexpr unsigned int st2BossCreateCall = 0xabf0;
        constexpr unsigned int st2bsPrePushSpellID = 0x3d0;
        constexpr unsigned int st2bsPostNotSpellPracCheck = 0x4b8;
        constexpr unsigned int st2bsSpellHealthVal = 0x4c8;
        constexpr unsigned int st2bsSpellSubCallOrd = 0x4e8;
        constexpr unsigned int st2bsNonSubCallOrd = 0x6f4 + 0x18;

        constexpr unsigned int st3BossCreateCall = 0xbc74;
        constexpr unsigned int st3bsPrePushSpellID = 0x458;
        constexpr unsigned int st3bsPostNotSpellPracCheck = 0x540;
        constexpr unsigned int st3bsSpellHealthVal = 0x550;
        constexpr unsigned int st3bsSpellSubCallOrd = 0x570;
        constexpr unsigned int st3bsNonSubCallOrd = 0x880 + 0x18;

        constexpr unsigned int st4MBossCreateCall = 0xadbc;
        constexpr unsigned int st4mbsPreChargeAnim = 0x310;
        constexpr unsigned int st4mbsPostChargeAnim = 0x3a8;
        constexpr unsigned int st4mbsPreWait = 0x3c8;
        constexpr unsigned int st4mbsPostWait = 0x3dc;
        constexpr unsigned int st4mbsPreWait2 = 0x47c;
        constexpr unsigned int st4mbsPostWait2 = 0x490;

        constexpr unsigned int st4BossCreateCall = 0xaed4;
        constexpr unsigned int st4bsPrePushSpellID = 0x41c;
        constexpr unsigned int st4bsPostNotSpellPracCheck = 0x504;
        constexpr unsigned int st4bsSpellHealthVal = 0x514;
        constexpr unsigned int st4bsSpellSubCallOrd = 0x534;
        constexpr unsigned int st4bsNonSubCallOrd = 0x844 + 0x18;

        constexpr unsigned int st5BossCreateCall = 0x57ec;
        constexpr unsigned int st5bsPrePushSpellID = 0x424;
        constexpr unsigned int st5bsPostNotSpellPracCheck = 0x50c;
        constexpr unsigned int st5bsSpellHealthVal = 0x51c;
        constexpr unsigned int st5bsSpellSubCallOrd = 0x53c;
        constexpr unsigned int st5bsNonSubCallOrd = 0x84c + 0x18;

        
        constexpr unsigned int st6BossCreateCall = 0x52c4;
        constexpr unsigned int st6bsPrePushSpellID = 0x534;
        constexpr unsigned int st6bsPostNotSpellPracCheck = 0x61c;
        constexpr unsigned int st6bsSpellHealthVal = 0x62c;
        constexpr unsigned int st6bsSpellSubCallOrd = 0x64c;
        constexpr unsigned int st6bsNonSubCallOrd = 0xc88 + 0x18;
        constexpr unsigned int st6bsSetYPos = 0x324 + 0x14;

        
        constexpr unsigned int st7MBossCreateCall = 0x7324;
        constexpr unsigned int st7MBossPreMsg = 0x7354;
        constexpr unsigned int st7MBossPostMsg = 0x73a0;
        constexpr unsigned int st7mbsPrePushSpellID = 0x464;
        constexpr unsigned int st7mbsPostNotSpellPracCheck = 0x4b4;
        constexpr unsigned int st7mbsNonSubCallOrd = 0x4c8 + 0x1d;

        constexpr unsigned int st7BossCreateCall = 0x7484;
        constexpr unsigned int st7bsPrePushSpellID = 0x5d0;
        constexpr unsigned int st7bsPostNotSpellPracCheck = 0x6b8;
        constexpr unsigned int st7bsSpellHealthVal = 0x6b8 + 0x10;
        constexpr unsigned int st7bsSpellSubCallOrd = 0x6cc + 0x1c;
        constexpr unsigned int st7bsNonSubCallOrd = 0x1010 + 0x18;
        constexpr unsigned int st7bsSetYPos = 0x3c0 + 0x14;


        switch (section) {
        case THPrac::TH20::TH20_ST1_MID1: {
            constexpr unsigned int st1MBossCreateCall = 0x8568;
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1MBossCreateCall, 60, 90);
            break;
        }
        case THPrac::TH20::TH20_ST1_BOSS1: {
            constexpr unsigned int st1BossDialogueCall = 0x866c;
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossDialogueCall, 60);
            else
                ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST1_BOSS2: {
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st1bsPrePushSpellID, st1bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST1_BOSS3: {
            constexpr unsigned int st1bsNonSubCallOrd = 0x6dc + 0x18;
            constexpr unsigned int st1bsNon2InvulnCallVal = 0x133c + 0x10;
            constexpr unsigned int st1bsNon2BossItemCallOp = 0x1428 + 0x4;
            constexpr unsigned int st1bsNon2PlaySoundOp = 0x1554 + 0x4;
            constexpr unsigned int st1bsNon2PostLifeMarker = 0x16c0;
            constexpr unsigned int st1bsNon2PostWait = 0x17ac; // 0x1608 previously

            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st1bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st1bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st1bsNon2BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st1bsNon2PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st1bsNon2PostLifeMarker, st1bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST1_BOSS4: {
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st1bsPrePushSpellID, st1bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st1bsSpellHealthVal, 2000 }; // Set correct health (set in skipped non)
            ecl << pair { st1bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST1_BOSS5: {
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st1bsPrePushSpellID, st1bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st1bsSpellHealthVal, 2500 }; // Set correct health (set in skipped non)
            ecl << pair { st1bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }

        case THPrac::TH20::TH20_ST2_MID1: {
            constexpr unsigned int st2MBossCreateCall = 0xaad8;
            ECLJump(ecl, st2PostMaple, st2MBossCreateCall, 60, 90);
            break;
        }
        case THPrac::TH20::TH20_ST2_BOSS1: {
            constexpr unsigned int st2BossDialogueCall = 0xabdc;
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossDialogueCall, 60);
            else
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST2_BOSS2: {
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st2bsPrePushSpellID, st2bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST2_BOSS3: {
            constexpr unsigned int st2bsNon2InvulnCallVal = 0x12a8 + 0x10;
            constexpr unsigned int st2bsNon2BossItemCallOp = 0x1394 + 0x4;
            constexpr unsigned int st2bsNon2PlaySoundOp = 0x14c0 + 0x4;
            constexpr unsigned int st2bsNon2PostLifeMarker = 0x162c;
            constexpr unsigned int st2bsNon2PostWait = 0x1718;

            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st2bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st2bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st2bsNon2BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st2bsNon2PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st2bsNon2PostLifeMarker, st2bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST2_BOSS4: {
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st2bsPrePushSpellID, st2bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st2bsSpellHealthVal, 2600 }; // Set correct health (set in skipped non)
            ecl << pair { st2bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST2_BOSS5: {
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st2bsPrePushSpellID, st2bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st2bsSpellHealthVal, 3000 }; // Set correct health (set in skipped non)
            ecl << pair { st2bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }

        case THPrac::TH20::TH20_ST3_BOSS1: {
            constexpr unsigned int st3BossDialogueCall = 0xbc60;
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossDialogueCall, 60);
            else
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS2: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS3: {
            constexpr unsigned int st3bsNon2InvulnCallVal = 0x127c + 0x10;
            constexpr unsigned int st3bsNon2BossItemCallOp = 0x1368 + 0x4;
            constexpr unsigned int st3bsNon2PlaySoundOp = 0x1494 + 0x4;
            constexpr unsigned int st3bsNon2PostLifeMarker = 0x1600;
            constexpr unsigned int st3bsNon2PostWait = 0x172c;

            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st3bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st3bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st3bsNon2BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st3bsNon2PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st3bsNon2PostLifeMarker, st3bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS4: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st3bsSpellHealthVal, 3200 }; // Set correct health (set in skipped non)
            ecl << pair { st3bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS5: {
            constexpr unsigned int st3bsNon3InvulnCallVal = 0x1f28 + 0x10;
            constexpr unsigned int st3bsNon3BossItemCallOp = 0x2014 + 0x4;
            constexpr unsigned int st3bsNon3PlaySoundOp = 0x2140 + 0x4;
            constexpr unsigned int st3bsNon3PostLifeCount = 0x22c0; // is this right? life count = 0? zun...
            constexpr unsigned int st3bsNon3PostWait = 0x23ec;

            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st3bsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            ecl << pair { st3bsNon3InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st3bsNon3BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st3bsNon3PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st3bsNon3PostLifeCount, st3bsNon3PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS6: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st3bsSpellHealthVal, 3400 }; // Set correct health (set in skipped non)
            ecl << pair { st3bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS7: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st3bsSpellHealthVal, 3400 }; // Set correct health (set in skipped non)
            ecl << pair { st3bsSpellSubCallOrd, (int8_t)0x34 }; // Set spell ID in sub call to '4'
            break;
        }

        case THPrac::TH20::TH20_ST4_MID1: {
            ECLJump(ecl, st4PostMaple, st4MBossCreateCall, 60, 90);
            ecl.SetFile(3);
            ECLJump(ecl, st4mbsPreChargeAnim, st4mbsPostChargeAnim, 0);
            ECLJump(ecl, st4mbsPreWait, st4mbsPostWait, 0);
            ECLJump(ecl, st4mbsPreWait2, st4mbsPostWait2, 0);
            break;
        }
        case THPrac::TH20::TH20_ST4_MID2: {
            constexpr unsigned int st4mbsNonSubCallOrd = 0x4a4 + 0x19;
            constexpr unsigned int st4mbsNon2BossItemCallOp = 0xd40 + 0x4;
            constexpr unsigned int st4mbsNon2PlaySoundOp = 0xe6c + 0x4;
            constexpr unsigned int st4mbsNon2PreWait = 0x1028;
            constexpr unsigned int st4mbsNon2PostWait = 0x103c;
            constexpr unsigned int st4mbsNon2InvincTime = 0xc54 + 0x10;
            constexpr unsigned int st4mbsNon2Timer = 0xfc4 + 0x18;
            constexpr unsigned int st4mbsNon2BulletClear = 0x1a8 + 0x4;

            ECLJump(ecl, st4PostMaple, st4MBossCreateCall, 60, 90);
            ecl.SetFile(3);
            ECLJump(ecl, st4mbsPreChargeAnim, st4mbsPostChargeAnim, 0);
            ecl << pair { st4mbsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st4mbsNon2BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st4mbsNon2PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st4mbsNon2BulletClear, (int16_t)0 }; // Disable bullet clear
            ECLJump(ecl, st4mbsNon2PreWait, st4mbsNon2PostWait, 0); // Skip wait (100f)
            ecl << pair { st4mbsNon2InvincTime, (int16_t)20 }; // Reduce invincible timer by time skipped (120f->20f)
            ecl << pair { st4mbsNon2Timer, (int16_t)380 }; // Reduce boss timer by time skipped (480f->380f)
            ECLJump(ecl, st4mbsPreWait, st4mbsPostWait, 0);
            ECLJump(ecl, st4mbsPreWait2, st4mbsPostWait2, 0);
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS1: {
            constexpr unsigned int st4BossDialogueCall = 0xaec0;
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossDialogueCall, 60);
            else
                ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS2: {
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st4bsPrePushSpellID, st4bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS3: {
            constexpr unsigned int st4bsNon2InvulnCallVal = 0x1540 + 0x10;
            constexpr unsigned int st4bsNon2BossItemCallOp = 0x162c + 0x4;
            constexpr unsigned int st4bsNon2PlaySoundOp = 0x1758 + 0x4;
            constexpr unsigned int st4bsNon2PostLifeMarker = 0x18c4;
            constexpr unsigned int st4bsNon2PostWait1 = 0x1970;
            constexpr unsigned int st4bsNon2PostInit = 0x19e8;
            constexpr unsigned int st4bsNon2PostWait2 = 0x1a40;

            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st4bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st4bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st4bsNon2BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st4bsNon2PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st4bsNon2PostLifeMarker, st4bsNon2PostWait1, 0); // Skip wait 1
            ECLJump(ecl, st4bsNon2PostInit, st4bsNon2PostWait2, 0); // Skip wait 2
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS4: {
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st4bsPrePushSpellID, st4bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st4bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS5: {
            constexpr unsigned int st4bsNon3InvulnCallVal = 0x2584 + 0x10;
            constexpr unsigned int st4bsNon3BossItemCallOp = 0x2670 + 0x4;
            constexpr unsigned int st4bsNon3PlaySoundOp = 0x279c + 0x4;
            constexpr unsigned int st4bsNon3PostLifeCount = 0x291c;
            constexpr unsigned int st4bsNon3PostWait1 = 0x29c8;
            constexpr unsigned int st4bsNon3PostInit = 0x2a40;
            constexpr unsigned int st4bsNon3PostWait2 = 0x2a98;

            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st4bsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            ecl << pair { st4bsNon3InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st4bsNon3BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st4bsNon3PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st4bsNon3PostLifeCount, st4bsNon3PostWait1, 0); // Skip wait 1
            ECLJump(ecl, st4bsNon3PostInit, st4bsNon3PostWait2, 0); // Skip wait 2
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS6: {
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st4bsPrePushSpellID, st4bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st4bsSpellHealthVal, 2500 }; // Set correct health (set in skipped non)
            ecl << pair { st4bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS7: {
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st4bsPrePushSpellID, st4bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st4bsSpellHealthVal, 4300 }; // Set correct health (set in skipped non)
            ecl << pair { st4bsSpellSubCallOrd, (int8_t)0x34 }; // Set spell ID in sub call to '4'
            break;
        }
        case THPrac::TH20::TH20_ST5_MID1: {
            constexpr unsigned int st5MBossCreateCall = 0x56d4;
            constexpr unsigned int st5mbsPreChargeAnim = 0x33c;
            constexpr unsigned int st5mbsPostChargeAnim = 0x3d4;
            constexpr unsigned int st5mbsPreWait = 0x3f4;
            constexpr unsigned int st5mbsPostWait = 0x408;
            constexpr unsigned int st5mbsPreWait2 = 0x474;
            constexpr unsigned int st5mbsPostWait2 = 0x488;

            ECLJump(ecl, st5PostMaple, st5MBossCreateCall, 0);
            ecl.SetFile(3);
            ECLJump(ecl, st5mbsPreChargeAnim, st5mbsPostChargeAnim, 0);
            ECLJump(ecl, st5mbsPreWait, st5mbsPostWait, 0);
            ECLJump(ecl, st5mbsPreWait2, st5mbsPostWait2, 0);
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS1: {
            constexpr unsigned int st5BossDialogueCall = 0x57d8;
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossDialogueCall, 60);
            else
                ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS2: {
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st5bsPrePushSpellID, st5bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS3: {
            constexpr unsigned int st5bsNon2InvulnCallVal = 0x1470 + 0x10;
            constexpr unsigned int st5bsNon2BossItemCallOp = 0x155c + 0x4;
            constexpr unsigned int st5bsNon2PlaySoundOp = 0x1688 + 0x4;
            constexpr unsigned int st5bsNon2PostLifeMarker = 0x17f4;
            constexpr unsigned int st5bsNon2PostWait = 0x18a0;

            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st5bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st5bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st5bsNon2BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st5bsNon2PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st5bsNon2PostLifeMarker, st5bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS4: {
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st5bsPrePushSpellID, st5bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st5bsSpellHealthVal, 3200 }; // Set correct health (set in skipped non)
            ecl << pair { st5bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS5: {
            constexpr unsigned int st5bsNon3InvulnCallVal = 0x2498 + 0x10;
            constexpr unsigned int st5bsNon3BossItemCallOp = 0x2584 + 0x4;
            constexpr unsigned int st5bsNon3PlaySoundOp = 0x26b0 + 0x4;
            constexpr unsigned int st5bsNon3PostLifeCount = 0x2830;
            constexpr unsigned int st5bsNon3PostWait = 0x28c8;

            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st5bsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            ecl << pair { st5bsNon3InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st5bsNon3BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st5bsNon3PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st5bsNon3PostLifeCount, st5bsNon3PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS6: {
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st5bsPrePushSpellID, st5bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st5bsSpellHealthVal, 3200 }; // Set correct health (set in skipped non)
            ecl << pair { st5bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS7: {
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st5bsPrePushSpellID, st5bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st5bsSpellHealthVal, 3200 }; // Set correct health (set in skipped non)
            ecl << pair { st5bsSpellSubCallOrd, (int8_t)0x34 }; // Set spell ID in sub call to '4'
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS1: {
            constexpr unsigned int st6BossDialogueCall = 0x52b0;
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossDialogueCall, 60);
            else
                ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS2: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS3: {
            constexpr unsigned int st6bsNon2InvulnCallVal = 0x1448 + 0x10;
            constexpr unsigned int st6bsNon2BossItemCallOp = 0x1534 + 0x4;
            constexpr unsigned int st6bsNon2PlaySoundOp = 0x1660 + 0x4;
            constexpr unsigned int st6bsNon2PostProtectRange = 0x17e0;
            constexpr unsigned int st6bsNon2PostWait = 0x188c;

            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st6bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st6bsNon2BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st6bsNon2PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st6bsSetYPos, (float)128.0f }; // Fix boss starting too high
            ECLJump(ecl, st6bsNon2PostProtectRange, st6bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS4: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS5: {
            constexpr unsigned int st6bsNon3InvulnCallVal = 0x1ec8 + 0x10;
            constexpr unsigned int st6bsNon3BossItemCallOp = 0x21f8 + 0x4;
            constexpr unsigned int st6bsNon3PlaySoundOp = 0x1660 + 0x4;
            constexpr unsigned int st6bsNon3PostLifeCount = 0x2300;
            constexpr unsigned int st6bsNon3PostWait = 0x2398;

            constexpr unsigned int st6bsNon3PreDialogue = 0x225c;
            constexpr unsigned int st6bsNon3PostDialogue = 0x2294;

            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            ecl << pair { st6bsNon3BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st6bsNon3PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st6bsSetYPos, (float)128.0f }; // Fix boss starting too high
            ECLJump(ecl, st6bsNon3PostLifeCount, st6bsNon3PostWait, 0); // Skip wait
            if (!thPracParam.dlg) {
                ecl << pair { st6bsNon3InvulnCallVal, (int16_t)0 }; // Disable Invincible
                ECLJump(ecl, st6bsNon3PreDialogue, st6bsNon3PostDialogue, 0); // Skip dialogue
            }
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS6: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellHealthVal, 4000 }; // Set correct health (set in skipped non)
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS7: {
            constexpr unsigned int st6bsNon4InvulnCallVal = 0x2b78 + 0x10;
            constexpr unsigned int st6bsNon4BossItemCallOp = 0x2c64 + 0x4;
            constexpr unsigned int st6bsNon4PlaySoundOp = 0x2d90 + 0x4;
            constexpr unsigned int st6bsNon4PostProtectRange = 0x2f10;
            constexpr unsigned int st6bsNon4PostWait = 0x2fbc;

            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsNonSubCallOrd, (int8_t)0x34 }; // Set nonspell ID in sub call to '4'
            ecl << pair { st6bsNon4InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st6bsNon4BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st6bsNon4PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st6bsSetYPos, (float)144.0f }; // Fix boss starting too high
            ECLJump(ecl, st6bsNon4PostProtectRange, st6bsNon4PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS8: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellHealthVal, 4000 }; // Set correct health (set in skipped non)
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x34 }; // Set spell ID in sub call to '4'
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS9: {
            constexpr unsigned int st6bsNon5InvulnCallVal = 0x38a0 + 0x10;
            constexpr unsigned int st6bsNon5BossItemCallOp = 0x39a4 + 0x4;
            constexpr unsigned int st6bsNon5PlaySoundOp = 0x3ad0 + 0x4;
            constexpr unsigned int st6bsNon5PostLifeMarker = 0x3cf8;
            constexpr unsigned int st6bsNon5PostWait = 0x3db8;

            constexpr unsigned int st6bsNon5PreDialogue = 0x3c28;
            constexpr unsigned int st6bsNon5PostDialogue = 0x3ca0;

            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsNonSubCallOrd, (int8_t)0x35 }; // Set nonspell ID in sub call to '5'
            ecl << pair { st6bsNon5BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st6bsNon5PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st6bsSetYPos, (float)192.0f }; // Fix boss starting too high
            ECLJump(ecl, st6bsNon5PostLifeMarker, st6bsNon5PostWait, 0); // Skip wait
            if (!thPracParam.dlg) {
                ecl << pair { st6bsNon5InvulnCallVal, (int16_t)0 }; // Disable Invincible
                ECLJump(ecl, st6bsNon5PreDialogue, st6bsNon5PostDialogue, 0); // Skip dialogue
            }
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS10: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellHealthVal, 4000 }; // Set correct health (set in skipped non)
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x35 }; // Set spell ID in sub call to '5'
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS11: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellHealthVal, 4500 }; // Set correct health (set in skipped non)
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x36 }; // Set spell ID in sub call to '6'
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS12: {
            auto disableWait = [&ecl]() -> void { 
                ecl << pair { 0x3d4 + 0x4, (int16_t)0 }; 
            };
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellHealthVal, 12000 }; // Set correct health (set in skipped non)
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x37 }; // Set spell ID in sub call to '7'
            disableWait();
            
            switch (thPracParam.phase) {
            case 1:
                ecl << pair { 0xa7b0 + 0x10, 5400 };
                break;
            case 2:
                ecl << pair { 0xa7b0 + 0x10, 5400 };
                ecl << pair { 0xaac8 + 0x10, 1800 };
                break;
            case 3:
                ecl << pair { 0xa7b0 + 0x10, 5400 };
                ecl << pair { 0xaac8 + 0x10, 1800 };
                ecl << pair { 0xae04 + 0x10, 1800 };
                break;
            default:
                break;
            }
            break;
        }
        case THPrac::TH20::TH20_ST7_MID1: {
            ECLJump(ecl, st7PostMaple, st7MBossCreateCall, 60, 90);
            if (thPracParam.dlg)
                break;

            ECLJump(ecl, st7MBossPreMsg, st7MBossPostMsg, 60, 90);
            ecl.SetFile(3);
            ECLJump(ecl, st7mbsPrePushSpellID, st7mbsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST7_MID2: {
            ECLJump(ecl, st7PostMaple, st7MBossCreateCall, 60, 90);
            ECLJump(ecl, st7MBossPreMsg, st7MBossPostMsg, 60, 90);
            ecl.SetFile(3);
            ECLJump(ecl, st7mbsPrePushSpellID, st7mbsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7mbsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST7_MID3: {
            ECLJump(ecl, st7PostMaple, st7MBossCreateCall, 60, 90);
            ECLJump(ecl, st7MBossPreMsg, st7MBossPostMsg, 60, 90);
            ecl.SetFile(3);
            ECLJump(ecl, st7mbsPrePushSpellID, st7mbsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7mbsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS1: {
            constexpr unsigned int st7BossDialogueCall = 0x7470;
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossDialogueCall, 60);
            else
                ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS2: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS3: {
            constexpr unsigned int st7bsNon2InvulnCallVal = 0x1ba4 + 0x10;
            constexpr unsigned int st7bsNon2BossItemCallOp = 0x1c88 + 0x4;
            constexpr unsigned int st7bsNon2PlaySoundOp = 0x1db4 + 0x4;
            constexpr unsigned int st7bsNon2PostProtectRange = 0x1f74;
            constexpr unsigned int st7bsNon2PostWait = 0x2020;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st7bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon2BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon2PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Avoid boss move
            ECLJump(ecl, st7bsNon2PostProtectRange, st7bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS4: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS5: {
            constexpr unsigned int st7bsNon3InvulnCallVal = 0x2618 + 0x10;
            constexpr unsigned int st7bsNon3BossItemCallOp = 0x26fc + 0x4;
            constexpr unsigned int st7bsNon3PlaySoundOp = 0x2828 + 0x4;
            constexpr unsigned int st7bsNon3PostProtectRange = 0x29e8;
            constexpr unsigned int st7bsNon3PostWait = 0x2a94;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            ecl << pair { st7bsNon3InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon3BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon3PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Avoid boss move
            ECLJump(ecl, st7bsNon3PostProtectRange, st7bsNon3PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS6: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS7: {
            constexpr unsigned int st7bsNon4InvulnCallVal = 0x308c + 0x10;
            constexpr unsigned int st7bsNon4BossItemCallOp = 0x3170 + 0x4;
            constexpr unsigned int st7bsNon4PlaySoundOp = 0x329c + 0x4;
            constexpr unsigned int st7bsNon4PostProtectRange = 0x345c;
            constexpr unsigned int st7bsNon4PostWait = 0x3508;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x34 }; // Set nonspell ID in sub call to '4'
            ecl << pair { st7bsNon4InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon4BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon4PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Avoid boss move
            ECLJump(ecl, st7bsNon4PostProtectRange, st7bsNon4PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS8: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x34 }; // Set spell ID in sub call to '4'
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS9: {
            constexpr unsigned int st7bsNon5InvulnCallVal = 0x3b00 + 0x10;
            constexpr unsigned int st7bsNon5BossItemCallOp = 0x3be4 + 0x4;
            constexpr unsigned int st7bsNon5PlaySoundOp = 0x3d10 + 0x4;
            constexpr unsigned int st7bsNon5PostProtectRange = 0x3e90;
            constexpr unsigned int st7bsNon5PostWait = 0x3f7c;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x35 }; // Set nonspell ID in sub call to '5'
            ecl << pair { st7bsNon5InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon5BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon5PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Avoid boss move
            ECLJump(ecl, st7bsNon5PostProtectRange, st7bsNon5PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS10: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x35 }; // Set spell ID in sub call to '5'
            ecl << pair { st7bsSpellHealthVal, 5000 }; // Set correct health (set in skipped non)
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS11: {
            constexpr unsigned int st7bsNon6InvulnCallVal = 0x4574 + 0x10;
            constexpr unsigned int st7bsNon6BossItemCallOp = 0x4658 + 0x4;
            constexpr unsigned int st7bsNon6PlaySoundOp = 0x4784 + 0x4;
            constexpr unsigned int st7bsNon6PostProtectRange = 0x4944;
            constexpr unsigned int st7bsNon6PostWait = 0x49f0;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x36 }; // Set nonspell ID in sub call to '6'
            ecl << pair { st7bsNon6InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon6BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon6PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Avoid boss move
            ECLJump(ecl, st7bsNon6PostProtectRange, st7bsNon6PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS12: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x36 }; // Set spell ID in sub call to '6'
            ecl << pair { st7bsSpellHealthVal, 5000 }; // Set correct health (set in skipped non)
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS13: {
            constexpr unsigned int st7bsNon7InvulnCallVal = 0x4fe8 + 0x10;
            constexpr unsigned int st7bsNon7BossItemCallOp = 0x50cc + 0x4;
            constexpr unsigned int st7bsNon7PlaySoundOp = 0x51f8 + 0x4;
            constexpr unsigned int st7bsNon7PostProtectRange = 0x53b8;
            constexpr unsigned int st7bsNon7PostWait = 0x5464;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x37 }; // Set nonspell ID in sub call to '7'
            ecl << pair { st7bsNon7InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon7BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon7PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Avoid boss move
            ECLJump(ecl, st7bsNon7PostProtectRange, st7bsNon7PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS14: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x37 }; // Set spell ID in sub call to '7'
            ecl << pair { st7bsSpellHealthVal, 3500 }; // Set correct health (set in skipped non)
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS15: {
            constexpr unsigned int st7bsNon8InvulnCallVal = 0x5a5c + 0x10;
            constexpr unsigned int st7bsNon8BossItemCallOp = 0x5b40 + 0x4;
            constexpr unsigned int st7bsNon8PlaySoundOp = 0x5c6c + 0x4;
            constexpr unsigned int st7bsNon8PostProtectRange = 0x5e2c;
            constexpr unsigned int st7bsNon8PostWait = 0x5ef8;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x38 }; // Set nonspell ID in sub call to '8'
            ecl << pair { st7bsNon8InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon8BossItemCallOp, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon8PlaySoundOp, (int16_t)0 }; // Disable sound effect
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Avoid boss move
            ECLJump(ecl, st7bsNon8PostProtectRange, st7bsNon8PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS16: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x38 }; // Set spell ID in sub call to '8'
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Avoid boss move
            ecl << pair { st7bsSpellHealthVal, 6000 }; // Set correct health (set in skipped non)
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS17: {
            constexpr unsigned int st7BossSpell9Duration = 0xdd50 + 0x18;
            constexpr unsigned int st7BossSpell9ChoccyPostSetup = 0xe150;
            constexpr unsigned int st7BossSpell9ChoccyPreFinale = 0xedb8;
            constexpr unsigned int st7BossSpell9BulletsPostSetup = 0xf68c;
            constexpr unsigned int st7BossSpell9BulletsPreFinale = 0xfcb0;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x39 }; // Set spell ID in sub call to '9'
            ecl << pair { st7bsSpellHealthVal, 6000 }; // Set correct health (set in skipped non)

            if (thPracParam.phase == 1) { // Finale
                ecl << pair { st7BossSpell9Duration, 5040 - 3400 + 90 }; // Adjust duration (we skipped 3400f worth of waits in the attacks but the boss waits for 90 before attacking)
                ECLJump(ecl, st7BossSpell9ChoccyPostSetup, st7BossSpell9ChoccyPreFinale, 0);
                ECLJump(ecl, st7BossSpell9BulletsPostSetup, st7BossSpell9BulletsPreFinale, 0);
            }

            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS18: {
            constexpr unsigned int st7BossSpell10PostCheck = 0xebc;
            constexpr unsigned int st7BossSpell10SpellHealthVal = 0xebc + 0x10;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7BossSpell10PostCheck, 1); // Utilize Spell Practice Jump

            switch (thPracParam.phase) {
            case 1: // P2
                ecl << pair { st7BossSpell10SpellHealthVal, 5300 };
                break;
            case 2: // P3
                ecl << pair { st7BossSpell10SpellHealthVal, 3200 };
                break;
            case 3: // P4
                ecl << pair { st7BossSpell10SpellHealthVal, 1500 };
                break;
            }

            break;
        }
        default:
            break;
        }

    }
    __declspec(noinline) void THSectionPatch()
    {
        ECLHelper ecl;
        ecl.SetBaseAddr(GetMemAddr<void*>(RVA(0x1BA570), 0x104, 0xc));

        auto section = thPracParam.section;
        if (section >= 10000 && section < 20000) {
            int stage = (section - 10000) / 100;
            int portionId = (section - 10000) % 100;
            THStageWarp(ecl, stage, portionId);
        } else {
            THPatch(ecl, (th_sections_t)section);
        }
    }

    int THBGMTest()
    {
        if (!thPracParam.mode)
            return 0;
        else if (thPracParam.section >= 10000)
            return 0;
        else if (thPracParam.dlg && thPracParam.section != TH20_ST6_BOSS5 && thPracParam.section != TH20_ST6_BOSS9)
            return 0;
        else
            return th_sections_bgm[thPracParam.section];
    }
    void THSaveReplay(char* repName)
    {
        thPracParam.MarkExpStoneFixed(GetMemContent(RVA(STAGE_NUM)) == 7); // consistent behavior guaranteed for new replays
        ReplaySaveParam(mb_to_utf16(repName, 932).c_str(), thPracParam.GetJson());
    }

    void __fastcall AnmVM_Reset_hook(uintptr_t self)
    {
        uint32_t fast_id = GetMemContent<uint32_t>(self + 0x4C4);
        memset((void*)self, 0, 0x5E4);
        *(uint32_t*)(self + 0x4C4) = fast_id;
        asm_call_rel<0x299D0, Thiscall>(self);
    }
    static char* sReplayPath = nullptr;
    static constinit HookCtx anmUninitFixHooks[] = {
        { .addr = 0x38974, .data = PatchCode("e800000000") },
        { .addr = 0x48411, .data = PatchCode("e800000000") },
        { .addr = 0x4CA14, .data = PatchCode("e800000000") },
        { .addr = 0x4CAB9, .data = PatchCode("e800000000") },
        { .addr = 0x4E2B3, .data = PatchCode("e800000000") },
        { .addr = 0x708EA, .data = PatchCode("e800000000") },
        { .addr = 0x7F1B1, .data = PatchCode("e800000000") },
        { .addr = 0x7F2F6, .data = PatchCode("e800000000") },
        { .addr = 0x82252, .data = PatchCode("e800000000") },
        { .addr = 0xD3208, .data = PatchCode("e800000000") },
        { .addr = 0xD32D2, .data = PatchCode("e800000000") },
        { .addr = 0xD3E57, .data = PatchCode("e800000000") },
        { .addr = 0xD4237, .data = PatchCode("e800000000") },
    };

    void RenderHits()
    {
        auto RotateVec = [](ImVec2 pt, ImVec2 mid, ImVec2 angle) -> ImVec2 {
            ImVec2 dif { pt.x - mid.x, pt.y - mid.y };
            float& cost = angle.x;
            float& sint = angle.y;
            return { mid.x + dif.x * cost - dif.y * sint, mid.y + dif.y * cost + dif.x * sint };
        };
        auto GetClientFromStage = [](ImVec2 stage) -> ImVec2 {
            float x = stage.x;
            float y = stage.y;
            
            y = y * 2.0f + 32.0f;
            x = x * 2.0f + 448.0f;
            ImGuiIO& io = ImGui::GetIO();
            return { x * io.DisplaySize.x / 1280.0f, y * io.DisplaySize.y / 960.0f };
        };
        auto GetClientFromStage2 = [](float f) -> float {
            ImGuiIO& io = ImGui::GetIO();
            return f * 2.0f * io.DisplaySize.x / 1280.0f;
        };

        auto p = ImGui::GetOverlayDrawList();
        DWORD patts = *(DWORD*)(RVA(0x1BA590));
        if (!patts)
            return;
        DWORD iter = *(DWORD*)(patts + 0xC414 + 0x4);
        while (iter) {
            DWORD patt = *(DWORD*)(iter);
            if(patt){
                if ((*(DWORD*)(patt + 0x14)) & 1) {
                   
                    if ((*(DWORD*)(patt + 0x14)) & 0x2) // round
                    {
                        float x = *(float*)(patt + 0x34);
                        float y = *(float*)(patt + 0x38);
                        ImVec2 cen = GetClientFromStage({ x, y });
                        float r = *(float*)(patt + 0x18);
                        p->AddCircleFilled(cen, GetClientFromStage2(r), 0x33FF0000);
                        p->AddCircle(cen, GetClientFromStage2(r), 0xFFFF0000);
                    } else {
                        float x = *(float*)(patt + 0x34);
                        float y = *(float*)(patt + 0x38);

                        float a = *(float*)(patt + 0x24);
                        float w = *(float*)(patt + 0x2C);
                        float h = *(float*)(patt + 0x30);

                        ImVec2 p1 { x - w / 2, y - h / 2 };
                        ImVec2 p2 { x + w / 2, y - h / 2 };
                        ImVec2 p3 { x + w / 2, y + h / 2 };
                        ImVec2 p4 { x - w / 2, y + h / 2 };

                        ImVec2 pm { x, y };
                        ImVec2 ang { cosf(a), sinf(a) };
                        p1 = RotateVec(p1, pm, ang);
                        p2 = RotateVec(p2, pm, ang);
                        p3 = RotateVec(p3, pm, ang);
                        p4 = RotateVec(p4, pm, ang);
                        p1 = GetClientFromStage(p1);
                        p2 = GetClientFromStage(p2);
                        p3 = GetClientFromStage(p3);
                        p4 = GetClientFromStage(p4);
                        p->AddQuadFilled(p1, p2, p3, p4, 0x33FF0000);
                        p->AddQuad(p1, p2, p3, p4, 0xFFFF0000);
                    }
                }
            }
            iter = *(DWORD*)(iter + 4);
        }
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th20_inf_lives, 0xe1288, 6,
        {
            int life_next = pCtx->Ecx;
            if (life_next >= *(int32_t*)(RVA(0x1BA6A8)))// life increased
                return;
            TH20InGameInfo::singleton().mMissCount++;
            TH20Save::singleton().pyraState.isPyraFailed = true;
            FastRetry(thPracParam.mode);

            if ((*(THOverlay::singleton().mInfLives))) {
                if (!g_adv_igi_options.map_inf_life_to_no_continue) {
                    pCtx->Eip = RVA(0xe128e);
                } else {
                    if (*(int32_t*)(RVA(0x1BA6A8)) == 0)
                        pCtx->Eip = RVA(0xe128e);
                }
            }
        })
    EHOOK_DY(th20_boss_bgm, 0xBAC98, 2, {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = RVA(0xBAC9A);
        }
    })
    EHOOK_DY(th20_everlasting_bgm_1, 0x28C90, 1, {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0] - ingame_image_base;
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        // 4th stack item = i32 call_addr
    
        bool el_switch = *(THOverlay::singleton().mElBgm)
            && !THGuiRep::singleton().mRepStatus && (thPracParam.mode == 1)
            && thPracParam.section;
        bool is_practice = (GetMemContent(RVA(MODEFLAGS)) & 0x1);
    
       bool result = ElBgmTest<0xD9B90, 0xD9BFE, 0xE60B8, 0xE64B8, 0xffffffff>(
            el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, 0xffffffff);
    
        if (result)
            pCtx->Eip = RVA(0x28DD5);
    })
    EHOOK_DY(th20_everlasting_bgm_2, 0xE5C91, 5, {
        bool el_switch = *(THOverlay::singleton().mElBgm)
            && !THGuiRep::singleton().mRepStatus && (thPracParam.mode == 1)
            && thPracParam.section;
        if (el_switch)
            pCtx->Eip = RVA(0xE5CF7);
    })
    EHOOK_DY(th20_patch_main, 0xBBD56, 1, {
        if (thPracParam.mode == 1) {
            GlobalsSide* globals = (GlobalsSide*)RVA(GAME_SIDE0 + 0x88);
            globals->score = (int32_t)(thPracParam.score / 10);
            globals->life_stocks = thPracParam.life;
            globals->life_fragments = thPracParam.life_fragment;
            globals->bomb_stocks = thPracParam.bomb;
            globals->bomb_fragments = thPracParam.bomb_fragment;
            globals->current_power = thPracParam.power;
            globals->piv = thPracParam.value;
    
            THSectionPatch();
        }
        thPracParam._playLock = true;
    })
    EHOOK_DY(th20_patch_stones, 0x1336F1, 1, {
        if (thPracParam.mode != 1)
            return;
    
        uintptr_t game_side_main = RVA(GAME_SIDE0);
        GlobalsSide* globals = (GlobalsSide*)(game_side_main + 0x88);
        int32_t hyperMax = globals->hyper_meter_max;

        if ((int32_t)thPracParam.hyper == 1 || thPracParam.hyperActive) {
            uintptr_t player_stone_manager = GetMemContent<uintptr_t>(game_side_main + 0x2c);
            bool hyperGLockEnabled = *(THOverlay::singleton().mHyperGLock);

            // call the hyper start method
            if (hyperGLockEnabled)
                THOverlay::singleton().mHyperGLock.Toggle(false); // if you know a less stupid way to prevent
            asm_call_rel<0x133780, Thiscall>(player_stone_manager, hyperMax);
            if (hyperGLockEnabled)
                THOverlay::singleton().mHyperGLock.Toggle(true); // gauge lock from interfering plz do lol

            if (thPracParam.hyperActive) { // set hyper drain timer correctly
                int32_t hyper_base_duration = globals->hyper_duration * 12;
                int32_t hyper_duration = hyper_base_duration + (hyper_base_duration * globals->g2_passive_factor) / 100;

               Timer20* hyper_timer = GetMemAddr<Timer20*>(player_stone_manager + 0x28, 0x14);
                asm_call_rel<SET_TIMER_FUNC, Thiscall>(hyper_timer, (int32_t)(thPracParam.hyper * hyper_duration));
            }
        }
        globals->hyper_meter = (int32_t)(thPracParam.hyper * hyperMax); // set hyper value

       if (thPracParam.stoneMax)
            globals->summon_meter_max = thPracParam.stoneMax; // check is for backwards compatibility
        globals->summon_meter = thPracParam.stoneActive ? thPracParam.stoneMax
                              : (int32_t)(thPracParam.stone * thPracParam.stoneMax);
        th20_cleanup_stone_active.Toggle(thPracParam.stoneActive || thPracParam.stone == 1.0f);

        globals->stone_priority_red = thPracParam.priorityR;
        globals->stone_priority_blue = thPracParam.priorityB;
        globals->stone_priority_yellow = thPracParam.priorityY;
        globals->stone_priority_green = thPracParam.priorityG;
        globals->stone_level_red = thPracParam.levelR;
        globals->stone_level_blue = thPracParam.levelB;
        globals->stone_level_yellow = thPracParam.levelY;
        globals->stone_level_green = thPracParam.levelG;
        globals->stone_enemy_count_total = thPracParam.cycle;
    })
    EHOOK_DY(th20_patch_ex_stones_fix, 0x6415A, 3, {
        if (thPracParam.mode == 1) {
            pCtx->Eip = RVA(0x6417F);
        }
    })
    EHOOK_DY(th20_param_reset, 0x129EA6, 3, {
        thPracParam.Reset();
    })
    EHOOK_DY(th20_prac_menu_1, 0x1294A3, 3, {
        THGuiPrac::singleton().State(1);
    })
    EHOOK_DY(th20_prac_menu_2, 0x1294D0, 3, {
        THGuiPrac::singleton().State(2);
    })
    EHOOK_DY(th20_prac_menu_3, 0x1299AE, 2, {
        THGuiPrac::singleton().State(3);
    })
    EHOOK_DY(th20_prac_menu_4, 0x129AD8, 3, {
        THGuiPrac::singleton().State(4);
    })
    PATCH_DY(th20_prac_menu_enter_1, 0x129595, "eb")
    EHOOK_DY(th20_prac_menu_enter_2, 0x129A98, 3, {
        pCtx->Eax = thPracParam.stage;
    })
    PATCH_DY(th20_disable_prac_menu_1, 0x129B40, "c3")
    PATCH_DY(th20_prac_menu_ignore_locked, 0x12CA30, "b001c3")
    EHOOK_DY(th20_extra_prac_fix, 0x11EB3D, 2, {
        *(uint32_t*)RVA(GAME_SIDE0 + 0x88 + 0x1E0) = GetMemContent(RVA(DIFFICULTY));
    })
    PATCH_DY(th20_instant_esc_r, 0xE2EB5, "EB")
    EHOOK_DY(th20_transition_pre_loading, 0xda801, 5, {
        deterministicTransitionR2TimerVal = GetMemContent(RVA(PLAYER_PTR), 0x22B4 + 0x12580 + 0x4);
    })

    EHOOK_DY(th20_transition_desync_fixes, 0xBA99F, 6, {
        const uintptr_t player_ptr = GetMemContent(RVA(PLAYER_PTR));
        const uint32_t stage = GetMemContent(RVA(STAGE_NUM)) - 1;
        const uintptr_t dmgSrcManager = GetMemContent(RVA(DMG_SRC_MGR_PTR));
        const bool isTransition = GetMemContent(RVA(TRANSITION_STG_PTR));
        stage_expired_summon_cnt = 0;

        // only do playback on starting stage when using copy data, record the rest

        const bool applyCopyData = !isTransition && advFixTimerOffsets && repFixParamCopy.HasTransitionSyncData(stage);

        if (THGuiRep::singleton().mRepStatus && (THGuiRep::singleton().mRepParam.HasTransitionSyncData() || applyCopyData)) {
            // Playback
            if (thPracParam.reimuR2Timer[stage]) { // ReimuR2 desync fix
                const int32_t offset = (stage != 0 && !isTransition) ? 30 : 0;
                asm_call_rel<SET_TIMER_FUNC, Thiscall>(player_ptr + 0x22B4 + 0x12580, thPracParam.reimuR2Timer[stage] + offset);
            }
            if (thPracParam.passiveMeterTimer[stage]) // passive summon gauge meter desync fix
                asm_call_rel<SET_TIMER_FUNC, Thiscall>(GetMemContent(RVA(ENM_STONE_MGR_PTR)) + 0x28, thPracParam.passiveMeterTimer[stage]);

            for (int i = 0; i < 4; i++) { // y2 option desync fix
                const uint32_t y2timer = thPracParam.yellow2CycleTimer[stage][i];

                if (y2timer) {
                    *(float*)(player_ptr + 0x684 + 0x12c * i + 0xd4) = *(float*)(&thPracParam.yellow2CycleAngle[stage][i]);
                    asm_call_rel<SET_TIMER_FUNC, Thiscall>(player_ptr + 0x684 + 0x12c * i + 0xe4, y2timer);
                }
            }
            // y1 lingering hitbox desync fix
            // if there are active sources, delete them (they won't sync due to inconsistent stage loading time)
            bool isTransition1 = false;

             while (auto activeSrc = GetMemContent<PlayerDamageSource*>(dmgSrcManager + 0xc414 + 0x4))
                asm_call_rel<DELETE_DMG_SRC_FUNC, Fastcall>(activeSrc);

            const auto& stageSrcs = thPracParam.rogueDmgSrcs[stage];

            if (stageSrcs.size()) {
                for (size_t i = 0; i < stageSrcs.size(); i++) {
                    PlayerDamageSource* newSrc = asm_call_rel<ALLOCATE_DMG_SRC_FUNC, Fastcall,
                        PlayerDamageSource*>(dmgSrcManager);

                    // copy everything from stageSrcs[i] to newSrc except ZUNList/game side stuff
                    std::memcpy((char*)newSrc + 0x14, (const char*)&stageSrcs[i] + 0x14, 0xc0 - 0x14);
                    newSrc->game_side = (void*)RVA(GAME_SIDE0);
                    if (!isTransition1) // adjusted since 30f transition stage is skipped
                        asm_call_rel<ADD_TIMER_FUNC, Thiscall>(&newSrc->duration, -30);
                }

                *(uint32_t*)(dmgSrcManager + 0xc410) = thPracParam.nextDmgID[stage];
            }
        } else {
            // Recording
            Timer20* const reimuR2Timer = GetMemAddr<Timer20*>(player_ptr + 0x22B4 + 0x12580);
            const bool enable_offset = advFixTimerOffsets && stage && isTransition && deterministicTransitionR2TimerVal && reimuR2Timer->cur;
            const int32_t fixed_offset_target = *advFixedTimerOffsets[stage - 1] + minTimerOffsets[stage - 1];
            const int32_t timer_offset = deterministicTransitionR2TimerVal + fixed_offset_target - reimuR2Timer->cur;

            // ReimuR2 desync fix
            if (enable_offset && timer_offset)
                asm_call_rel<ADD_TIMER_FUNC, Thiscall>(reimuR2Timer, timer_offset);
            thPracParam.reimuR2Timer[stage] = reimuR2Timer->cur;

               // passive summon gauge meter desync fix
            thPracParam.passiveMeterTimer[stage] = GetMemContent<int32_t>(RVA(ENM_STONE_MGR_PTR), 0x28 + 4);

            // y2 option desync fix
            for (int i = 0; i < 4; i++) {
                thPracParam.yellow2CycleAngle[stage][i] = GetMemContent(player_ptr + 0x684 + 0x12c * i + 0xd4); // save float angle as int to not lose precision
                thPracParam.yellow2CycleTimer[stage][i] = GetMemContent(player_ptr + 0x684 + 0x12c * i + 0xe4 + 0x4);
            }

            // y1 lingering hitbox desync fix
            thPracParam.rogueDmgSrcs[stage].clear();
            thPracParam.nextDmgID[stage] = GetMemContent(dmgSrcManager + 0xc410);


            PlayerDamageSource* node = GetMemAddr<PlayerDamageSource*>(dmgSrcManager + 0xc414);

            while (node = node->list_next) { // (erroneously) active damage source
                if (enable_offset && timer_offset)
                    asm_call_rel<ADD_TIMER_FUNC, Thiscall>(&node->duration, -timer_offset);
                thPracParam.rogueDmgSrcs[stage].push_back(*node);
            }
        }
    })
    PATCH_DY(th20_fix_stone_timeout_use_after_freeA, 0x1136AF, "8B4D0C" NOP(3))
    PATCH_DY(th20_fix_stone_timeout_use_after_freeB, 0x111F7F, "8B0A83E103")
    EHOOK_DY(th20_expired_summon_kill_desync_fix, 0x111f84, 3, {
        if (!THGuiRep::singleton().mRepStatus)
            return; // liveplay is fixed (cf above)

        if (!GetMemContent(RVA(ENM_STONE_MGR_PTR), 0x4)) { // summon object was deallocated
            const uint32_t stage = GetMemContent(RVA(STAGE_NUM)) - 1;

            if (stage_expired_summon_cnt < thPracParam.expStoneColors[stage].size())
                pCtx->Ecx = thPracParam.expStoneColors[stage][stage_expired_summon_cnt];
            else
                advExpStoneColors[stage].push_back(pCtx->Ecx);

            stage_expired_summon_cnt++;
        }
    })
    EHOOK_DY(th20_esc_q, 0xe2f45, 7, {
        if (Gui::KeyboardInputGetRaw('Q'))
            pCtx->Eip = RVA(0xe2fe7);
    })
    PATCH_DY(th20_fix_rep_save_stone_names, 0x127B9F, "8B82D8000000" NOP(22))
    EHOOK_DY(th20_fix_rep_stone_init, 0xBB0A0, 5, {
        if (GetMemContent(RVA(GAME_THREAD_PTR), 0x108)) {
            // Yes, the order really is swapped like this
            GlobalsSide* globals = (GlobalsSide*)RVA(GAME_SIDE0 + 0x88);
            globals->story_stone_raw = replayStones[0];
            globals->narrow_shot_stone_raw = replayStones[2];
            globals->wide_shot_stone_raw = replayStones[1];
            globals->assist_stone_raw = replayStones[3];
        }
    })
    EHOOK_DY(th20_fix_ex_rep_resolution, 0x2c719, 1, { // hooks post setting ANM sprite info from sprite_id -1 (-> ASCII sprite 288)
        if (GetMemContent(RVA(STAGE_NUM)) == 7) {
            AnmVM* vm = GetMemContent<AnmVM*>((uintptr_t)pCtx->Ebp - 0xc4);

            if (vm->sprite_id == 288 && vm->script_id == 33) { // reptilian circle (set-up once during stage load)
                if (!THGuiRep::singleton().mRepStatus) // Recording
                    thPracParam.resolutionSpriteHeight = vm->sprite_dims.y;

                else if (thPracParam.resolutionSpriteHeight) // Playback
                    vm->sprite_dims.y = thPracParam.resolutionSpriteHeight;
            }
        }
    })
    PATCH_DY(th20_fix_rep_results_skip, 0x110D61, "7B4BFAFF")
    EHOOK_DY(th20_rep_save, 0x109D6A, 3, {
        if (sReplayPath) {
            THSaveReplay(sReplayPath);
            free(sReplayPath);
            sReplayPath = nullptr;
        }
    })
    EHOOK_DY(th20_rep_get_path, 0x1098E1, 5, {
        sReplayPath = _strdup((char*)pCtx->Edx);
    })
    EHOOK_DY(th20_rep_menu_1, 0x123614, 3, {
        THGuiRep::singleton().State(1);
    })
    EHOOK_DY(th20_rep_menu_2, 0x123955, 1, {
        THGuiRep::singleton().State(2);
    })
    EHOOK_DY(th20_rep_menu_3, 0x123DAE, 2, {
        THGuiRep::singleton().State(3);
    })
    EHOOK_DY(th20_update, 0x12A72, 1, {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());
    
        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
    
        TH20InGameInfo::singleton().Update();
        auto p = ImGui::GetOverlayDrawList();
        // in case boss movedown do not disabled when playing normal games
        {
            if (THAdvOptWnd::singleton().forceBossMoveDown) {
                auto sz = ImGui::CalcTextSize(S(TH_BOSS_FORCE_MOVE_DOWN));
                p->AddRectFilled({ 240.0f, 0.0f }, { sz.x + 240.0f, sz.y }, 0xFFCCCCCC);
                p->AddText({ 240.0f, 0.0f }, 0xFFFF0000, S(TH_BOSS_FORCE_MOVE_DOWN));
            }
        }
    
        if (g_adv_igi_options.show_keyboard_monitor && *(DWORD*)(RVA(0x1ba56c)))
            KeysHUD(20, { 1280.0f, 0.0f }, { 840.0f, 0.0f }, g_adv_igi_options.keyboard_style);
        SSS::SSS_Update(20);
        // if (g_show_att_hitbox)
        //     RenderHits();
        GameUpdateOuter(p, 20);
    
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();
        GameGuiEnd(drawCursor);
    })
    EHOOK_DY(th20_player_state, 0xf7430, 1, {
        if (g_adv_igi_options.show_keyboard_monitor)
            RecordKey(20, *(DWORD*)(RVA(0x1B8B4C)));
    })
    EHOOK_DY(th20_render, 0x12CEA, 1, {
        GameGuiRender(IMPL_WIN32_DX9);
    })
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInGameInfo)
    EHOOK_DY(th20_game_start, 0xbded2, 6, // gamestart-life set
        {
            TH20InGameInfo::singleton().mMissCount = 0;
            TH20InGameInfo::singleton().mBombCount = 0;
            TH20InGameInfo::singleton().mHyperBreakCount = 0;
            TH20InGameInfo::singleton().mHyperCount = 0;
            TH20InGameInfo::singleton().mPyramidShotDownCount = 0;
            TH20Save::singleton().pyraState.Reset();
        })
    EHOOK_DY(th20_bomb_dec, 0xe1710, 1, // bomb dec
        {
            TH20InGameInfo::singleton().mBombCount++;
            TH20Save::singleton().pyraState.isPyraFailed = true;
        })
    EHOOK_DY(th20_hyper_break, 0x132c10, 3, 
        {
            TH20InGameInfo::singleton().mHyperBreakCount++;
            TH20Save::singleton().pyraState.isPyraFailed = true;
        })
    EHOOK_DY(th20_hyper, 0x134d06, 3,
        {
            TH20InGameInfo::singleton().mHyperCount++;
        })
    EHOOK_DY(th20_stone, 0x112077, 2,
        {
            TH20InGameInfo::singleton().mPyramidShotDownCount++;
        })
    
    EHOOK_DY(th20_ins_534, 0x95780, 6,
        {
            int32_t stage = GetMemContent(RVA(STAGE_NUM));
            if ((stage == 4 || stage == 5) &&(!THGuiRep::singleton().mRepStatus))
            {
                DWORD p_enmA = *(DWORD*)(pCtx->Ebp - 0x3F4);

                int32_t p_ins = *(int32_t*)(pCtx->Ebp - 0x3F8);
                std::pair<int32_t*, bool> ecl_addrs_s = GetMemAddr_s<int32_t*>(RVA(0x1BA570), 0x104, 0xc);
                if (!ecl_addrs_s.second)
                    return;
                int32_t* ecl_addrs = ecl_addrs_s.first;

                uintptr_t player_stats = RVA(0x1BA5F0);
                int32_t cur_player_type = (*(int32_t*)(player_stats + 0x8));
                int32_t main_stone = (*(int32_t*)(player_stats + 0x1C));
                int32_t diff = *((int32_t*)(RVA(0x1BA7D0)));

                int32_t spellid = -1;
                bool is_pyra = false;
                if (stage == 4 && p_ins - ecl_addrs[3] == 0x5C0)
                {
                    spellid = TH20Save::GetPyraSpellId(stage, true, main_stone);
                    is_pyra = true;
                } else if (stage == 4 && p_ins - ecl_addrs[3] == 0x1014) {
                    spellid = TH20Save::GetPyraSpellId(stage, false, main_stone);
                    is_pyra = true;
                } else if (stage == 5 && p_ins - ecl_addrs[3] == 0x5E0) {
                    spellid = TH20Save::GetPyraSpellId(stage, false, main_stone);
                    is_pyra = true;
                }
                if (is_pyra) {
                    TH20Save::singleton().pyraState.lastPyraAddr = p_enmA;
                    TH20Save::singleton().pyraState.isInPyra = true;
                    TH20Save::singleton().pyraState.lastPyraSpellId = spellid;
                    TH20Save::singleton().pyraState.isPyraFailed = false;
                    TH20Save::singleton().AddAttempt(spellid, diff, TH20Save::GetPlayerType(cur_player_type, main_stone), thPracParam.mode);
                }
            }
            
        })
    EHOOK_DY(th20_hit_bullet, 0xF8805, 2,//set state=4
        {
            TH20Save::singleton().pyraState.isPyraFailed = true;
        })
    EHOOK_DY(th20_KilledCallback, 0xA6866, 3,
        {
            int32_t stage = GetMemContent(RVA(STAGE_NUM));
            if ((stage == 4 || stage == 5) 
                && TH20Save::singleton().pyraState.isInPyra
                && pCtx->Eax == TH20Save::singleton().pyraState.lastPyraAddr
                && TH20Save::singleton().pyraState.lastPyraSpellId == 0 // st4 mid 1 pyra
                && (!THGuiRep::singleton().mRepStatus)
                ) {
                if (TH20Save::singleton().pyraState.isPyraFailed == false) {
                    uintptr_t player_stats = RVA(0x1BA5F0);
                    int32_t cur_player_type = (*(int32_t*)(player_stats + 0x8));
                    int32_t main_stone = (*(int32_t*)(player_stats + 0x1C));
                    int32_t diff = *((int32_t*)(RVA(0x1BA7D0)));
                    TH20Save::singleton().AddCapture(TH20Save::singleton().pyraState.lastPyraSpellId, diff, TH20Save::GetPlayerType(cur_player_type, main_stone),thPracParam.mode);
                    }
                TH20Save::singleton().pyraState.Reset();
                }
        })
    EHOOK_DY(th20_TimeoutCallback, 0xA624E, 3,
        {
            int32_t stage = GetMemContent(RVA(STAGE_NUM));
            if ((stage == 4 || stage == 5)
                && TH20Save::singleton().pyraState.isInPyra
                && pCtx->Eax == TH20Save::singleton().pyraState.lastPyraAddr
                && (!THGuiRep::singleton().mRepStatus)
                ) {
                if (TH20Save::singleton().pyraState.lastPyraSpellId == 0)
                {
                    if (TH20Save::singleton().pyraState.isPyraFailed == false) {
                        uintptr_t player_stats = RVA(0x1BA5F0);
                        int32_t cur_player_type = (*(int32_t*)(player_stats + 0x8));
                        int32_t main_stone = (*(int32_t*)(player_stats + 0x1C));
                        int32_t diff = *((int32_t*)(RVA(0x1BA7D0)));
                        TH20Save::singleton().AddTimeout(TH20Save::singleton().pyraState.lastPyraSpellId, diff, TH20Save::GetPlayerType(cur_player_type, main_stone), thPracParam.mode);
                    }
                    TH20Save::singleton().pyraState.Reset();
                } else {
                    TH20Save::singleton().pyraState.isPyraTimeout = true;
                }
            }
        })
    EHOOK_DY(th20_ins_517, 0x93F8E, 2, // shake screen effect
        {
            int32_t stage = GetMemContent(RVA(STAGE_NUM));
            if ((stage == 4 || stage == 5) && (!THGuiRep::singleton().mRepStatus)) {
                DWORD p_enmA = *(DWORD*)(pCtx->Ebp - 0x3F4);
                if (p_enmA == TH20Save::singleton().pyraState.lastPyraAddr){
                    int32_t p_ins = *(int32_t*)(pCtx->Ebp - 0x3F8);
                    std::pair<int32_t*, bool> ecl_addrs_s = GetMemAddr_s<int32_t*>(RVA(0x1BA570), 0x104, 0xc);
                    if (!ecl_addrs_s.second)
                        return;
                    int32_t* ecl_addrs = ecl_addrs_s.first;
                    if ((stage == 4 && p_ins - ecl_addrs[3] == 0x2cb8) || (stage == 5 && p_ins - ecl_addrs[3] == 0x32d0)) {
                        if (TH20Save::singleton().pyraState.isPyraFailed == false)
                        {
                            uintptr_t player_stats = RVA(0x1BA5F0);
                            int32_t cur_player_type = (*(int32_t*)(player_stats + 0x8));
                            int32_t main_stone = (*(int32_t*)(player_stats + 0x1C));
                            int32_t diff = *((int32_t*)(RVA(0x1BA7D0)));
                            if (TH20Save::singleton().pyraState.isPyraTimeout)
                                TH20Save::singleton().AddTimeout(TH20Save::singleton().pyraState.lastPyraSpellId, diff, TH20Save::GetPlayerType(cur_player_type, main_stone), thPracParam.mode);
                            else
                                TH20Save::singleton().AddCapture(TH20Save::singleton().pyraState.lastPyraSpellId, diff, TH20Save::GetPlayerType(cur_player_type, main_stone), thPracParam.mode);
                        }
                        TH20Save::singleton().pyraState.Reset();
                    }
                }
            }
        })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext())
            return;

        // Init
        GameGuiInit(IMPL_WIN32_DX9, RVA(0x1C4D48), RVA(WINDOW_PTR),
            Gui::INGAGME_INPUT_GEN2, RVA(0x1B88C0), RVA(0x1B88B8), 0,
            -2, *(float*)RVA(0x1B8818), 0.0f);

        // Init fonts
        if (*(DWORD*)RVA(0x1B66F8) == 0)  {
            asm_call_rel<0x16D20, Stdcall>();
        }

        SetDpadHook(0x22651, 6);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();

        // Hooks
        EnableAllHooks(THMainHook);
        EnableAllHooks(THInGameInfo);

        TH20InGameInfo::singleton();
        TH20Save::singleton();
        TH20Save::singleton().LoadSave();

        
        // Replay menu string fixes
        auto stageStrings = (const char**)RVA(0x1AFFD0);
        stageStrings[4] = "St4";
        stageStrings[5] = "St5";
        stageStrings[6] = "St6";
        stageStrings[7] = "Ex ";
        stageStrings[8] = "All";
        *(const char**)RVA(0x1B0A7C) = "%s  %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %s %2.1f%%";

        // AnmVM reset uninitialized memory fix
        for (size_t i = 0; i < elementsof(anmUninitFixHooks); i++) {
            *(uintptr_t*)((uintptr_t)anmUninitFixHooks[i].data.buffer.ptr + 1) = (uintptr_t)&AnmVM_Reset_hook - RVA(anmUninitFixHooks[i].addr) - 5;
            anmUninitFixHooks[i].Setup();
            anmUninitFixHooks[i].Enable();
        }


        //  Reset thPracParam
        thPracParam.Reset();
    }

    PATCH_ST(TH20FontCreateHook, 0x1e937, "9090909090"); // disable font create before GUI create

    // int(__stdcall* g_realMultiByteToWideChar)(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
    // int __stdcall MultiByteToWideChar_Changed(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
    // {
    //     return g_realMultiByteToWideChar(CP_ACP, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
    // }
    // 
    // int(__stdcall* g_reaWideCharToMultiByte)(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar);
    // int __stdcall WideCharToMultiByte_Changed(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar)
    // {
    //     return g_reaWideCharToMultiByte(CP_ACP, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
    // }


    HOOKSET_DEFINE(THInitHook)
    PATCH_DY(th20_startup_1, 0x11F82C, "EB")
    PATCH_DY(th20_startup_2, 0x11E8C9, "EB")
    EHOOK_DY(th20_gui_init_1, 0x12A0A3, 7, {
        self->Disable();
        THGuiCreate();
        TH20FontCreateHook.Disable();
    })
    EHOOK_DY(th20_gui_init_2, 0x1C725, 1, {
        self->Disable();
        THGuiCreate();
        TH20FontCreateHook.Disable();
    })
    HOOKSET_ENDDEF()
}

void TH20Init()
{
    ingame_image_base = CurrentImageBase;
    EnableAllHooks(TH20::THInitHook);
    InitHook(20);
    TH20::TH20FontCreateHook.Setup();
    TH20::TH20FontCreateHook.Enable();
}
}