#include "thprac_games.h"
#include "thprac_utils.h"
#include "../3rdParties/d3d8/include/d3d8.h"
#include <fstream>

#include <format>
#include "../3rdParties/d3d8/include/dsound.h"


#include "thprac_res.h"
namespace THPrac {
extern bool g_pauseBGM_06;
extern bool g_forceRenderCursor;

namespace TH06 {
    ImTextureID g_hitbox_textureID = NULL;
    ImVec2 g_hitbox_sz = { 32.0f, 32.0f };

    ImVec2 g_books_pos[6] = { { 0.0f, 0.0f } };

    int32_t g_last_rep_seed = 0;
    bool g_show_bullet_hitbox = false;
    float g_last_boss_x, g_last_boss_y;

    static bool is_died = false;
    int g_lock_timer = 0;

    bool THBGMTest();
    using std::pair;
    struct THPracParam {
        int32_t mode;

        int32_t stage;
        int32_t section;
        int32_t phase;
        int32_t frame;

        int64_t score;
        float life;
        float bomb;
        float power;
        int32_t graze;
        int32_t point;

        int32_t rank;
        bool rankLock;
        int32_t fakeType;

        int32_t delay_st6bs9;
        bool wall_prac_st6;
        bool dlg;

        int bF;//book_fix
        int bX1;//book_x
        int bX2;
        int bX3;
        int bX4;
        int bX5;
        int bX6;

        int bY1;//book_y
        int bY2;
        int bY3;
        int bY4;
        int bY5;
        int bY6;

        int snipeN;
        int snipeF;

        bool _playLock;
        void Reset()
        {
            mode = 0;
            stage = 0;
            section = 0;
            phase = 0;
            score = 0ll;
            life = 0.0f;
            bomb = 0.0f;
            power = 0.0f;
            graze = 0;
            point = 0;
            rank = 0;
            rankLock = false;
            fakeType = 0;

            bY1 = 32.0f,
            bY2 = 128.0f,
            bY3 = 144.0f,
            bY4 = 64.0f,
            bY5 = 80.0f,
            bY6 = 96.0f;
            bX1 = 0.0f;
            bX2 = 0.0f;
            bX3 = 0.0f;
            bX4 = 0.0f;
            bX5 = 0.0f;
            bX6 = 0.0f;
            bF = 0;
            snipeN = 0;
            snipeF = 0;
            
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th06");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValueEx(dlg, Bool);
            GetJsonValue(frame);
            GetJsonValue(score);
            GetJsonValue(life);
            GetJsonValue(bomb);
            GetJsonValue(power);
            GetJsonValue(graze);
            GetJsonValue(point);
            GetJsonValue(rank);
            GetJsonValueEx(rankLock, Bool);
            GetJsonValue(fakeType);
            GetJsonValue(delay_st6bs9);
            GetJsonValueEx(wall_prac_st6, Bool);

            GetJsonValue(bF);
            GetJsonValue(bX1);
            GetJsonValue(bX2);
            GetJsonValue(bX3);
            GetJsonValue(bX4);
            GetJsonValue(bX5);
            GetJsonValue(bX6);
            GetJsonValue(bY1);
            GetJsonValue(bY2);
            GetJsonValue(bY3);
            GetJsonValue(bY4);
            GetJsonValue(bY5);
            GetJsonValue(bY6);

            GetJsonValue(snipeN);
            GetJsonValue(snipeF);

            return true;
        }
        std::string GetJson()
        {
            CreateJson();

            AddJsonValueEx(version, GetVersionStr(), jalloc);
            AddJsonValueEx(game, "th06", jalloc);
            AddJsonValue(mode);
            AddJsonValue(stage);
            if (section)
                AddJsonValue(section);
            if (phase)
                AddJsonValue(phase);
            if (frame)
                AddJsonValue(frame);
            if (dlg)
                AddJsonValue(dlg);

            AddJsonValue(score);
            AddJsonValueEx(life, (int)life);
            AddJsonValueEx(bomb, (int)bomb);
            AddJsonValueEx(power, (int)power);
            AddJsonValue(graze);
            AddJsonValue(point);
            AddJsonValue(rank);
            AddJsonValue(rankLock);
            AddJsonValue(fakeType);
            AddJsonValue(delay_st6bs9);
            AddJsonValue(wall_prac_st6);
            
            AddJsonValue(bF);
            AddJsonValue(bX1);
            AddJsonValue(bX2);
            AddJsonValue(bX3);
            AddJsonValue(bX4);
            AddJsonValue(bX5);
            AddJsonValue(bX6);
            AddJsonValue(bY1);
            AddJsonValue(bY2);
            AddJsonValue(bY3);
            AddJsonValue(bY4);
            AddJsonValue(bY5);
            AddJsonValue(bY6);

            AddJsonValue(snipeN);
            AddJsonValue(snipeF);

            ReturnJson();
        }
    };
    bool thRestartFlag = false;
    bool threstartflag_normalgame = false;
    

    THPracParam thPracParam {};

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
        SINGLETON(THOverlay);
    public:

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

            mMenu.SetTextOffsetRel(x_offset_1, x_offset_2);
            mMuteki.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfLives.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfBombs.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfPower.SetTextOffsetRel(x_offset_1, x_offset_2);
            mTimeLock.SetTextOffsetRel(x_offset_1, x_offset_2);
            mAutoBomb.SetTextOffsetRel(x_offset_1, x_offset_2);
            mElBgm.SetTextOffsetRel(x_offset_1, x_offset_2);
            mShowSpellCapture.SetTextOffsetRel(x_offset_1, x_offset_2);
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mInfLives();
            mInfBombs();
            mInfPower();
            mTimeLock();
            mAutoBomb();
            mElBgm();
            mShowSpellCapture();
        }
        virtual void OnPreUpdate() override
        {
            if (mMenu(false) && (!ImGui::IsAnyItemActive() || *mShowSpellCapture)) {
                if (*mMenu) {
                    Open();
                } else {
                    Close();
                    *((int32_t*)0x6c6eb0) = 3;
                }
            }
        }

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };
        
        HOTKEY_DEFINE(mMuteki, TH_MUTEKI, "F1", VK_F1)
        PATCH_HK(0x4277c2, "03"),
        PATCH_HK(0x42779a, "83c4109090")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfBombs, TH_INFBOMBS, "F3", VK_F3)
        PATCH_HK(0x4289e3, "00")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfPower, TH_INFPOWER, "F4", VK_F4)
        PATCH_HK(0x428B7D, "00"),
        PATCH_HK(0x428B67, "909090909090909090")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mAutoBomb, TH_AUTOBOMB, "F6", VK_F6)
        PATCH_HK(0x428989, "EB1D"),
        PATCH_HK(0x4289B4, "85D2"),
        PATCH_HK(0x428A94, "FF89"),
        PATCH_HK(0x428A9D, "66C70504D9690002")
        HOTKEY_ENDDEF();
        

    public:
        HOTKEY_DEFINE(mInfLives, TH_INFLIVES2, "F2", VK_F2)
        PATCH_HK(0x428DDB, "a0bad469009090909090909090909090"),
        PATCH_HK(0x428AC6, "909090909090")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F5", VK_F5)
        PATCH_HK(0x412DD1, "eb")
        HOTKEY_ENDDEF();

        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
        Gui::GuiHotKey mShowSpellCapture { THPRAC_INGAMEINFO, "F8", VK_F8 };
    };

    class TH06InGameInfo : public Gui::GameGuiWnd {
        TH06InGameInfo() noexcept
        {
            SetTitle("igi");
            SetFade(0.9f, 0.9f);
            SetSizeRel(180.0f/640.0f, 0.0f);
            SetPosRel(433.0f / 640.0f, 245.0f / 480.0f);
            SetWndFlag(ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | 
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
            OnLocaleChange();
        }
        SINGLETON(TH06InGameInfo);

    public:
        int32_t mMissCount;
    protected:
        Gui::GuiHotKey mDetails { THPRAC_INGAMEINFO_06_SHOWDETAIL_HOTKEY, "F9", VK_F9 };

        struct
        {
            int Power = 0;
            int Bombs = 0;
            int EndFrame = 0;
            byte SCB = 0;
        } doubleKOFix;


        struct TH06Save
        {
            int32_t SC_History[65][5][4][3] = { 0 }; // spellid, diff, playertype, [capture/attempt/timeout]
            int64_t timePlayer[5][4] = { 0 }; // diff/type, precision:ns
        };
        TH06Save save_current;
        TH06Save save_total;
        bool mIsSaveLoaded = false;

        bool last_is_in_spell = false;
        byte last_spell_id = 0;
        byte last_diff = 0;
        byte last_ingame_flag = 16;
        byte last_player_typea = 0;
        byte last_player_typeb = 0;
        int last_cur_face_time = 0;
        int last_stage = 0;
        int last_pl_power = 0;
        int last_pl_bomb = 0;
        short last_cur_hp = 0;
        short last_tot_hp = 0;
        byte last_has_SCB = 0;

        bool power_decreased = false;
        bool bomb_decreased = false;
        bool fin_flag = false;
        bool is_magic_book = false;

        bool detail_open = false;

        int last_spell_id_hist = -1;
        int last_diff_hist = -1;

        int time_books = 0;

        void LoadSave()
        {
            mIsSaveLoaded = true;
            PushCurrentDirectory(L"%appdata%\\ShanghaiAlice\\th06");
            auto fs_new = ::std::fstream("score06.dat", ::std::ios::in | ::std::ios::binary);
            if (fs_new.is_open()){
                int version = 1;
                fs_new.read((char*)&version,sizeof(version));
                switch (version)
                {
                default:
                case 1:
                {
                    fs_new.read((char*)save_total.SC_History, sizeof(save_total.SC_History));
                    fs_new.read((char*)save_total.timePlayer, sizeof(save_total.timePlayer));
                }
                    break;
                }
                fs_new.close();
            } else {
                //compatible
                auto fs = ::std::fstream("spell_capture.dat", ::std::ios::in | ::std::ios::binary);
                if (fs.is_open()) {
                      fs.read((char*)save_total.SC_History, sizeof(save_total.SC_History));
                      if (!fs.eof())
                          fs.read((char*)save_total.timePlayer, sizeof(save_total.timePlayer)); // compatible , avoid eof

                      for(int i=0;i<5;i++)
                          for(int j=0;j<4;j++)
                              save_total.timePlayer[i][j] *= 1000000;// make precision to ns
                }
                fs.close();
            }
            PopCurrentDirectory();
        }

        void SaveSave()
        {
            int version = 1;
            PushCurrentDirectory(L"%appdata%\\ShanghaiAlice\\th06");
            auto fs = ::std::fstream("score06.dat", ::std::ios::out | ::std::ios::binary);
            if (fs.is_open()) {
                fs.write((char*)(&version), sizeof(version));
                fs.write((char*)(&save_total), sizeof(save_total));
                fs.close();
            }
            PopCurrentDirectory();
        }

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
            mDetails.SetTextOffsetRel(x_offset_1, x_offset_2);
        }

        virtual void OnContentUpdate() override
        {
            int spell_id = last_spell_id;
            if (is_magic_book)
                spell_id = 64;
            int32_t mBombCount = *(int8_t*)(0x0069BCC4);

            byte cur_player_typea = *(byte*)(0x69D4BD);
            byte cur_player_typeb = *(byte*)(0x69D4BE);
            byte cur_player_type = (cur_player_typea << 1) | cur_player_typeb;
            auto diff_pl = std::format("{}({})", S(IGI_DIFF[*(int8_t*)(0x69bcb0)]), S(IGI_PL_06[cur_player_type]));
            auto diff_pl_sz = ImGui::CalcTextSize(diff_pl.c_str());

            ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5 - diff_pl_sz.x*0.5);
            ImGui::Text(diff_pl.c_str());

            ImGui::Columns(2);
            ImGui::Text(S(THPRAC_INGAMEINFO_MISS_COUNT));
            ImGui::NextColumn();
            ImGui::Text("%8d", mMissCount);
            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_BOMB_COUNT));
            ImGui::NextColumn();
            ImGui::Text("%8d", mBombCount);

            if (g_adv_igi_options.th06_showRank) {
                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_INGAMEINFO_TH06_RANK));
                ImGui::NextColumn();
                ImGui::Text("%8d.%02d", *(int32_t*)(0x69d710), *(int32_t*)(0x69d71C));
            }

            ImGui::Columns(1);

            if (spell_id != -1 && last_ingame_flag > 0 && (last_is_in_spell || is_magic_book)) {
                byte last_player_type = (last_player_typea << 1) | last_player_typeb;
                ImGui::Text("%s", th06_spells_str[Gui::LocaleGet()][spell_id]);
                int tot_sc_caped = save_total.SC_History[spell_id][last_diff][last_player_type][0];
                int tot_sc_tot = save_total.SC_History[spell_id][last_diff][last_player_type][1];
                int tot_sc_to = save_total.SC_History[spell_id][last_diff][last_player_type][2];
                ImGui::Text("%d/%d(%.1f%%); %d", tot_sc_caped,tot_sc_tot, (float)(tot_sc_caped) / std::fmax(1.0f, tot_sc_tot) * 100.0f,  tot_sc_to);

                int cur_sc_caped = save_current.SC_History[spell_id][last_diff][last_player_type][0];
                int cur_sc_tot = save_current.SC_History[spell_id][last_diff][last_player_type][1];
                int cur_sc_to = save_current.SC_History[spell_id][last_diff][last_player_type][2];
                ImGui::Text("%d/%d(%.1f%%); %d", cur_sc_caped, cur_sc_tot, (float)(cur_sc_caped) / std::fmax(1.0f, cur_sc_tot) * 100.0f, cur_sc_to);

                if (is_magic_book && thPracParam.mode && (thPracParam.phase != 0)) { // books
                    ImGui::Text("%.1f", (float)time_books / 60.0f);
                }
            }
            mDetails();
        }
        void ResetSpell()
        {
            memset(save_current.SC_History, 0, sizeof(save_current.SC_History));
        }
        void AddAttempt(int spell_id, byte diff, byte player_type)
        {
            is_died = false;
            save_total.SC_History[spell_id][diff][player_type][1]++;
            save_current.SC_History[spell_id][diff][player_type][1]++;
            SaveSave();
        }
        void AddCapture(int spell_id, byte diff, byte player_type)
        {
            save_total.SC_History[spell_id][diff][player_type][0]++;
            save_current.SC_History[spell_id][diff][player_type][0]++;
            SaveSave();
        }
        void AddTimeOut(int spell_id, byte diff, byte player_type)
        {
            save_total.SC_History[spell_id][diff][player_type][2]++;
            save_current.SC_History[spell_id][diff][player_type][2]++;
            SaveSave();
        }

        public:
            void ShowDetail(bool *isOpen){
                ImGui::BeginTabBar("Detail Spell");
                {
                    const char* tabs_diff[5] = { S(THPRAC_IGI_DIFF_E), S(THPRAC_IGI_DIFF_N), S(THPRAC_IGI_DIFF_H), S(THPRAC_IGI_DIFF_L), S(THPRAC_IGI_DIFF_EX) };
                    for (int diff = 0; diff < 5; diff++) {
                        if (ImGui::BeginTabItem(tabs_diff[diff])) {
                            ImGui::BeginTabBar("Player Type");
                            const char* tabs_player[4] = { S(THPRAC_IGI_PL_ReimuA), S(THPRAC_IGI_PL_ReimuB), S(THPRAC_IGI_PL_MarisaA), S(THPRAC_IGI_PL_MarisaB) };
                            for (int pl = 0; pl < 4; pl++) {
                                if (ImGui::BeginTabItem(tabs_player[pl])) {
                                    // time
                                    ImGui::BeginTable(std::format("{}{}timetable", tabs_diff[diff], tabs_player[pl]).c_str(), 4, ImGuiTableFlags_::ImGuiTableFlags_Resizable);
                                    ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_GAMTIME_TOT));
                                    ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_GAMTIME_CUR));
                                    ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_GAMTIME_CHARACTER_TOT));
                                    ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_GAMTIME_ALL));
                                    ImGui::TableHeadersRow();

                                    int64_t gametime_tot = save_total.timePlayer[diff][pl];
                                    int64_t gametime_cur = save_current.timePlayer[diff][pl];
                                    int64_t gametime_chartot = 0;
                                    for (int i = 0; i < 5; i++)
                                        gametime_chartot += save_total.timePlayer[i][pl];
                                    int64_t gametime_all = 0;
                                    for (int i = 0; i < 5; i++)
                                        for (int j = 0; j < 4; j++)
                                            gametime_all += save_total.timePlayer[i][j];

                                    ImGui::TableNextRow();
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped(GetTime_HHMMSS(gametime_tot).c_str());
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped(GetTime_HHMMSS(gametime_cur).c_str());
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped(GetTime_HHMMSS(gametime_chartot).c_str());
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped(GetTime_HHMMSS(gametime_all).c_str());

                                    ImGui::TableNextRow();
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(gametime_tot).c_str());
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(gametime_cur).c_str());
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(gametime_chartot).c_str());
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(gametime_all).c_str());

                                    ImGui::TableNextRow();
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped("(%lld %s)", gametime_tot, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped("(%lld %s)", gametime_cur, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped("(%lld %s)", gametime_chartot, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
                                    ImGui::TableNextColumn();
                                    ImGui::TextWrapped("(%lld %s)", gametime_all, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));

                                    ImGui::EndTable();
                                    // spell capture
                                    ImGui::BeginTable(std::format("{}{}sptable", tabs_diff[diff], tabs_player[pl]).c_str(), 5, ImGuiTableFlags_::ImGuiTableFlags_Resizable);
                                    ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_SPELL_NAME), 0, 100.0f);
                                    ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_CAPTURE_TOT), 0, 50.0f);
                                    ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_TIMEOUT_TOT), 0, 30.0f);
                                    ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_CAPTURE_CUR), 0, 50.0f);
                                    ImGui::TableSetupColumn(S(THPRAC_INGAMEINFO_06_TIMEOUT_CUR), 0, 30.0f);
                                    ImGui::TableHeadersRow();

                                    for (int spell = 0; spell < 65; spell++) {
                                        if (is_spell_used[diff][spell]) {
                                            ImGui::TableNextRow();
                                            ImGui::TableNextColumn();
                                            ImGui::Text("%s", th06_spells_str[Gui::LocaleGet()][spell]);
                                            ImGui::TableNextColumn();

                                            ImGui::Text("%d/%d(%.1f%%)",
                                                save_total.SC_History[spell][diff][pl][0],
                                                save_total.SC_History[spell][diff][pl][1],
                                                (float)(save_total.SC_History[spell][diff][pl][0]) / std::fmax(1.0f, save_total.SC_History[spell][diff][pl][1]) * 100);
                                            ImGui::TableNextColumn();

                                            ImGui::Text("%d", save_total.SC_History[spell][diff][pl][2]);
                                            ImGui::TableNextColumn();

                                            ImGui::Text("%d/%d(%.1f%%)",
                                                save_current.SC_History[spell][diff][pl][0],
                                                save_current.SC_History[spell][diff][pl][1],
                                                (float)(save_current.SC_History[spell][diff][pl][0]) / std::fmax(1.0f, save_current.SC_History[spell][diff][pl][1]) * 100);
                                            ImGui::TableNextColumn();

                                            ImGui::Text("%d", save_current.SC_History[spell][diff][pl][2]);
                                        }
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
                if (isOpen != nullptr && ImGui::Button(S(TH_BACK))){
                    *isOpen = false;
                }
            }
        private:

        virtual void OnPreUpdate() override
        {

            bool cur_is_in_spell = *(bool*)(0x5A5F90);
            byte cur_spell_id = *(byte*)(0x5A5F98);
            byte cur_ingame_flag = *(byte*)(0x69BC8C);
            byte cur_diff = *(byte*)(0x69BCB0);
            byte cur_player_typea = *(byte*)(0x69D4BD);
            byte cur_player_typeb = *(byte*)(0x69D4BE);
            int cur_cur_face_time = *(int*)(0x69BC08);
            int cur_stage = *(int*)(0x69D6D4);
            int cur_pl_power = *(int*)(0x69D4B0);
            int cur_pl_bomb = *(int*)(0x69D4BB);
            short cur_cur_hp = *(short*)(0x4B957C);
            short cur_tot_hp = *(short*)(0x4B9580);
            byte cur_has_SCB = *(byte*)(0x5A5F8C);

            byte is_rep = *(byte*)(0x69BCBC);
            int time_sec = *(int*)(0x69BC48);

            byte cur_player_type = (cur_player_typea << 1) | cur_player_typeb;
            // is_rep = false;
            if (is_rep == 0) {
                if (cur_pl_power < last_pl_power) {
                    power_decreased = true;
                }
                if (cur_pl_bomb < last_pl_bomb) {
                    bomb_decreased = true;
                }
                if (cur_stage == 4) {
                    if (last_cur_hp < 3500 && cur_cur_hp == 3500 && last_tot_hp < 3500 && cur_tot_hp == 3500) // 第一本魔法书的血量
                    {
                        if (!(thPracParam.mode && thPracParam.stage == 3 && thPracParam.section == TH06_ST4_BOOKS && thPracParam.phase != 0)) {
                            AddAttempt(64, cur_diff, cur_player_type);
                        }
                        time_books = 0;
                        power_decreased = false;
                        bomb_decreased = false;
                        fin_flag = false;
                        is_magic_book = true;
                    }
                    if (is_magic_book && (time_sec == 40 || time_sec == 39) && cur_ingame_flag > 0) { // 见到小谔魔
                        if (!fin_flag) {
                            doubleKOFix.Power = last_pl_power;
                            doubleKOFix.Bombs = last_pl_bomb;
                            doubleKOFix.EndFrame = cur_cur_face_time;
                            fin_flag = true;
                        }
                        if (fin_flag && cur_cur_face_time - doubleKOFix.EndFrame >= 8) {
                            if (cur_pl_power < doubleKOFix.Power) {
                                power_decreased = true;
                            }
                            if (cur_pl_bomb < doubleKOFix.Bombs) {
                                bomb_decreased = true;
                            }
                            if (!power_decreased && !bomb_decreased && is_died == false) {
                                AddCapture(64, cur_diff, cur_player_type);
                            }
                            fin_flag = false;
                            is_magic_book = false;
                        }
                    }
                    if (((last_ingame_flag > 0 && cur_ingame_flag == 0) && is_magic_book)) {
                        is_magic_book = false;
                        fin_flag = false;
                    }
                }
                if ((last_is_in_spell == 1 && cur_is_in_spell == 0 && cur_ingame_flag > 0) || (last_is_in_spell == 1 && cur_is_in_spell == 2 && cur_ingame_flag > 0) || (last_is_in_spell == 1 && cur_is_in_spell == 1 && cur_ingame_flag > 0 && (last_spell_id != cur_spell_id && cur_spell_id != 0 && last_spell_id != 0))) {
                    if (!fin_flag) {
                        doubleKOFix.Power = last_pl_power;
                        doubleKOFix.Bombs = last_pl_bomb;
                        doubleKOFix.EndFrame = cur_cur_face_time;
                        fin_flag = true;
                        doubleKOFix.SCB = last_has_SCB;
                    }
                } else if ((last_is_in_spell == 1 && last_ingame_flag > 0 && cur_ingame_flag == 0) || ((last_ingame_flag > 0 && cur_ingame_flag == 0) && fin_flag)) {
                    fin_flag = false;
                }
                if (fin_flag && ((cur_cur_face_time - doubleKOFix.EndFrame >= 8) || (last_spell_id != cur_spell_id && cur_spell_id != 0 && last_spell_id != 0))) {
                    if (cur_pl_power < doubleKOFix.Power) {
                        power_decreased = true;
                    }
                    if (cur_pl_bomb < doubleKOFix.Bombs) {
                        bomb_decreased = true;
                    }
                    if (doubleKOFix.SCB == 1 && last_has_SCB == 1 && cur_ingame_flag > 0 && is_died == false) {
                        AddCapture(last_spell_id, cur_diff, cur_player_type);
                    }
                    if (last_has_SCB == 0 && cur_ingame_flag > 0 && !power_decreased && !bomb_decreased && is_died == false) {
                        AddTimeOut(last_spell_id, cur_diff, cur_player_type);
                    }
                    fin_flag = false;
                }

                if ((last_is_in_spell == 0 && cur_is_in_spell == 1) || (last_spell_id != cur_spell_id && cur_spell_id != 0 && last_spell_id != 0)) {
                    AddAttempt(cur_spell_id, cur_diff, cur_player_type);
                    power_decreased = false;
                    bomb_decreased = false;
                    fin_flag = false;
                }
            }
            last_is_in_spell = cur_is_in_spell;
            last_spell_id = cur_spell_id;
            last_ingame_flag = cur_ingame_flag;
            last_diff = cur_diff;
            last_player_typea = cur_player_typea;
            last_player_typeb = cur_player_typeb;
            last_diff = cur_diff;
            last_cur_face_time = cur_cur_face_time;
            last_stage = cur_stage;
            last_pl_power = cur_pl_power;
            last_pl_bomb = cur_pl_bomb;
            last_cur_hp = cur_cur_hp;
            last_tot_hp = cur_tot_hp;
            last_has_SCB = cur_has_SCB;

            // books time
            DWORD gameState = *(DWORD*)(0x6C6EA4);
            BYTE pauseMenuState = *(BYTE*)(0x69D4BF);
            if (is_magic_book && thPracParam.mode && (thPracParam.phase != 0)) { // books
                if (gameState == 2 && pauseMenuState == 0) {
                    time_books++;
                }
            } else {
                time_books = 0;
            }

            if (*THOverlay::singleton().mShowSpellCapture && (*(DWORD*)(0x6C6EA4) == 2)) {
                SetPosRel(433.0f / 640.0f, 245.0f / 480.0f);
                SetSizeRel(180.0f / 640.0f, 0.0f);
                // DWORD cfg = *(DWORD*)(0x006C6E60);
                // cfg |= 1 << 3;
                // *(DWORD*)(0x6C6E60) = cfg;
                Open();
            } else {
                Close();
                *((int32_t*)0x6c6eb0) = 3;
            }

            if (*THOverlay::singleton().mShowSpellCapture && *mDetails) {
                detail_open = true;
                if (ImGui::Begin(S(THPRAC_INGAMEINFO_06_SHOWDETAIL_PAGE), &detail_open,
                        ImGuiWindowFlags_NoMove)) {
                    mDetails.Toggle(detail_open);
                    *((int32_t*)0x6c6eb0) = 3;
                }
                ImGui::SetWindowPos(S(THPRAC_INGAMEINFO_06_SHOWDETAIL_PAGE), ImVec2(10.0f, 10.0f));
                ImGui::SetWindowSize(S(THPRAC_INGAMEINFO_06_SHOWDETAIL_PAGE), ImVec2(620.0f, 460.0f));
                bool is_close = false;
                ShowDetail(&detail_open);
                mDetails.Toggle(detail_open);

                if (g_forceRenderCursor || Gui::ImplWin32CheckFullScreen()) {
                    auto& io = ::ImGui::GetIO();
                    io.MouseDrawCursor = true;
                }

                ImGui::End();
            }
        }
        LARGE_INTEGER mPerformanceFreq = { 0 };
        LARGE_INTEGER mLastCount = { 0 };
        int64_t mTimePlayedns = 0;
    public:
        void Retry()
        {
            last_is_in_spell = false;
            last_spell_id = 0;
            last_diff = 0;
            last_ingame_flag = 16;
            last_player_typea = 0;
            last_player_typeb = 0;
            last_cur_face_time = 0;
            last_stage = 0;
            last_pl_power = 0;
            last_pl_bomb = 0;
            last_cur_hp = 0;
            last_tot_hp = 0;
            last_has_SCB = 0;
            power_decreased = false;
            bomb_decreased = false;
            fin_flag = false;
            is_magic_book = false;
            last_spell_id_hist = -1;
            last_diff_hist = -1;
            time_books = 0;
        }
        void SaveAll(){
            if (mIsSaveLoaded)
                SaveSave();
        }
        void Init() {
            QueryPerformanceFrequency(&mPerformanceFreq);
            mLastCount.QuadPart=0;
            mTimePlayedns = 0;
            LoadSave();
        }
        void IncreaseGameTime(){
            DWORD gameState = *(DWORD*)(0x6C6EA4);
            BYTE pauseMenuState = *(BYTE*)(0x69D4BF);
            byte is_rep = *(byte*)(0x69BCBC);
            if ((!is_rep) && gameState==2 && pauseMenuState == 0) {
                byte cur_diff = *(byte*)(0x69BCB0);
                byte cur_player_typea = *(byte*)(0x69D4BD);
                byte cur_player_typeb = *(byte*)(0x69D4BE);
                byte cur_player_type = (cur_player_typea << 1) | cur_player_typeb;

                LARGE_INTEGER curCount;
                QueryPerformanceCounter(&curCount);
                if (mLastCount.QuadPart != 0) {
                    int64_t time_ns = ((((double)(curCount.QuadPart - mLastCount.QuadPart)) / (double)mPerformanceFreq.QuadPart) * 1e9);
                    save_total.timePlayer[cur_diff][cur_player_type] += time_ns;
                    save_current.timePlayer[cur_diff][cur_player_type] += time_ns;
                    mTimePlayedns += time_ns;
                    if (mTimePlayedns >= (1000000000ll*60*3)) { // save every 3 minutes automatically
                        mTimePlayedns = 0;
                        SaveSave();
                    }
                }
                mLastCount = curCount;
            } else {
                QueryPerformanceCounter(&mLastCount);
            }
        }
    };

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mLife = 8;
            *mBomb = 8;
            *mPower = 128;
            *mMode = 1;
            *mScore = 0;
            *mGraze = 0;
            *mRank = 32;

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiPrac);
    public:

        __declspec(noinline) void State(int state)
        {
            switch (state) {
            case 0:
                break;
            case 1:
                SetFade(0.8f, 0.1f);
                Open();
                mDiffculty = (int)(*((int8_t*)0x69bcb0));
                mShotType = (int)(*((int8_t*)0x69d4bd) * 2) + *((int8_t*)0x69d4be);
                break;
            case 2:
                break;
            case 3:
                SetFade(0.8f, 0.8f);
                Close();
                *mNavFocus = 0;

                // Fill Param
                thPracParam.mode = *mMode;

                thPracParam.stage = *mStage;
                thPracParam.section = CalcSection();
                thPracParam.phase = *mPhase;
                thPracParam.frame = *mFrame;
                if (SectionHasDlg(thPracParam.section))
                    thPracParam.dlg = *mDlg;
                if (thPracParam.section == TH06_ST6_BOSS9)
                    thPracParam.delay_st6bs9 = *mDelaySt6Bs9;
                if (thPracParam.section == TH06_ST6_BOSS9 || thPracParam.section == TH06_ST6_BOSS6)
                {
                    thPracParam.wall_prac_st6 = *mWallPrac;
                    thPracParam.snipeF = *mWallPracSnipeF;
                    thPracParam.snipeN = *mWallPracSnipeN;
                }
                if (thPracParam.section == TH06_ST4_BOOKS) {
                    thPracParam.bF = 0;
                    thPracParam.bF |= ((int)*mBookC1)<<0;thPracParam.bX1 = *mBookX1;thPracParam.bY1 = *mBookY1;
                    thPracParam.bF |= ((int)*mBookC2)<<1;thPracParam.bX2 = *mBookX2;thPracParam.bY2 = *mBookY2;
                    thPracParam.bF |= ((int)*mBookC3)<<2;thPracParam.bX3 = *mBookX3;thPracParam.bY3 = *mBookY3;
                    thPracParam.bF |= ((int)*mBookC4)<<3;thPracParam.bX4 = *mBookX4;thPracParam.bY4 = *mBookY4;
                    thPracParam.bF |= ((int)*mBookC5)<<4;thPracParam.bX5 = *mBookX5;thPracParam.bY5 = *mBookY5;
                    thPracParam.bF |= ((int)*mBookC6)<<5;thPracParam.bX6 = *mBookX6;thPracParam.bY6 = *mBookY6;
                }

                thPracParam.score = *mScore;
                thPracParam.life = (float)*mLife;
                thPracParam.bomb = (float)*mBomb;
                thPracParam.power = (float)*mPower;
                thPracParam.graze = *mGraze;
                thPracParam.point = *mPoint;

                thPracParam.rank = *mRank;
                thPracParam.rankLock = *mRankLock;
                if (thPracParam.section >= TH06_ST4_BOSS1 && thPracParam.section <= TH06_ST4_BOSS7)
                    thPracParam.fakeType = *mFakeShot;
                break;
            case 4:
                Close();
                *mNavFocus = 0;
                break;
            case 5:
                // Fill Param
                thPracParam.mode = *mMode;

                thPracParam.stage = *mStage;
                thPracParam.section = CalcSection();
                thPracParam.phase = *mPhase;
                thPracParam.frame = *mFrame;
                if (SectionHasDlg(thPracParam.section))
                    thPracParam.dlg = *mDlg;
                if (thPracParam.section == TH06_ST6_BOSS9)
                    thPracParam.delay_st6bs9 = *mDelaySt6Bs9;
                if (thPracParam.section == TH06_ST6_BOSS9 || thPracParam.section == TH06_ST6_BOSS6) {
                    thPracParam.wall_prac_st6 = *mWallPrac;
                    thPracParam.snipeF = *mWallPracSnipeF;
                    thPracParam.snipeN = *mWallPracSnipeN;
                }
                if (thPracParam.section == TH06_ST4_BOOKS) {
                    thPracParam.bF = 0;
                    thPracParam.bF |= ((int)*mBookC1)<<0;thPracParam.bX1 = *mBookX1;thPracParam.bY1 = *mBookY1;
                    thPracParam.bF |= ((int)*mBookC2)<<1;thPracParam.bX2 = *mBookX2;thPracParam.bY2 = *mBookY2;
                    thPracParam.bF |= ((int)*mBookC3)<<2;thPracParam.bX3 = *mBookX3;thPracParam.bY3 = *mBookY3;
                    thPracParam.bF |= ((int)*mBookC4)<<3;thPracParam.bX4 = *mBookX4;thPracParam.bY4 = *mBookY4;
                    thPracParam.bF |= ((int)*mBookC5)<<4;thPracParam.bX5 = *mBookX5;thPracParam.bY5 = *mBookY5;
                    thPracParam.bF |= ((int)*mBookC6)<<5;thPracParam.bX6 = *mBookX6;thPracParam.bY6 = *mBookY6;
                }
                thPracParam.score = *mScore;
                thPracParam.life = (float)*mLife;
                thPracParam.bomb = (float)*mBomb;
                thPracParam.power = (float)*mPower;
                thPracParam.graze = *mGraze;
                thPracParam.point = *mPoint;

                thPracParam.rank = *mRank;
                thPracParam.rankLock = *mRankLock;
                if (thPracParam.section >= TH06_ST4_BOSS1 && thPracParam.section <= TH06_ST4_BOSS7)
                    thPracParam.fakeType = *mFakeShot;
                break;
            default:
                break;
            }
        }

        void SpellPhase()
        {
            auto section = CalcSection();
            if (section == TH06_ST7_END_S10) {
                mPhase(TH_PHASE, TH_SPELL_PHASE1);
            }else if (section == TH06_ST4_BOOKS) {
                mPhase(TH_PHASE, TH_BOOKS_PHASE_INF_TIME);
                if (*mPhase == 4){
                    float w = ImGui::GetColumnWidth();
                    ImGui::Columns(3);
                    ImGui::SetColumnWidth(0, 0.2f*w);
                    ImGui::SetColumnWidth(1, 0.4f*w);
                    ImGui::SetColumnWidth(2, 0.4f*w);

                    mBookC1();ImGui::NextColumn(); mBookX1();ImGui::NextColumn();mBookY1();ImGui::NextColumn();
                    mBookC2();ImGui::NextColumn(); mBookX2();ImGui::NextColumn();mBookY2();ImGui::NextColumn();
                    mBookC3();ImGui::NextColumn(); mBookX3();ImGui::NextColumn();mBookY3();ImGui::NextColumn();
                    mBookC4();ImGui::NextColumn(); mBookX4();ImGui::NextColumn();mBookY4();ImGui::NextColumn();
                    mBookC5();ImGui::NextColumn(); mBookX5();ImGui::NextColumn();mBookY5();ImGui::NextColumn();
                    mBookC6();ImGui::NextColumn(); mBookX6();ImGui::NextColumn();mBookY6();ImGui::NextColumn();
                    ImGui::Columns(1);
                    if (ImGui::Button(S(TH_BOOK_MIRROR))) {
                        *mBookX4 = -*mBookX3;
                        *mBookX5 = -*mBookX2;
                        *mBookX6 = -*mBookX1;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(S(TH_BOOK_MIRROR2))) {
                        *mBookX1 = -*mBookX1;
                        *mBookX2 = -*mBookX2;
                        *mBookX3 = -*mBookX3;
                        *mBookX4 = -*mBookX4;
                        *mBookX5 = -*mBookX5;
                        *mBookX6 = -*mBookX6;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(S(TH_BOOK_ROLL))) {
                        int temp = *mBookX6;
                        *mBookX6 = *mBookX5;
                        *mBookX5 = *mBookX4;
                        *mBookX4 = *mBookX3;
                        *mBookX3 = *mBookX2;
                        *mBookX2 = *mBookX1;
                        *mBookX1 = temp;
                    }
                    if (ImGui::Button(S(TH_BOOK_RANDX))) {
                        static std::default_random_engine rand_engine(timeGetTime());
                        static std::uniform_int_distribution<int32_t> rand_value(-192, 192);
                        *mBookX1 = rand_value(rand_engine);
                        *mBookX2 = rand_value(rand_engine);
                        *mBookX3 = rand_value(rand_engine);
                        *mBookX4 = rand_value(rand_engine);
                        *mBookX5 = rand_value(rand_engine);
                        *mBookX6 = rand_value(rand_engine);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(S(TH_BOOK_RESETY))) {
                        *mBookY1 = 32;
                        *mBookY2 = 128;
                        *mBookY3 = 144;
                        *mBookY4 = 64;
                        *mBookY5 = 80;
                        *mBookY6 = 96;
                    }
                    if (ImGui::Button(S(TH_BOOK_COPY_SETTING)))
                    {
                        ImGui::SetClipboardText(std::format("({},{}),({},{}),({},{}),({},{}),({},{}),({},{})", 
                            *mBookX1, *mBookY1, 
                            *mBookX2, *mBookY2, 
                            *mBookX3, *mBookY3,
                            *mBookX4, *mBookY4,
                            *mBookX5, *mBookY5,
                            *mBookX6, *mBookY6
                        ).c_str());
                    }
                    if (ImGui::Button(S(TH_BOOK_PASTE_SETTING))){
                        int xs[6] = { 0 }, ys[6] = { 32,128,144,64,80,96 };
                        auto text = ImGui::GetClipboardText();
                        if (text){
                            int n = 0;
                            while (isspace(text[n]) && text[n+1] != 0)
                                n++;
                            // trim
                            if (text && sscanf_s(text + n, "(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d)", &xs[0], &ys[0], &xs[1], &ys[1], &xs[2], &ys[2], &xs[3], &ys[3], &xs[4], &ys[4], &xs[5], &ys[5])) {
                                *mBookX1 = xs[0], *mBookY1 = ys[0],
                                *mBookX2 = xs[1], *mBookY2 = ys[1],
                                *mBookX3 = xs[2], *mBookY3 = ys[2],
                                *mBookX4 = xs[3], *mBookY4 = ys[3],
                                *mBookX5 = xs[4], *mBookY5 = ys[4],
                                *mBookX6 = xs[5], *mBookY6 = ys[5];
                            }
                        }
                       
                    }
                }
            }else if (section == TH06_ST5_BOSS6) {
                mPhase(TH_PHASE, TH_EOSD_SAKUYA_DOLLS);
            }else if (section == TH06_ST6_BOSS9){
                mPhase(TH_PHASE, TH06_FINAL_SPELL);
                if (*mPhase == 1 || *mPhase == 2 || *mPhase == 3)
                    mDelaySt6Bs9();
                mWallPrac();
                if (*mWallPrac) {
                    mWallPracSnipeN("%d%%");
                    mWallPracSnipeF("%d%%");
                }
            } else if (section == TH06_ST6_BOSS6) {
                mWallPrac();
                if (*mWallPrac){
                    mWallPracSnipeN("%d%%");
                    mWallPracSnipeF("%d%%");
                }
            }
        }
        void PracticeMenu(Gui::GuiNavFocus& nav_focus)
        {
            mMode();
            if (mStage())
                *mSection = *mChapter = 0;
            if (*mMode == 1) {
                if (mWarp()) {
                    *mSection = *mChapter = *mPhase = *mFrame = 0, *mDelaySt6Bs9 = 120,*mWallPrac = false,*mWallPracSnipeF=30,*mWallPracSnipeN=0;
                    //(-180,32),(-116,128),(-61,144),(41,64),(112,80),(180,96)
                    //(-180,32),(-12,128),(-72,144),(68,64),(130,80),(180,96)
                    *mBookC1 = true,*mBookX1 = -180, *mBookY1 = 32;
                    *mBookC2 = true,*mBookX2 = -116, *mBookY2 = 128;
                    *mBookC3 = true, *mBookX3 = -61, *mBookY3 = 144;
                    *mBookC4 = true,*mBookX4 = 41, *mBookY4 = 64;
                    *mBookC5 = true,*mBookX5 = 112, *mBookY5 = 80;
                    *mBookC6 = true,*mBookX6 = 180, *mBookY6 = 96;
                }
                if (*mWarp) {
                    int st = 0;
                    if (*mStage == 3) {
                        mFakeShot();
                        st = (*mFakeShot ? *mFakeShot - 1 : mShotType) + 4;
                    }
                    SectionWidget();
                    SpellPhase();
                }

                mLife();
                mBomb();
                mScore();
                mScore.RoundDown(10);
                mPower();
                mGraze();
                mPoint();
                mRank();
                if (mRankLock()) {
                    if (*mRankLock)
                        mRank.SetBound(0, 99);
                    else
                        mRank.SetBound(0, 32);
                }
            }

            nav_focus();
        }

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_MENU));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSize(330.f, 390.f);
                SetPos(260.f, 65.f);
                SetItemWidth(-60.0f);
                break;
            case Gui::LOCALE_EN_US:
                SetSize(370.f, 375.f);
                SetPos(240.f, 75.f);
                SetItemWidth(-60.0f);
                break;
            case Gui::LOCALE_JA_JP:
                SetSize(330.f, 390.f);
                SetPos(260.f, 65.f);
                SetItemWidth(-65.0f);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(S(TH_MENU));
            ImGui::Separator();

            PracticeMenu(mNavFocus);
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
            case TH06_ST1_BOSS1:
            case TH06_ST2_BOSS1:
            case TH06_ST3_BOSS1:
            case TH06_ST4_BOSS1:
            case TH06_ST5_BOSS1:
            case TH06_ST5_MID1:
            case TH06_ST6_BOSS1:
            case TH06_ST6_MID1:
            case TH06_ST7_END_NS1:
            case TH06_ST7_MID1:
                return true;
            default:
                return false;
            }
        }
        void SectionWidget()
        {
            static char chapterStr[256] {};
            auto& chapterCounts = mChapterSetup[*mStage];

            int st = 0;
            if (*mStage == 3) {
                st = (*mFakeShot ? *mFakeShot - 1 : mShotType) + 4;
            }

            switch (*mWarp) {
            case 1: // Chapter
                mChapter.SetBound(1, chapterCounts[0] + chapterCounts[1]);

                if (chapterCounts[1] == 0) {
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
                if (mSection(TH_WARP_SELECT_FRAME[*mWarp],
                    th_sections_cba[*mStage + st][*mWarp - 2],
                    th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cba[*mStage][*mWarp - 2][*mSection]))
                    mDlg();
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                if (mSection(TH_WARP_SELECT_FRAME[*mWarp],
                    th_sections_cbt[*mStage + st][*mWarp - 4],
                    th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cbt[*mStage][*mWarp - 4][*mSection]))
                    mDlg();
                break;
            case 6:
                mFrame();
                break;
            }
        }


        // Data
        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage { TH_STAGE, TH_STAGE_SELECT };
        Gui::GuiCombo mWarp { TH_WARP, TH_WARP_SELECT_FRAME };
        Gui::GuiCombo mSection { TH_MODE };
        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiCheckBox mDlg { TH_DLG };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mFrame { TH_FRAME, 0, INT_MAX };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 8 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 8 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 0, 128 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, 99999, 1, 10000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mPoint { TH_POINT, 0, 9999, 1, 1000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mDelaySt6Bs9 { TH_DELAY, 0, 600, 1, 10,10};
        Gui::GuiCheckBox mWallPrac { TH06_ST6_WALL_PRAC };


        Gui::GuiDrag<int, ImGuiDataType_S32> mBookX1 { TH_BOOK_X1, -192, 192, 1, 10, 10 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mBookY1 { TH_BOOK_Y1, -50, 448, 1, 10, 10 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mBookX2 { TH_BOOK_X2, -192, 192, 1, 10, 10 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mBookY2 { TH_BOOK_Y2, -50, 448, 1, 10, 10 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mBookX3 { TH_BOOK_X3, -192, 192, 1, 10, 10 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mBookY3 { TH_BOOK_Y3, -50, 448, 1, 10, 10 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mBookX4 { TH_BOOK_X4, -192, 192, 1, 10, 10 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mBookY4 { TH_BOOK_Y4, -50, 448, 1, 10, 10 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mBookX5 { TH_BOOK_X5, -192, 192, 1, 10, 10 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mBookY5 { TH_BOOK_Y5, -50, 448, 1, 10, 10 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mBookX6 { TH_BOOK_X6, -192, 192, 1, 10, 10 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mBookY6 { TH_BOOK_Y6, -50, 448, 1, 10, 10 };

        Gui::GuiSlider<int, ImGuiDataType_S32> mWallPracSnipeF { TH06_ST6_WALL_PRAC_SNIPE_F, 0, 100};
        Gui::GuiSlider<int, ImGuiDataType_S32> mWallPracSnipeN { TH06_ST6_WALL_PRAC_SNIPE_N, 0, 100};

        Gui::GuiCheckBox                           mBookC1 { TH_BOOK_C1};
        Gui::GuiCheckBox                           mBookC2 { TH_BOOK_C2};
        Gui::GuiCheckBox                           mBookC3 { TH_BOOK_C3};
        Gui::GuiCheckBox                           mBookC4 { TH_BOOK_C4};
        Gui::GuiCheckBox                           mBookC5 { TH_BOOK_C5};
        Gui::GuiCheckBox                           mBookC6 { TH_BOOK_C6};


        Gui::GuiSlider<int, ImGuiDataType_S32> mRank { TH06_RANK, 0, 32, 1, 10, 10 };
        Gui::GuiCheckBox mRankLock { TH06_RANKLOCK };
        Gui::GuiCombo mFakeShot { TH06_FS, TH06_TYPE_SELECT };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP, TH_FRAME,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_LIFE, TH_BOMB, TH_SCORE, TH_POWER, TH_GRAZE, TH_POINT,
            TH06_RANK, TH06_RANKLOCK, TH06_FS,
            TH_BOOK_X1,TH_BOOK_Y1,
            TH_BOOK_X2,TH_BOOK_Y2,
            TH_BOOK_X3,TH_BOOK_Y3,
            TH_BOOK_X4,TH_BOOK_Y4,
            TH_BOOK_X5,TH_BOOK_Y5,
            TH_BOOK_X6,TH_BOOK_Y6,
        };

        int mChapterSetup[7][2] {
            { 4, 2 },
            { 2, 2 },
            { 4, 3 },
            { 4, 5 },
            { 3, 2 },
            { 2, 0 },
            { 4, 3 }
        };

        float mStep = 10.0;
        int mDiffculty = 0;
        int mShotType = 0;
    };
    
    class THPauseMenu : public Gui::GameGuiWnd {
        THPauseMenu() noexcept
        {
            SetTitle("Pause Menu");
            SetFade(0.8f, 0.1f);
            SetSize(384.f, 448.f);
            SetPos(32.f, 16.f);
            SetItemWidth(-60);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
        }
        SINGLETON(THPauseMenu);
    public:

        bool el_bgm_signal { false };
        bool el_bgm_changed { false };

        enum state {
            STATE_CLOSE = 0,
            STATE_RESUME = 1,
            STATE_EXIT = 2,
            STATE_RESTART = 3,
            STATE_OPEN = 4,
            STATE_EXIT2 = 5,
        };
        enum signal {
            SIGNAL_NONE = 0,
            SIGNAL_RESUME = 1,
            SIGNAL_EXIT = 2,
            SIGNAL_RESTART = 3,
            SIGNAL_EXIT2 = 4,
        };
        signal PMState()
        {
            switch (mState) {
            case THPrac::TH06::THPauseMenu::STATE_CLOSE:
                return StateClose();
            case THPrac::TH06::THPauseMenu::STATE_RESUME:
                return StateResume();
            case THPrac::TH06::THPauseMenu::STATE_EXIT:
                return StateExit();
            case THPrac::TH06::THPauseMenu::STATE_EXIT2:
                return StateExit2();
            case THPrac::TH06::THPauseMenu::STATE_RESTART:
                return StateRestart();
            case THPrac::TH06::THPauseMenu::STATE_OPEN:
                return StateOpen();
            default:
                return SIGNAL_NONE;
            }
        }

    protected:
        signal StateRestart()
        {
            if (mState != STATE_RESTART) {
                mState = STATE_RESTART;
                mFrameCounter = 0;
            }

            if (mFrameCounter == 1) {
                *mNavFocus = 0;
                inSettings = false;

                auto oldMode = thPracParam.mode;
                auto oldStage = thPracParam.stage;
                auto oldBgmFlag = THBGMTest();
                thRestartFlag = true;
                THGuiPrac::singleton().State(5);
                if (*(THOverlay::singleton().mElBgm) && !el_bgm_changed && oldMode == thPracParam.mode && oldStage == thPracParam.stage && oldBgmFlag == THBGMTest()) {
                    el_bgm_signal = true;
                }

                Close();
            } else if (mFrameCounter == 10) {
                StateClose();
                return SIGNAL_RESTART;
            }

            return SIGNAL_NONE;
        }
        signal StateExit()
        {
            if (mState != STATE_EXIT) {
                mState = STATE_EXIT;
                mFrameCounter = 0;
            }

            if (mFrameCounter == 1) {
                *mNavFocus = 0;
                inSettings = false;
                Close();
            } else if (mFrameCounter == 10) {
                StateClose();
                return SIGNAL_EXIT;
            }

            return SIGNAL_NONE;
        }
        signal StateExit2()
        {
            if (mState != STATE_EXIT2) {
                mState = STATE_EXIT2;
                mFrameCounter = 0;
            }
            if (mFrameCounter == 1) {
                *mNavFocus = 0;
                inSettings = false;
                Close();
            } else if (mFrameCounter == 10) {
                StateClose();
                return SIGNAL_EXIT2;
            }
            return SIGNAL_NONE;
        }
        signal StateResume()
        {
            if (mState != STATE_RESUME) {
                mState = STATE_RESUME;
                mFrameCounter = 0;
            }

            if (mFrameCounter == 1) {
                *mNavFocus = 0;
                inSettings = false;
                Close();
            } else if (mFrameCounter == 10) {
                StateClose();
                return SIGNAL_RESUME;
            }

            return SIGNAL_NONE;
        }
        signal StateClose()
        {
            if (mState != STATE_CLOSE) {
                mState = STATE_CLOSE;
                mFrameCounter = 0;
            }

            if (mFrameCounter > 5) {
                return StateOpen();
            }

            return SIGNAL_NONE;
        }
        signal StateOpen()
        {
            if (mState != STATE_OPEN) {
                mState = STATE_OPEN;
                mFrameCounter = 0;
            }

            if (mFrameCounter == 1) {
                Open();
            }
            if (mFrameCounter > 10) {
                if (Gui::KeyboardInputGetSingle(VK_ESCAPE))
                    StateResume();
                else if (Gui::KeyboardInputGetRaw(0x51))
                    StateExit2();
                else if (Gui::KeyboardInputGetRaw(0x52))
                    StateRestart();
            }

            return SIGNAL_NONE;
        }

        virtual void OnPreUpdate() override
        {
            if (mFrameCounter < UINT_MAX)
                mFrameCounter++;
        }
        virtual void OnLocaleChange() override
        {
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetItemWidth(-60.0f);
                break;
            case Gui::LOCALE_EN_US:
                SetItemWidth(-60.0f);
                break;
            case Gui::LOCALE_JA_JP:
                SetItemWidth(-65.0f);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            if (!inSettings) {
                ImGui::Dummy(ImVec2(10.0f, 140.0f));
                ImGui::Indent(119.0f);
                if (mResume())
                    StateResume();
                if (ImGui::IsItemFocused()) {
                    WORD key = *(WORD*)(0x69D904);
                    WORD key_last = *(WORD*)(0x69D908);
                    if (((key & (16)) == 16 && (key & (16)) != (key_last & (16)))) { // up
                        mNavFocus.ForceFocus(TH_TWEAK);
                    }
                }
                ImGui::Spacing();
                if (mExit())
                    StateExit();
                ImGui::Spacing();
                if (mExit2())
                    StateExit2();
                ImGui::Spacing();
                if (mRestart())
                    StateRestart();
                ImGui::Spacing();
                if (mSettings())
                    inSettings = !inSettings;
                if (ImGui::IsItemFocused()) {
                    WORD key = *(WORD*)(0x69D904);
                    WORD key_last = *(WORD*)(0x69D908);
                    if (((key & (32)) == 32 && (key & (32)) != (key_last & (32)))) { // down
                        mNavFocus.ForceFocus(TH_RESUME);
                    }
                }
                ImGui::Spacing();
                ImGui::Unindent();
                mNavFocus();
            } else {
                ImGui::Dummy(ImVec2(10.0f, 10.0f));
                ImGui::Indent(119.0f);
                if (mResume())
                    StateResume();
                ImGui::Spacing();
                if (mExit())
                    StateExit();
                ImGui::Spacing();
                if (mExit2())
                    StateExit2();
                ImGui::Spacing();
                if (mRestart())
                    StateRestart();
                ImGui::Spacing();
                if (mSettings())
                    inSettings = !inSettings;
                ImGui::Spacing();
                ImGui::Unindent(67.0f);
                THGuiPrac::singleton().PracticeMenu(mNavFocus);
            }
        }

        // Var
        state mState = STATE_CLOSE;
        unsigned int mFrameCounter = 0;
        bool inSettings = false;

        Gui::GuiButton mResume { TH_RESUME, 130.0f, 25.0f };
        Gui::GuiButton mExit { TH_EXIT, 130.0f, 25.0f };
        Gui::GuiButton mExit2 { TH_EXIT2, 130.0f, 25.0f };
        Gui::GuiButton mRestart { TH_RESTART, 130.0f, 25.0f };
        Gui::GuiButton mSettings { TH_TWEAK, 130.0f, 25.0f };

        Gui::GuiNavFocus mNavFocus { TH_RESUME, TH_EXIT, TH_EXIT2, TH_RESTART, TH_TWEAK,
            TH_STAGE, TH_MODE, TH_WARP,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE,
            TH_LIFE, TH_BOMB, TH_SCORE, TH_POWER, TH_GRAZE, TH_POINT,
            TH06_RANK, TH06_RANKLOCK, TH06_FS };
    };

    class THGuiRep : public Gui::GameGuiWnd {
        THGuiRep() noexcept
        {
        }
        SINGLETON(THGuiRep);
    public:

        void CheckReplay()
        {
            uint32_t index = GetMemContent(0x6d46c0 + 0x81e8);
            char* raw = (char*)(0x6d46c0 + index * 512 + 0x823c);

            std::string param;
            if (ReplayLoadParam(mb_to_utf16(raw, 932).c_str(), param) && mRepParam.ReadJson(param))
                mParamStatus = true;
            else
                mRepParam.Reset();
        }

        bool mRepStatus = false;
        void State(int state)
        {
            switch (state) {
            case 1:
                mRepStatus = false;
                mParamStatus = false;
                thPracParam.Reset();
                break;
            case 2:
                CheckReplay();
                break;
            case 3:
                mRepStatus = true;
                if (mParamStatus)
                    memcpy(&thPracParam, &mRepParam, sizeof(THPracParam));
                break;
            default:
                break;
            }
        }

    protected:
        bool mParamStatus = false;
        THPracParam mRepParam;
    };

    float g_bossMoveDownRange = BOSS_MOVE_DOWN_RANGE_INIT;
    EHOOK_ST(th06_bossmovedown, 0x0040917F, 5, {
        float* left = (float*)(pCtx->Ecx + 0xE60);
        float* top = (float*)(pCtx->Ecx + 0xE64);
        float* right = (float*)(pCtx->Ecx + 0xE68);
        float* bottom = (float*)(pCtx->Ecx + 0xE6C);
        float range = *bottom - *top;
        *top = *bottom - range * (1.0f - g_bossMoveDownRange);
    });

    HOOKSET_DEFINE(th06_rankdown_disable)
    PATCH_DY(th06_rankdown_disable1, 0x428C34, "909090909090909090909090909090")
    PATCH_DY(th06_rankdown_disable2, 0x428A55, "909090909090909090909090909090")
    HOOKSET_ENDDEF()

    class THAdvOptWnd : public Gui::PPGuiWnd {
        SINGLETON(THAdvOptWnd);
        // Option Related Functions

    private:
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th06.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th06.dll", hash);
                if (hash[0] != 3665784961181135876ll || hash[1] != 9283021252209177490ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x17024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x17034);
                }
            } else {
                mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th06_unicode.dll");
                if (mOptCtx.vpatch_base) {
                    uint64_t hash[2];
                    CalcFileHash(L"vpatch_th06_unicode.dll", hash);
                    if (hash[0] != 5021620919341617817ll || hash[1] != 10919509441391235291ll)
                        mOptCtx.fps_status = -1;
                    else if (*(int32_t*)(mOptCtx.vpatch_base + 0x17024) == 0) {
                        mOptCtx.fps_status = 2;
                        mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x17034);
                    }
                } else
                    mOptCtx.fps_status = 0;
            }
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                *(int32_t*)(mOptCtx.vpatch_base + 0x15a4c) = mOptCtx.fps;
                *(int32_t*)(mOptCtx.vpatch_base + 0x17034) = mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
        }
        void GameplaySet()
        {
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

            for (int i = 0; i < 2; i++)
                th06_rankdown_disable[i].Setup();
            for (int i = 0; i < 2; i++)
                th06_rankdown_disable[i].Toggle(g_adv_igi_options.disable_master_autoly);
            th06_bossmovedown.Setup();
            th06_bossmovedown.Toggle(false);
            GameplayInit();
        }

    public:
        bool forceBossMoveDown = false;
        __declspec(noinline) static bool StaticUpdate()
        {
            auto& advOptWnd = THAdvOptWnd::singleton();

            if (Gui::KeyboardInputUpdate(VK_F12) == 1) {
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
            SetTitle(S(TH_SPELL_PRAC));
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
        void ContentUpdate()
        {
            *((int32_t*)0x6c6eb0) = 3;
            ImGui::TextUnformatted(S(TH_ADV_OPT));
            ImGui::Separator();
            ImGui::BeginChild("Adv. Options", ImVec2(0.0f, 0.0f));
            
            if (BeginOptGroup<TH_GAME_SPEED>()) {
                if (GameFPSOpt(mOptCtx))
                    FpsSet();
                EndOptGroup();
            }
            {
                if (ImGui::Checkbox(S(TH06_RANKLOCK_DOWN), &g_adv_igi_options.th06_disable_drop_rank)){

                    for (int i = 0; i < 2; i++)
                        th06_rankdown_disable[i].Toggle(g_adv_igi_options.th06_disable_drop_rank);
                }
                ImGui::SameLine();
                HelpMarker(S(TH06_RANKLOCK_DOWN_DESC));
                if (ImGui::IsKeyDown(0x10)) // shift
                {
                    if (ImGui::IsKeyPressed('C'))
                    {
                        g_adv_igi_options.th06_disable_drop_rank = !g_adv_igi_options.th06_disable_drop_rank;
                        for (int i = 0; i < 2; i++)
                            th06_rankdown_disable[i].Toggle(g_adv_igi_options.th06_disable_drop_rank);
                    }
                }
            }
            DisableKeyOpt();
            KeyHUDOpt();
            InfLifeOpt();
            if (ImGui::Checkbox(S(TH_BOSS_FORCE_MOVE_DOWN), &forceBossMoveDown)) {
                th06_bossmovedown.Toggle(forceBossMoveDown);
            }
            ImGui::SameLine();
            HelpMarker(S(TH_BOSS_FORCE_MOVE_DOWN_DESC));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(180.0f);
            if (ImGui::DragFloat(S(TH_BOSS_FORCE_MOVE_DOWN_RANGE), &g_bossMoveDownRange, 0.002f, 0.0f, 1.0f))
                g_bossMoveDownRange = std::clamp(g_bossMoveDownRange, 0.0f, 1.0f);

            ImGui::Checkbox(S(THPRAC_SHOW_BULLET_HITBOX), &g_show_bullet_hitbox);

            ImGui::Checkbox(S(THPRAC_INGAMEINFO_TH06_SHOW_RANK), &g_adv_igi_options.th06_showRank);
            ImGui::Checkbox(S(THPRAC_INGAMEINFO_TH06_SHOW_HITBOX), &g_adv_igi_options.th06_show_hitbox);
            ImGui::SameLine();
            HelpMarker(S(THPRAC_INGAMEINFO_TH06_SHOW_HITBOX_DESC));
            ImGui::SameLine();
            if (ImGui::Button(S(THPRAC_INGAMEINFO_TH06_SHOW_HITBOX_RELOAD))) {
                if (g_hitbox_textureID)
                {
                    ((LPDIRECT3DTEXTURE8)g_hitbox_textureID)->Release();
                    g_hitbox_textureID = nullptr;
                }
                g_hitbox_textureID = ReadImage(8, *(DWORD*)0x6c6d20, "hitbox.png", hitbox_file, sizeof(hitbox_file));
                D3DSURFACE_DESC desc;
                ((LPDIRECT3DTEXTURE8)g_hitbox_textureID)->GetLevelDesc(0, &desc);
                g_hitbox_sz.x = desc.Width,g_hitbox_sz.y = desc.Height;
            }
            ImGui::Checkbox(S(THPRAC_TH06_SHOW_REP_MARKER), &g_adv_igi_options.th06_showRepMarker);
            ImGui::Checkbox(S(THPRAC_TH06_FIX_RAND_SEED), &g_adv_igi_options.th06_fix_seed);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(180.0f);
            if (ImGui::InputInt(S(THPRAC_TH06_RAND_SEED), &g_adv_igi_options.th06_seed)){
                g_adv_igi_options.th06_seed = std::clamp(g_adv_igi_options.th06_seed, 0, 65535);
            }
            ImGui::Text("%s: %d",S(THPRAC_TH06_REP_RAND_SEED),g_last_rep_seed);

            ImGui::Checkbox(S(THPRAC_TH06_BACKGROUND_FIX), &g_adv_igi_options.th06_bg_fix);
            HelpMarker(S(THPRAC_INGAMEINFO_ADV_DESC1));
            ImGui::SameLine();
            HelpMarker(S(THPRAC_INGAMEINFO_ADV_DESC2));

            SSS_UI();

            {
                ImGui::SetNextWindowCollapsed(false);
                if (ImGui::CollapsingHeader(S(THPRAC_INGAMEINFO_06_SHOWDETAIL_COLLAPSE)))
                {
                    TH06InGameInfo::singleton().ShowDetail(nullptr);
                    ImGui::NewLine();
                    ImGui::Separator();
                    ImGui::Separator();
                    ImGui::Separator();
                    ImGui::NewLine();
                }
                ImGui::Text("%s:(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d)", S(TH_BOOK_LAST), 
                    (int)(g_books_pos[0].x - 192.0f), (int)g_books_pos[0].y,
                    (int)(g_books_pos[1].x - 192.0f), (int)g_books_pos[1].y,
                    (int)(g_books_pos[2].x - 192.0f), (int)g_books_pos[2].y,
                    (int)(g_books_pos[3].x - 192.0f), (int)g_books_pos[3].y,
                    (int)(g_books_pos[4].x - 192.0f), (int)g_books_pos[4].y,
                    (int)(g_books_pos[5].x - 192.0f), (int)g_books_pos[5].y
                );
                if (ImGui::Button(S(TH_BOOK_COPY_SETTING))) {
                    ImGui::SetClipboardText(std::format("({},{}),({},{}),({},{}),({},{}),({},{}),({},{})", 
                        (int)(g_books_pos[0].x - 192.0f), (int)g_books_pos[0].y,
                        (int)(g_books_pos[1].x - 192.0f), (int)g_books_pos[1].y,
                        (int)(g_books_pos[2].x - 192.0f), (int)g_books_pos[2].y,
                        (int)(g_books_pos[3].x - 192.0f), (int)g_books_pos[3].y,
                        (int)(g_books_pos[4].x - 192.0f), (int)g_books_pos[4].y,
                        (int)(g_books_pos[5].x - 192.0f), (int)g_books_pos[5].y).c_str());
                }
            }
            
            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;


    };

    // ECL Patch Helper

    void ECLJump(ECLHelper& ecl,int32_t time,int32_t pos, int32_t time_jmp,int32_t target)
    {
        ecl << pair { pos, (int32_t)time } << pair { pos + 4, (int16_t)0x2 } << pair { pos + 6, (int16_t)0x14 } << pair { pos + 8, (int32_t)0x00FFFF00 };
        ecl << pair { pos + 0xC, (int32_t)time_jmp } << pair { pos + 0xC + 4, (int32_t)target - pos };
    }

    void ECLWarp(int32_t time)
    {
        *((int32_t*)(0x5a5fb0)) = time;
    }
    void ECLSetHealth(ECLHelper& ecl, int offset, int32_t ecl_time, int32_t time)
    {
        ecl.SetPos(offset);
        ecl << ecl_time << 0x0010006f << 0x00ffff00 << time;
    }
    void ECLSetTime(ECLHelper& ecl, int offset, int32_t ecl_time, int32_t health)
    {
        ecl.SetPos(offset);
        ecl << ecl_time << 0x00100073 << 0x00ffff00 << health;
    }
    void ECLStall(ECLHelper& ecl, int offset)
    {
        ecl.SetPos(offset);
        ecl << 0x99999 << 0x000c0000 << 0x0000ff00;
    }
    void ECLNameFix()
    {
        void* thisPtr = *((void**)0x6d4588);
        if (thPracParam.stage == 5) {
            asm_call<0x431dc0, Thiscall>(thisPtr, 11, "data/eff06.anm", 691);
        } else if (thPracParam.stage == 6) {
            asm_call<0x431dc0, Thiscall>(thisPtr, 11, "data/eff07.anm", 691);
            asm_call<0x431dc0, Thiscall>(thisPtr, 18, "data/face12c.anm", 1192);
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        int shot;
        auto s2b_nd = [&]() {
            ECLWarp(0x1760);
            ecl << pair{0x18fc, 0x0};
            ecl << pair{0x191c, 0x0};
            ecl << pair{0x192c, 0x0};
            ecl << pair{0x194c, 0x0};
            ecl << pair{0x196c, 0x0};
            ecl << pair{0x198c, 0x0};
            ecl << pair{0x19a0, 0x0};
        };
        auto s3b_n1 = [&]() {
            ecl << pair{0x1274, (int16_t)0x0};
            ecl << pair{0x12f0, (int16_t)0x0};
            ECLWarp(0x16d4);
            ecl << pair{0x80d6, (int16_t)0x16d4};
            ecl << pair{0x1f70, 0x1};
            ecl << pair{0x1f90, 0x0};
            ecl << pair{0x80dc, (int16_t)0x24};
            ecl << pair{0x20ec, 0x0};
            ecl << pair{0x210c, 0x0};
            ecl << pair{0x212c, 0x0};
            ecl << pair{0x214c, 0x1e};
            ecl << pair{0x2160, 0x1e};
            ecl << pair{0x2194, 0x1e};
            ecl << pair{0x2188, 150};
        };
        auto s4b_time = [&]() {
            ecl << pair{0x2790, 0x0};
            ecl << pair{0x27b0, 0x0};
            ecl << pair{0x27d0, 0x0};
            ecl << pair{0x27f0, 0x0};
            ecl << pair{0x2810, 0x0};
            ecl << pair{0x2824, 0x0};
            ecl << pair{0x283c, 0x0};
            ecl << pair{0x2850, 0x0};
        };
        auto s7b_call = [&]() {
            ecl << pair{0x344e, 0x0};
            ecl << pair{0x3452, 0x00180023};
            ecl << pair{0x3456, 0x00ffff00};
            ecl << pair{0x345a, 0x0};
            ecl << pair{0x345e, 0x0};
            ecl << pair{0x3462, 0x0};
            ECLStall(ecl, 0x3466);
        };
        auto s7b_n1 = [&]() {
            ECLWarp(0x2192);
            ecl << pair{0x339e, 0x0};
            ecl << pair{0x33ae, 0x0};
            ecl << pair{0x33ce, 0x0};
            ecl << pair{0x33ee, 0x0};
            ecl << pair{0x340e, 0x0};
            ecl << pair{0x342e, 0x0};
        };

        switch (section) {
        case THPrac::TH06::TH06_ST1_MID1:
            ECLWarp(0x7d8);
            ecl << pair{0x0ab0, 0x3c} << pair{0x0ad0, 0x3c};
            break;
        case THPrac::TH06::TH06_ST1_MID2:
            ECLWarp(0x7d8);
            ecl << pair{0x0ab0, 0x3c} << pair{0x0ad0, 0x3c};
            ECLSetHealth(ecl, 0x0af0, 0x3c, 0x1f3);
            break;
        case THPrac::TH06::TH06_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(0x149e);
            else {
                ECLWarp(0x149f);
                ecl << pair{0x16a6, 0} << pair{0x16c6, 0} << pair{0x16e6, 0x50};
            }
            break;
        case THPrac::TH06::TH06_ST1_BOSS2:
            ECLWarp(0x149f);
            ecl << pair{0x16a6, 0} << pair{0x16c6, 0} << pair{0x16e6, 0x50};
            ECLSetTime(ecl, 0x16e6, 0, 0);
            ECLStall(ecl, 0x16f6);
            break;
        case THPrac::TH06::TH06_ST1_BOSS3:
            ECLWarp(0x149f);
            ecl << pair{0x16a6, 0} << pair{0x16c6, 0} << pair{0x16e6, 0x50}
                << pair{0x16f2, 0x10} << pair{0x293a, 0} << pair{0x294a, 0}
                << pair{0x291e, (int16_t)0};
            break;
        case THPrac::TH06::TH06_ST1_BOSS4:
            ECLWarp(0x149f);
            ecl << pair{0x16a6, 0} << pair{0x16c6, 0} << pair{0x16e6, 0x50}
                << pair{0x16f2, 0x10} << pair{0x293a, 0} << pair{0x294a, 0}
                << pair{0x291e, (int16_t)0};
            ECLSetTime(ecl, 0x294a, 0, 0);
            ECLStall(ecl, 0x295a);
            break;
        case THPrac::TH06::TH06_ST2_MID1:
            ECLWarp(0xa1c);
            break;
        case THPrac::TH06::TH06_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(0x175f);
            else
                ECLWarp(0x1760);
            break;
        case THPrac::TH06::TH06_ST2_BOSS2:
            s2b_nd();
            ECLSetTime(ecl, 0x19a0, 0x0, 0x0);
            ECLStall(ecl, 0x19b0);
            break;
        case THPrac::TH06::TH06_ST2_BOSS3:
            s2b_nd();
            ecl << pair{0x19ac, 0x19};
            ecl << pair{0x2138, 0x0};
            ecl << pair{0x2148, 0x60};
            ecl << pair{0x2110, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST2_BOSS4:
            s2b_nd();
            ecl << pair{0x19ac, 0x19};
            ecl << pair{0x2138, 0x0};
            ecl << pair{0x2148, 0x60};
            ecl << pair{0x2110, (int16_t)0x0};
            ECLSetTime(ecl, 0x2148, 0x30, 0x0);
            ECLStall(ecl, 0x2158);
            break;
        case THPrac::TH06::TH06_ST2_BOSS5:
            s2b_nd();
            ecl << pair{0x19ac, 0x19};
            ecl << pair{0x2138, 0x0};
            ecl << pair{0x2148, 0x60};
            ecl << pair{0x2110, (int16_t)0x0};
            ecl << pair{0x2148, 0x0};
            ecl << pair{0x2154, 0x20};
            ecl << pair{0x33a2, (int16_t)0x0};
            ecl << pair{0x337a, (int16_t)0x0};
            ecl << pair{0x3392, (int16_t)0x0};
            ecl << pair{0x2090, 0x578};
            ecl << pair{0x20b0, 0xffffffff};
            ecl << pair{0x20c0, 0xffffffff};
            ecl << pair{0x20f0, 0x1c};
            break;
        case THPrac::TH06::TH06_ST3_MID1:
            ECLWarp(0x0edc);
            break;
        case THPrac::TH06::TH06_ST3_MID2:
            ECLWarp(0x0edc);
            ecl << pair{0x1274, (int16_t)0x0};
            ecl << pair{0x12f0, (int16_t)0x0};
            ecl << pair{0x1018, 0x0};
            ECLSetHealth(ecl, 0x10dc, 0x1e, 0x513);
            ECLStall(ecl, 0x10ec);
            break;
        case THPrac::TH06::TH06_ST3_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(0x16d4);
            else
                s3b_n1();
            break;
        case THPrac::TH06::TH06_ST3_BOSS2:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ECLSetTime(ecl, 0x2160, 0x0, 0x0);
            ECLStall(ecl, 0x2170);
            break;
        case THPrac::TH06::TH06_ST3_BOSS3:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ecl << pair{0x2160, 0x0};
            ecl << pair{0x216c, 0x14};
            ecl << pair{0x25f4, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST3_BOSS4:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ecl << pair{0x2160, 0x0};
            ecl << pair{0x216c, 0x14};
            ecl << pair{0x25f4, (int16_t)0x0};
            ECLSetTime(ecl, 0x267c, 0x0, 0x0);
            ECLStall(ecl, 0x268c);
            break;
        case THPrac::TH06::TH06_ST3_BOSS5:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ecl << pair{0x2160, 0x0};
            ecl << pair{0x216c, 0x1a};
            ecl << pair{0x31d0, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST3_BOSS6:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ecl << pair{0x2160, 0x0};
            ecl << pair{0x216c, 0x1a};
            ecl << pair{0x31d0, (int16_t)0x0};
            ECLSetTime(ecl, 0x3254, 0x0, 0x0);
            ECLStall(ecl, 0x3264);
            break;
        case THPrac::TH06::TH06_ST3_BOSS7:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ecl << pair{0x2160, 0x0};
            ecl << pair{0x216c, 0x1a};
            ecl << pair{0x31d0, (int16_t)0x0};
            ECLSetTime(ecl, 0x3254, 0x0, 0x0);
            ECLStall(ecl, 0x3264);
            ecl << pair{0x3168, 0x7d0};
            ecl << pair{0x31a8, 0x21};
            ecl << pair{0x31b8, 0x21};
            ecl << pair{0x4b64, (int16_t)0x0};
            ecl << pair{0x4bec, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST4_BOOKS:
            switch (thPracParam.phase)
            {
            case 0:
            default:
                break;
            case 1:
                ecl << pair { 0xCE7C, (int16_t)-1 }; // disable timeline after books
                ecl << pair { 0x1324, (int32_t)99999999 }; // loop forever
                break;
            case 2:
                ecl << pair { 0xCE60, (int16_t)-1 }; // disable timeline after books
                ecl << pair { 0x1324, (int32_t)99999999 }; // loop forever
                break;
            case 3:
                ecl << pair { 0xCDFC, (float)12345.0f }; // disable book 2
                ecl << pair { 0xCE7C, (int16_t)-1 }; // disable timeline after books
                ecl << pair { 0x1324, (int32_t)99999999 }; // loop forever
                break;
            case 4:{
                int32_t ofs = 0xCDD8;
                if (thPracParam.bF & (1<<0)) ecl << pair { ofs, (int16_t)0 } << pair { ofs + 0x4, (float)thPracParam.bX1 + 192.0f } << pair { ofs + 0x8, (float)thPracParam.bY1 }; ofs += 0x1C;
                if (thPracParam.bF & (1<<1)) ecl << pair { ofs, (int16_t)0 } << pair { ofs + 0x4, (float)thPracParam.bX2 + 192.0f } << pair { ofs + 0x8, (float)thPracParam.bY2 }; ofs += 0x1C;
                if (thPracParam.bF & (1<<2)) ecl << pair { ofs, (int16_t)0 } << pair { ofs + 0x4, (float)thPracParam.bX3 + 192.0f } << pair { ofs + 0x8, (float)thPracParam.bY3 }; ofs += 0x1C;
                if (thPracParam.bF & (1<<3)) ecl << pair { ofs, (int16_t)0 } << pair { ofs + 0x4, (float)thPracParam.bX4 + 192.0f } << pair { ofs + 0x8, (float)thPracParam.bY4 }; ofs += 0x1C;
                if (thPracParam.bF & (1<<4)) ecl << pair { ofs, (int16_t)0 } << pair { ofs + 0x4, (float)thPracParam.bX5 + 192.0f } << pair { ofs + 0x8, (float)thPracParam.bY5 }; ofs += 0x1C;
                if (thPracParam.bF & (1<<5)) ecl << pair { ofs, (int16_t)0 } << pair { ofs + 0x4, (float)thPracParam.bX6 + 192.0f } << pair { ofs + 0x8, (float)thPracParam.bY6 };
                ecl << pair { 0xCE7C, (int16_t)-1 }; // disable timeline after books
                ecl << pair { 0x1324, (int32_t)99999999 }; // loop forever
            }
                break;

            }
            ECLWarp(0x0d40);
            break;
        case THPrac::TH06::TH06_ST4_MID1:
            ECLWarp(0x1024);
            break;
        case THPrac::TH06::TH06_ST4_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(0x29c6);
            else
                ECLWarp(0x29c7);
            break;
        case THPrac::TH06::TH06_ST4_BOSS2:
            ECLWarp(0x29c7);
            s4b_time();
            ECLSetTime(ecl, 0x2810, 0x0, 0x0);
            ECLStall(ecl, 0x2820);
            break;
        case THPrac::TH06::TH06_ST4_BOSS3:
            ECLWarp(0x29c7);
            s4b_time();
            ecl << pair{0x2854, (int16_t)0x23};
            ecl << pair{0x285c, 0x25};
            ecl << pair{0x6da0, (int16_t)0x0};
            ECLSetTime(ecl, 0x7440, 0x0, 0x0);
            ECLStall(ecl, 0x7450);
            break;
        case THPrac::TH06::TH06_ST4_BOSS4:
            ECLWarp(0x29c7);
            s4b_time();
            ecl << pair{0x2854, (int16_t)0x23};
            ecl << pair{0x285c, 0x25};
            ecl << pair{0x6da0, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST4_BOSS5:
            ECLWarp(0x29c7);
            s4b_time();
            ecl << pair{0x2854, (int16_t)0x23};
            ecl << pair{0x285c, 0x27};
            ecl << pair{0x7950, (int16_t)0x0};
            ecl << pair{0x7afc, 0x0};
            ecl << pair{0x7b0c, 0x0};
            ecl << pair{0x7b2c, 0x0};
            ecl << pair{0x7b4c, 0x0};
            ecl << pair{0x7b6c, 0x0};
            ecl << pair{0x7b8c, 0x0};
            break;
        case THPrac::TH06::TH06_ST4_BOSS6:
            ECLWarp(0x29c7);
            s4b_time();
            ecl << pair{0x2854, (int16_t)0x23};
            ecl << pair{0x285c, 0x27};
            ecl << pair{0x7950, (int16_t)0x0};

            ECLSetHealth(ecl, 0x7afc, 0, 1699);
            ECLSetHealth(ecl, 0x7b0c, 0, 3399);
            ECLStall(ecl, 0x7b1c);
            ecl << pair{0x7b04, (int16_t)0x0200} << pair{0x7b14, (int16_t)0x0c00};
            ecl << pair{0x7bc8, (int16_t)0}
                << pair{0x7cf4, (int16_t)0} << pair{0x7d0c, (int16_t)0};
            ecl << pair{0x7d18, 0x0} << pair{0x7d28, 0x0}
                << pair{0x7d48, 0x0} << pair{0x7d68, 0x0}
                << pair{0x7d88, 0x0} << pair{0x7da8, 0x0};
            break;
        case THPrac::TH06::TH06_ST4_BOSS7:
            ECLWarp(0x29c7);
            s4b_time();
            ecl << pair{0x2854, (int16_t)0x23};
            ecl << pair{0x285c, 0x27};
            ecl << pair{0x7950, (int16_t)0x0};

            ECLSetHealth(ecl, 0x7afc, 0, 1699);
            ECLStall(ecl, 0x7b0c);
            ecl << pair{0x7a74, (int16_t)41} << pair{0x7a94, (int16_t)41};
            ecl << pair{0x7a54, (int16_t)1700} << pair{0x7a64, (int16_t)1700};
            ecl << pair{0x7de4, (int16_t)0}
                << pair{0x7ed0, (int16_t)0} << pair{0x7ee8, (int16_t)0};
            ecl << pair{0x7ef4, 0x0} << pair{0x7f04, 0x0}
                << pair{0x7f24, 0x0} << pair{0x7f44, 0x0}
                << pair{0x7f64, 0x0} << pair{0x7f84, 0x0};
            break;
        case THPrac::TH06::TH06_ST5_MID1:
            ECLWarp(0x0d2c);
            if (!thPracParam.dlg)
                ecl << pair{0x64a8, (uint16_t)13};
            break;
        case THPrac::TH06::TH06_ST5_MID2:
            ECLWarp(0x0d2c);
            ecl << pair{0x64a4, (int16_t)0x0};
            ECLSetHealth(ecl, 0x14d8, 0x1e, 0x2c5);
            ECLStall(ecl, 0x14e8);
            break;
        case THPrac::TH06::TH06_ST5_BOSS1:
            ECLWarp(0x1e18);
            if (!thPracParam.dlg) {
                ecl << pair{0x767c, (int16_t)0x0};
                ecl << pair{0x22c8, 0x0};
                ecl << pair{0x22e8, 0x0};
                ecl << pair{0x2308, 0x0};
                ecl << pair{0x2328, 0x0};
                ecl << pair{0x2348, 0x0};
                ecl << pair{0x2218, (int16_t)0x0};
            }
            break;
        case THPrac::TH06::TH06_ST5_BOSS2:
            ECLWarp(0x1e18);
            ecl << pair{0x767c, (int16_t)0x0};
            ecl << pair{0x22c8, 0x0};
            ecl << pair{0x22e8, 0x0};
            ecl << pair{0x2308, 0x0};
            ecl << pair{0x2328, 0x0};
            ecl << pair{0x2348, 0x0};
            ecl << pair{0x2218, (int16_t)0x0};
            ECLSetTime(ecl, 0x2348, 0x0, 0x0);
            ECLStall(ecl, 0x2358);
            break;
        case THPrac::TH06::TH06_ST5_BOSS3:
            ECLWarp(0x1e18);
            ecl << pair{0x767c, (int16_t)0x0};
            ecl << pair{0x22c8, 0x0};
            ecl << pair{0x22e8, 0x0};
            ecl << pair{0x2308, 0x0};
            ecl << pair{0x2328, 0x0};
            ecl << pair{0x2348, 0x0};
            ecl << pair{0x2218, (int16_t)0x0};
            ecl << pair{0x235c, 0x0};
            ecl << pair{0x2368, 0x22};
            ecl << pair{0x3778, (int16_t)0x0};
            ecl << pair{0x3828, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST5_BOSS4:
            ECLWarp(0x1e18);
            ecl << pair{0x767c, (int16_t)0x0};
            ecl << pair{0x22c8, 0x0};
            ecl << pair{0x22e8, 0x0};
            ecl << pair{0x2308, 0x0};
            ecl << pair{0x2328, 0x0};
            ecl << pair{0x2348, 0x0};
            ecl << pair{0x2218, (int16_t)0x0};
            ecl << pair{0x235c, 0x0};
            ecl << pair{0x2368, 0x22};
            ecl << pair{0x3778, (int16_t)0x0};
            ecl << pair{0x3828, (int16_t)0x0};
            ECLSetTime(ecl, 0x38b8, 0x0, 0x0);
            ECLStall(ecl, 0x38c8);
            break;
        case THPrac::TH06::TH06_ST5_BOSS5:
            ECLWarp(0x1e18);
            ecl << pair{0x767c, (int16_t)0x0};
            ecl << pair{0x22c8, 0x0};
            ecl << pair{0x22e8, 0x0};
            ecl << pair{0x2308, 0x0};
            ecl << pair{0x2328, 0x0};
            ecl << pair{0x2348, 0x0};
            ecl << pair{0x2218, (int16_t)0x0};
            ecl << pair{0x235c, 0x1e};
            ecl << pair{0x2368, 0x29};
            ecl << pair{0x4638, (int16_t)0x0};
            ecl << pair{0x46e8, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST5_BOSS6:
        {
            std::default_random_engine rand_engine(*(WORD*)(0x69D8F8));
            std::uniform_int_distribution<int32_t> rand_value(0, 640);
            DWORD* pBulletInsertCount = (DWORD*)(0x005A5FF8 + 0xF5C00);
            switch (thPracParam.phase) {
            case 0:
                break;
            case 1:
                *pBulletInsertCount = 640 - 360;
                break;
            case 2:
                *pBulletInsertCount = 550;
                break;
            case 3:
                *pBulletInsertCount = rand_value(rand_engine);
                break;
            }
        }
            ECLWarp(0x1e18);
            ecl << pair{0x767c, (int16_t)0x0};
            ecl << pair{0x22c8, 0x0};
            ecl << pair{0x22e8, 0x0};
            ecl << pair{0x2308, 0x0};
            ecl << pair{0x2328, 0x0};
            ecl << pair{0x2348, 0x0};
            ecl << pair{0x2218, (int16_t)0x0};
            ecl << pair{0x235c, 0x1e};
            ecl << pair{0x2368, 0x29};
            ecl << pair{0x4638, (int16_t)0x0};
            ecl << pair{0x46e8, (int16_t)0x0};
            ecl << pair{0x235c, 0x0};
            ECLSetTime(ecl, 0x4758, 0x0, 0x0);
            ECLStall(ecl, 0x4768);
            break;
        case THPrac::TH06::TH06_ST6_MID1:
            ECLWarp(0x0a04);
            if (!thPracParam.dlg) {
                ecl << pair{0x77f2, (int16_t)0x0};
                ecl << pair{0x9e8, 0x1};
            }
            break;
        case THPrac::TH06::TH06_ST6_MID2:
            shot = (int)(*((int8_t*)0x69d4bd) * 2) + *((int8_t*)0x69d4be);
            if (shot > 1)
                shot = 1099;
            else if (!shot)
                shot = 749;
            else
                shot = 999;
            ECLWarp(0x0a04);
            ecl << pair{0x77f2, (int16_t)0x0};
            ecl << pair{0x0d2c, 0x0};
            ECLSetHealth(ecl, 0x0d3c, 0x0, shot);
            ECLStall(ecl, 0x0d4c);
            break;
        case THPrac::TH06::TH06_ST6_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(0x0c5f);
            else {
                ECLWarp(0x0c61);
                ECLNameFix();
                ecl << pair{0x1686, 0x0};
                ecl << pair{0x16a6, 0x0};
                ecl << pair{0x16c6, 0x0};
                ecl << pair{0x16e6, 0x0};
                ecl << pair{0x15d6, (int16_t)0x0};
            }
            break;
        case THPrac::TH06::TH06_ST6_BOSS2:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ECLSetTime(ecl, 0x1706, 0x0, 0x0);
            ECLStall(ecl, 0x1716);
            break;
        case THPrac::TH06::TH06_ST6_BOSS3:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x0};
            ecl << pair{0x1726, 0x13};
            ecl << pair{0x1b8e, (int16_t)0x0};
            ecl << pair{0x1c3e, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST6_BOSS4:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x0};
            ecl << pair{0x1726, 0x13};
            ecl << pair{0x1b8e, (int16_t)0x0};
            ecl << pair{0x1c3e, (int16_t)0x0};
            ECLSetTime(ecl, 0x1cf2, 0x0, 0x0);
            ECLStall(ecl, 0x1d02);
            break;
        case THPrac::TH06::TH06_ST6_BOSS5:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x1e};
            ecl << pair{0x1726, 0x17};
            ecl << pair{0x28e2, (int16_t)0x0};
            ecl << pair{0x2992, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST6_BOSS6:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x1e};
            ecl << pair{0x1726, 0x17};
            ecl << pair{0x28e2, (int16_t)0x0};
            ecl << pair{0x2992, (int16_t)0x0};
            ecl << pair{0x171a, 0x0};
            ECLSetTime(ecl, 0x2a22, 0x0, 0x0);
            ECLStall(ecl, 0x2a32);
            break;
        case THPrac::TH06::TH06_ST6_BOSS7:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x0};
            ecl << pair{0x1726, 0x1a};
            ecl << pair{0x2d8e, (int16_t)0x0};
            ecl << pair{0x2e3e, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST6_BOSS8:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x0};
            ecl << pair{0x1726, 0x1a};
            ecl << pair{0x2d8e, (int16_t)0x0};
            ecl << pair{0x2e3e, (int16_t)0x0};
            ECLSetTime(ecl, 0x2ee6, 0x0, 0x0);
            ECLStall(ecl, 0x2ef6);
            break;
        case THPrac::TH06::TH06_ST6_BOSS9:
            ECLNameFix();
            ECLWarp(0x0c61);
            
            switch (thPracParam.phase) {
            case 0:
                break;
            case 1:
                // full 56 way
                ECLJump(ecl, 180 + thPracParam.delay_st6bs9, 0x6592, 180, 0x6486);
                break;
            case 2:
                // super full 56 way
                ECLJump(ecl, 180 + thPracParam.delay_st6bs9, 0x6592, 180, 0x6486);
                ecl << pair { 0x658A, (int32_t)9 };
                break;
            case 3:
            {// half full 56 way
                const uint8_t data[132] = {
                    0x04, 0x01, 0x00, 0x00, 0x52, 0x00, 0x2C, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x78, 0x00, 0x00, 0x00,
                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x6A, 0xBC, 0x3C,
                    0xDB, 0x0F, 0xC9, 0x3C, 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x80, 0xBF, 0x04, 0x01, 0x00, 0x00,
                    0x46, 0x00, 0x2C, 0x00, 0x00, 0x04, 0xFF, 0x00, 0x09, 0x00, 0x01, 0x00, 0x0A, 0x00, 0x00, 0x00,
                    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x66, 0x66, 0xE6, 0x3F, 0x00, 0x54, 0x1C, 0xC6,
                    0x7C, 0xD9, 0xA0, 0xBE, 0x00, 0x02, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x46, 0x00, 0x2C, 0x00,
                    0x00, 0x08, 0xFF, 0x00, 0x09, 0x00, 0x01, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x80, 0x40, 0x66, 0x66, 0xE6, 0x3F, 0x00, 0x54, 0x1C, 0xC6, 0x7C, 0xD9, 0xA0, 0xBE,
                    0x00, 0x02, 0x00, 0x00
                };
                for (int i=0;i < 132;i++)
                    ecl << pair { 0x67A2 + i,data[i] };
                ECLJump(ecl, 180 + thPracParam.delay_st6bs9, 0x6592, 260, 0x6756);
            }
                break;
            case 4:
                // + wave 3
                ECLJump(ecl, 180, 0x6592, 180, 0x6756);
                break;
            case 5:
                // + wave 2,3
                ECLJump(ecl, 180, 0x6592, 180, 0x668E);
                break;
            }

            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x0};
            ecl << pair{0x1732, 0x0};
            ecl << pair{0x1726, 0x2b};
            ecl << pair{0x1722, (int16_t)0x0300};
            ecl << pair{0x1736, (int16_t)0x23};
            ecl << pair{0x173a, (int16_t)0x0c00};
            ecl << pair{0x173e, 0x2c};
            ecl << pair{0x5c8e, (int16_t)0x0};
            ecl << pair{0x6290, (int16_t)0x0};
            ecl << pair{0x1622, 0xffffffff};
            break;
        case THPrac::TH06::TH06_ST7_MID1:
            ECLWarp(0x1284);
            if (!thPracParam.dlg)
                ecl << pair{0x0d2e2, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_MID2:
            ECLWarp(0x1284);
            ecl << pair{0x0d2e2, (int16_t)0x0};
            ecl << pair{0x1b14, 0x12};
            ecl << pair{0x1c2c, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_MID3:
            ECLWarp(0x1284);
            ecl << pair{0x0d2e2, (int16_t)0x0};
            ecl << pair{0x1b14, 0x13};
            ecl << pair{0x1d7c, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLWarp(0x2191);
            else {
                ECLNameFix();
                s7b_n1();
            }
            break;
        case THPrac::TH06::TH06_ST7_END_S1:
            ECLNameFix();
            s7b_n1();
            ECLSetTime(ecl, 0x344e, 0x0, 0x0);
            break;
        case THPrac::TH06::TH06_ST7_END_NS2:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x23};
            ecl << pair{0x4210, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S2:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x23};
            ecl << pair{0x4210, (int16_t)0x0};
            ecl << pair{0x41fc, 0x0};
            ecl << pair{0x420c, 0x0};
            ECLSetTime(ecl, 0x421c, 0x0, 0x0);
            ECLStall(ecl, 0x422c);
            break;
        case THPrac::TH06::TH06_ST7_END_NS3:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x26};
            ecl << pair{0x4c62, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S3:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x26};
            ecl << pair{0x4c62, (int16_t)0x0};
            ecl << pair{0x4c4e, 0x0};
            ecl << pair{0x4c5e, 0x0};
            ECLSetTime(ecl, 0x4c6e, 0x0, 0x0);
            ECLStall(ecl, 0x4c7e);
            break;
        case THPrac::TH06::TH06_ST7_END_NS4:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x2b};
            ecl << pair{0x59cc, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S4:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x2b};
            ecl << pair{0x59cc, (int16_t)0x0};
            ecl << pair{0x59b8, 0x0};
            ecl << pair{0x59c8, 0x0};
            ECLSetTime(ecl, 0x59d8, 0x0, 0x0);
            ECLStall(ecl, 0x59e8);
            break;
        case THPrac::TH06::TH06_ST7_END_NS5:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x2f};
            ecl << pair{0x63a2, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S5:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x2f};
            ecl << pair{0x63a2, (int16_t)0x0};
            ecl << pair{0x638e, 0x0};
            ecl << pair{0x639e, 0x0};
            ECLSetTime(ecl, 0x63ae, 0x0, 0x0);
            ECLStall(ecl, 0x63be);
            break;
        case THPrac::TH06::TH06_ST7_END_NS6:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x31};
            ecl << pair{0x6b1c, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S6:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x31};
            ecl << pair{0x6b1c, (int16_t)0x0};
            ecl << pair{0x6b08, 0x0};
            ecl << pair{0x6b18, 0x0};
            ECLSetTime(ecl, 0x6b28, 0x0, 0x0);
            ECLStall(ecl, 0x6b38);
            break;
        case THPrac::TH06::TH06_ST7_END_NS7:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x35};
            ecl << pair{0x78aa, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S7:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x35};
            ecl << pair{0x78aa, (int16_t)0x0};
            ecl << pair{0x7896, 0x0};
            ecl << pair{0x78a6, 0x0};
            ECLSetTime(ecl, 0x78b6, 0x0, 0x0);
            ECLStall(ecl, 0x78c6);
            break;
        case THPrac::TH06::TH06_ST7_END_NS8:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x38};
            ecl << pair{0x8508, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S8:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x38};
            ecl << pair{0x8508, (int16_t)0x0};
            ecl << pair{0x84f4, 0x0};
            ecl << pair{0x8504, 0x0};
            ECLSetTime(ecl, 0x8514, 0x0, 0x0);
            ECLStall(ecl, 0x8524);
            break;
        case THPrac::TH06::TH06_ST7_END_S9:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x3b};
            ecl << pair{0x940a, (int16_t)0x0};
            ecl << pair{0x93f6, 0x0};
            ecl << pair{0x9406, 0x0};
            ecl << pair{0x9416, 0x0};
            ecl << pair{0x9422, 0x0};
            ecl << pair{0x943a, 0x0};
            ecl << pair{0x9466, 0x0};
            ecl << pair{0x9472, 0x0};
            ecl << pair{0x9482, 0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S10:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x43};
            ecl << pair{0x0bea4, (int16_t)0x0};
            ecl << pair{0x0be90, 0x0};
            ecl << pair{0x0bea0, 0x0};
            ecl << pair{0x0beb0, 0x0};
            ecl << pair{0x0bed0, 0x0};
            ecl << pair{0x0bef0, 0x0};
            ecl << pair{0x0bf10, 0x0};
            ecl << pair{0x0bf30, 0x0};
            ecl << pair{0x0bf50, 0x0};
            ecl << pair{0x0bf5c, 0x0};
            ecl << pair{0x0bf74, 0x0};
            ecl << pair{0x0bfa0, 0x0};
            ecl << pair{0x0bfac, 0x0};
            ecl << pair{0x0bfbc, 0x0};
            break;
        default:
            break;
        }
    }
    __declspec(noinline) void THStageWarp([[maybe_unused]] ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                ECLWarp(68);
                break;
            case 2:
                ECLWarp(580);
                break;
            case 3:
                ECLWarp(1160);
                break;
            case 4:
                ECLWarp(1540);
                break;
            case 5:
                ECLWarp(2348);
                break;
            case 6:
                ECLWarp(4438);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                ECLWarp(270);
                break;
            case 2:
                ECLWarp(924);
                break;
            case 3:
                ECLWarp(3528);
                break;
            case 4:
                ECLWarp(4563);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                ECLWarp(340);
                break;
            case 2:
                ECLWarp(1050);
                break;
            case 3:
                ECLWarp(1670);
                break;
            case 4:
                ECLWarp(2762);
                break;
            case 5:
                ECLWarp(3807);
                break;
            case 6:
                ECLWarp(4118);
                break;
            case 7:
                ECLWarp(5274);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                ECLWarp(380);
                break;
            case 2:
                ECLWarp(1454);
                break;
            case 3:
                ECLWarp(2328);
                break;
            case 4:
                ECLWarp(0x0d40);
                break;
            case 5:
                ECLWarp(4872);
                break;
            case 6:
                ECLWarp(5712);
                break;
            case 7:
                ECLWarp(7434);
                break;
            case 8:
                ECLWarp(8354);
                break;
            case 9:
                ECLWarp(9784);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                ECLWarp(350);
                break;
            case 2:
                ECLWarp(1352);
                break;
            case 3:
                ECLWarp(2292);
                break;
            case 4:
                ECLWarp(3814);
                break;
            case 5:
                ECLWarp(6774);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                ECLWarp(380);
                break;
            case 2:
                ECLWarp(1484);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                ECLWarp(380);
                break;
            case 2:
                ECLWarp(1300);
                break;
            case 3:
                ECLWarp(2600);
                break;
            case 4:
                ECLWarp(3680);
                break;
            case 5:
                ECLWarp(4803);
                break;
            case 6:
                ECLWarp(5933);
                break;
            case 7:
                ECLWarp(7733);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THSectionPatch()
    {
        ECLHelper ecl;
        ecl.SetBaseAddr((void*)0x487e50);

        auto section = thPracParam.section;
        if (section >= 10000 && section < 20000) {
            int stage = (section - 10000) / 100;
            int portionId = (section - 10000) % 100;
            THStageWarp(ecl, stage, portionId);
        } else {
            THPatch(ecl, (th_sections_t)section);
        }
    }

    // Hook Helper
    bool THBGMTest()
    {
        if (!thPracParam.mode)
            return 0;
        else if (thPracParam.section >= 10000)
            return 0;
        else
            return th_sections_bgm[thPracParam.section];
    }
    void THSaveReplay(char* rep_name)
    {
        ReplaySaveParam(mb_to_utf16(rep_name, 932).c_str(), thPracParam.GetJson());
    }

    static float MInterpolation(float t, float a, float b)
    {
        if (t < 0.0f) {
            return a;
        } else if (t < 0.5) {
            float k = (b - a) * 2.0f;
            return k * t * t + a;
        } else if (t < 1.0f) {
            float k = (b - a) * 2.0f;
            t = t - 1.0f;
            return -k * t * t + b;
        }
        return b;
    }
    static void RenderRepMarker(ImDrawList* p)
    {
        if (g_adv_igi_options.th06_showRepMarker) {
            DWORD is_rep = *(DWORD*)(0x69BCBC);
            DWORD gameState = *(DWORD*)(0x6C6EA4);
            if (is_rep && gameState == 2) {
                auto f = ImGui::GetFont();
                auto sz = f->CalcTextSizeA(20, 100, 100, "ＲＥＰ");
                ImVec2 p1 = { 416.0f, 464.0f };
                p->AddRectFilled({ p1.x - sz.x, p1.y - sz.y }, p1, 0x77000000);
                p->AddText(f, 20, { p1.x - sz.x, p1.y - sz.y }, 0xFFFFFFFF, "ＲＥＰ");
            }
        }
    }
    static void RenderPlHitbox(ImDrawList* p)
    {
        { // player hitbox
            static float t = 0.0f;
            static bool is_shift_pressed = false;
            if (g_adv_igi_options.th06_show_hitbox && g_hitbox_textureID != NULL) {
                bool is_time_stopped = *(BYTE*)(0x69BCCC);
                DWORD gameState = *(DWORD*)(0x6C6EA4);
                BYTE pauseMenuState = *(BYTE*)(0x69D4BF);
                WORD keyState = *(WORD*)(0x69D904);
                if (!is_time_stopped)
                    is_shift_pressed = keyState & 0x4;
                if (gameState == 2 && is_shift_pressed) {
                    if (pauseMenuState == 0) {
                        if (!is_time_stopped)
                            t += 1.0f;
                        float scale = MInterpolation(t / 18.0f, 1.5f, 1.0f),
                              scale2 = MInterpolation(t / 12.0f, 0.3f, 1.0f),
                              angle = 3.14159f,
                              angle2 = 0.0f,
                              alpha = t < 6.0f ? t / 6.0f : 1.0f;
                        if (t < 18.0f) {
                            angle = MInterpolation(t / 18.0f, 3.14159f, -3.14159f);
                            angle2 = -angle;
                        } else {
                            angle = -3.14159f + t * 0.05235988f;
                            angle2 = 3.14159f - t * 0.05235988f;
                        }
                        scale *= 0.75f;
                        scale2 *= 0.75f; // 32->24
                        p->PushClipRect({ 32.0f, 16.0f }, { 416.0f, 464.0f });
                        ImVec2 p1 = { *(float*)(0x6CAA68) + 32.0f, *(float*)(0x6CAA6C) + 16.0f };
                        float c, s;
                        c = cosf(angle) * scale * g_hitbox_sz.x, s = sinf(angle) * scale * g_hitbox_sz.y;
                        p->AddImageQuad(g_hitbox_textureID, { p1.x + c, p1.y + s }, { p1.x - s, p1.y + c }, { p1.x - c, p1.y - s }, { p1.x + s, p1.y - c }, { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, ImGui::ColorConvertFloat4ToU32({ 1, 1, 1, alpha }));
                        c = cosf(angle2) * scale2 * g_hitbox_sz.x, s = sinf(angle2) * scale2 * g_hitbox_sz.y;
                        p->AddImageQuad(g_hitbox_textureID, { p1.x + c, p1.y + s }, { p1.x - s, p1.y + c }, { p1.x - c, p1.y - s }, { p1.x + s, p1.y - c });
                        p->PopClipRect();
                    }
                } else {
                    t = 0.0f;
                }
            }
        }
    }
    static void RenderBtHitbox(ImDrawList* p)
    {
        // show bullet hitbox
        if (g_show_bullet_hitbox) {
            DWORD is_practice = *(DWORD*)(0x69d4c3);
            DWORD gameState = *(DWORD*)(0x6C6EA4);
            DWORD is_rep = *(DWORD*)(0x69BCBC);
            if (((is_practice) || (is_rep) || (thPracParam.mode)) && gameState == 2) {
                p->PushClipRect({ 32.0f, 16.0f }, { 416.0f, 464.0f });
                ImVec2 stage_pos = { 32.0f, 16.0f };
                ImVec2 plpos1 = *(ImVec2*)(0x6CAA80);
                ImVec2 plpos2 = *(ImVec2*)(0x6CAA8C);
                float plhit = plpos2.x - plpos1.x;

                // bullet hitbox
                for (int i = 0; i < 640; i++) {
                    DWORD pbt = 0x005AB5F8 + i * 0x5C4;
                    if ((*(WORD*)(pbt + 0x5BE) != 1) || (*(WORD*)(pbt + 0x5BE) == 0)) {
                        continue;
                    }
                    ImVec2 pos = *(ImVec2*)(pbt + 0x560);
                    ImVec2 hit = *(ImVec2*)(pbt + 0x550);

                    ImVec2 p1 = { pos.x - hit.x * 0.5f - plhit * 0.5f + stage_pos.x, pos.y - hit.y * 0.5f - plhit * 0.5f + stage_pos.y };
                    ImVec2 p2 = { pos.x + hit.x * 0.5f + plhit * 0.5f + stage_pos.x, pos.y + hit.y * 0.5f + plhit * 0.5f + stage_pos.y };
                    p->AddRectFilled(p1, p2, 0x88002288);
                    p->AddRect(p1, p2, 0xFFFFFFFF, 0.0f);
                }

                // laser hitbox
                for (int i = 0; i < 64; i++) {
                    DWORD pls = 0x00691FF8 + 0x270 * i;
                    DWORD is_used = *(DWORD*)(pls + 0x258);
                    if (is_used) {
                        ImVec2 pos = *(ImVec2*)(pls + 0x220);
                        float angle = *(float*)(pls + 0x22C);
                        float quat_width = *(float*)(pls + 0x23C) * 0.5f * 0.5f;
                        float half_width_pl = plhit * 0.5f;
                        float start_ofs = *(float*)(pls + 0x230);
                        float end_ofs = *(float*)(pls + 0x234);
                        int state = *(DWORD*)(pls + 0x26C);

                        int start_time_graze = *(DWORD*)(pls + 0x248);
                        int end_time_graze = *(DWORD*)(pls + 0x254);
                        int time_cur_state = *(DWORD*)(pls + 0x264);
                        float sub_frame = *(float*)(pls + 0x260);
                        if (state == 0) {
                            int state_change_time_hit = *(DWORD*)(pls + 0x244);
                            float l2 = 0.0f;
                            if (time_cur_state <= state_change_time_hit - std::max(30, state_change_time_hit)) {
                                l2 = 1.2f * 0.5f;
                            } else {
                                l2 = quat_width * ((float)time_cur_state + sub_frame) / (float)state_change_time_hit;
                            }
                            float mid = (start_ofs + end_ofs) * 0.5f;
                            start_ofs = mid - l2;
                            end_ofs = mid + l2;
                        }
                        if (state == 2) {
                            int state_change_time_disappear = *(DWORD*)(pls + 0x250);
                            float l2 = 0.0f;
                            if (state_change_time_disappear > 0) {
                                l2 = quat_width - quat_width * ((float)time_cur_state + sub_frame) / (float)state_change_time_disappear;
                            }
                            float mid = (start_ofs + end_ofs) * 0.5f;
                            start_ofs = mid - l2;
                            end_ofs = mid + l2;
                        }
                        if (state == 1 || (state == 0 && time_cur_state >= start_time_graze) || (state == 2 && time_cur_state < end_time_graze)) {
                            float c = cosf(angle);
                            float s = sinf(angle);
                            ImVec2 hitpos[4] = {
                                { start_ofs - half_width_pl, -quat_width - half_width_pl },
                                { end_ofs + half_width_pl, -quat_width - half_width_pl },
                                { end_ofs + half_width_pl, quat_width + half_width_pl },
                                { start_ofs - half_width_pl, quat_width + half_width_pl }
                            };
                            auto RotPos = [](ImVec2 p, float c, float s) -> ImVec2 {
                                return { p.x * c - p.y * s, p.x * s + p.y * c };
                            };
                            for (int j = 0; j < 4; j++) {
                                hitpos[j] = RotPos(hitpos[j], c, s);
                                hitpos[j].x += pos.x + stage_pos.x;
                                hitpos[j].y += pos.y + stage_pos.y;
                            }
                            p->AddQuad(hitpos[0], hitpos[1], hitpos[2], hitpos[3], 0xFFFF0000);
                            p->AddQuadFilled(hitpos[0], hitpos[1], hitpos[2], hitpos[3], 0x88882200);
                        }
                    }
                }

                // enemy hitbox
                for (int i = 0; i < 0x100; i++) {
                    DWORD penm = 0x4B79C8 + 0xED0 + i * 0xEC8;
                    if (*(char*)(0xE50 + penm) < 0) {
                        if ((*(BYTE*)(penm + 0xE51) & 4) != 0 && (*(BYTE*)(penm + 0xE51) & 2) != 0 && (*(BYTE*)(penm + 0xE51) & 1) != 0) {
                            ImVec2 pos = *(ImVec2*)(penm + 0xC6C);
                            ImVec2 hit = *(ImVec2*)(penm + 0xC78);
                            hit.x *= 0.666666f;
                            hit.y *= 0.666666f;
                            ImVec2 p1 = { pos.x - hit.x * 0.5f - plhit * 0.5f + stage_pos.x, pos.y - hit.y * 0.5f - plhit * 0.5f + stage_pos.y };
                            ImVec2 p2 = { pos.x + hit.x * 0.5f + plhit * 0.5f + stage_pos.x, pos.y + hit.y * 0.5f + plhit * 0.5f + stage_pos.y };
                            p->AddRectFilled(p1, p2, 0xAAFF5500);
                            p->AddRect(p1, p2, 0xFFFFFFFF);
                        }
                    }
                }
                 p->PopClipRect();
            }
        }
    }
    static void RenderLockTimer(ImDrawList* p)
    {
        if (*THOverlay::singleton().mTimeLock && g_lock_timer > 0) {
            std::string time_text = std::format("{:.2f}", (float)g_lock_timer / 60.0f);
            auto sz = ImGui::CalcTextSize(time_text.c_str());
            p->AddRectFilled({ 32.0f, 0.0f }, { 110.0f, sz.y }, 0xFFFFFFFF);
            p->AddText({ 110.0f - sz.x, 0.0f }, 0xFF000000, time_text.c_str());
        }
    }

    EHOOK_ST(th06_result_screen_create, 0x42d812, 4, {
        self->Disable();
        *(uint32_t*)(*(uint32_t*)(pCtx->Ebp - 0x10) + 0x8) = 0xA;
        pCtx->Eip = 0x42d839;
    });

    // It would be good practice to run Setup() on this
    // But due to the way this new hooking system works
    // running Setup is only needed for Hooks, not patches
    PATCH_ST(th06_white_screen, 0x42fee0, "c3");
    HOOKSET_DEFINE(THMainHook)
    PATCH_DY(th06_reacquire_input, 0x41dc58, "0000000074")
    EHOOK_DY(th06_activateapp, 0x420D96, 3, {
        // Wacky hack to disable rendering for one frame to prevent the game from crasing when alt tabbing into it if the pause menu is open and the game is in fullscreen mode
        GameGuiProgress = 1;
    })
    EHOOK_DY(th06_bgm_play, 0x424b5d, 1, {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];

        if (THPauseMenu::singleton().el_bgm_signal) {
            pCtx->Eip = 0x424d35;
        }
        if (retn_addr == 0x418db4) {
            THPauseMenu::singleton().el_bgm_changed = true;
        }
    })
    EHOOK_DY(th06_bgm_stop, 0x430f80, 1, {
        if (THPauseMenu::singleton().el_bgm_signal) {
            pCtx->Eip = 0x43107b;
        }
    })
    EHOOK_DY(th06_prac_menu_1, 0x437179, 7, {
        THGuiPrac::singleton().State(1);
    })
    EHOOK_DY(th06_prac_menu_3, 0x43738c, 3, {
        THGuiPrac::singleton().State(3);
    })
    EHOOK_DY(th06_prac_menu_4, 0x43723f, 3, {
        THGuiPrac::singleton().State(4);
    })
    EHOOK_DY(th06_prac_menu_enter, 0x4373a3, 5, {
        *(int32_t*)(0x69d6d4) = *(int32_t*)(0x69d6d8) = thPracParam.stage;
        if (thPracParam.stage == 6)
            *(int8_t*)(0x69bcb0) = 4;
        else
            *(int8_t*)(0x69bcb0) = *(int8_t*)(0x6c6e49);
    })
    EHOOK_DY(th06_pause_menu, 0x401b8f, 2, {
        if (thPracParam.mode && (*((int32_t*)0x69bcbc) == 0)) {
            auto sig = THPauseMenu::singleton().PMState();
            if (sig == THPauseMenu::SIGNAL_RESUME) {
                pCtx->Eip = 0x40223d;
            } else if (sig == THPauseMenu::SIGNAL_EXIT) {
                *(uint32_t*)0x6c6ea4 = 7; // Set gamemode to result screen
                *(uint16_t*)0x69d4bf = 0; // Close pause menu
                th06_result_screen_create.Enable();
            } else if (sig == THPauseMenu::SIGNAL_RESTART) {
                pCtx->Eip = 0x40263c;
            } else if (sig == THPauseMenu::SIGNAL_EXIT2) {
                *(uint32_t*)0x6c6ea4 = 1; 
                *(uint16_t*)0x69d4bf = 0;
                th06_result_screen_create.Enable();
            } else {
                pCtx->Eip = 0x4026a6;
            }
        }
        // escR patch
        DWORD thiz = pCtx->Ecx;
        if (!thPracParam.mode && (*((int32_t*)0x69bcbc) == 0))
        {
            if (*(DWORD*)(thiz) != 7) {
                WORD key = *(WORD*)(0x69D904);
                WORD key_last = *(WORD*)(0x69D908);
                if (((key & (292)) == 292 && (key & (292)) != (key_last & (292))) || (GetAsyncKeyState('R')&0x8000) ){ // ctrl+shift+down or R
                    *(DWORD*)(thiz) = 7;
                    threstartflag_normalgame = true;
                }
            }
        }
        if (*(DWORD*)(thiz) == 7) {
            pCtx->Eip = 0x40263c;
        }
    })
    
    EHOOK_DY(th06_inf_lives,0x00428DEB,2,{
        if ((*(THOverlay::singleton().mInfLives)))
        {
            if (!g_adv_igi_options.map_inf_life_to_no_continue){
                pCtx->Eax += 1;
            }else{
                if ((pCtx->Eax & 0xFF) == 0)
                    pCtx->Eax += 1;
            }
        }  
    })

    EHOOK_DY(th06_pause_menu_pauseBGM, 0x402714,3,{
        if (g_pauseBGM_06) {
            DWORD soundstruct = *(DWORD*)(0x6D457C);
            if (soundstruct)
            {
                int32_t n = *(int32_t*)(soundstruct + 0x10);
                IDirectSound8* d;
                IDirectSoundBuffer** soundbuffers = *(IDirectSoundBuffer***)(soundstruct + 0x4);
                if (*(BYTE*)(0x69D4BF) == 0 || (*(THOverlay::singleton().mElBgm) && thPracParam.mode)) // show menu==0
                {
                    for (int i = 0; i < n; i++) {
                        DWORD st = 0;
                        soundbuffers[i]->GetStatus(&st);
                        if (!(st & DSBSTATUS_PLAYING)) {
                            soundbuffers[i]->Play(0, 0, DSBPLAY_LOOPING);
                        }
                    }
                } else {
                    for (int i = 0; i < n; i++) {
                        DWORD st = 0;
                        soundbuffers[i]->GetStatus(&st);
                        if (st & DSBSTATUS_PLAYING) {
                            soundbuffers[i]->Stop();
                        }
                    }
                }
            }
        }
    })

    EHOOK_DY(th06_patch_main, 0x41c17a,5, {
        THPauseMenu::singleton().el_bgm_changed = false;
        if (thPracParam.mode == 1) {
            // TODO: Probably remove this ASM comment?
            /*
                    mov eax,dword ptr [@MENU_RANK]
                    mov dword ptr [69d710],eax
                    cmp dword ptr [@MENU_RANKLOCK],@MENU_ON_STR
                    jnz @f
                    mov dword ptr [69d714],eax
                    mov dword ptr [69d718],eax
                */
            *(int8_t*)(0x69d4ba) = (int8_t)thPracParam.life;
            *(int8_t*)(0x69d4bb) = (int8_t)thPracParam.bomb;
            *(int16_t*)(0x69d4b0) = (int16_t)thPracParam.power;
            *(int32_t*)(0x69bca0) = *(int32_t*)(0x69bca4) = (int32_t)thPracParam.score;
            *(int32_t*)(0x69bcb4) = *(int32_t*)(0x69bcb8) = (int32_t)thPracParam.graze;
            *(int16_t*)(0x69d4b4) = *(int16_t*)(0x69d4b6) = (int16_t)thPracParam.point;
            *(uint32_t*)0x5a5fb0 = thPracParam.frame;

            if (*(int8_t*)(0x69bcb0) != 4) {
                if (thPracParam.score >= 60000000)
                    *(int8_t*)(0x69d4bc) = 4;
                else if (thPracParam.score >= 40000000)
                    *(int8_t*)(0x69d4bc) = 3;
                else if (thPracParam.score >= 20000000)
                    *(int8_t*)(0x69d4bc) = 2;
                else if (thPracParam.score >= 10000000)
                    *(int8_t*)(0x69d4bc) = 1;
            }

            *(int32_t*)(0x69d710)  = (int32_t)thPracParam.rank;
            if (thPracParam.rankLock) {
                *(int32_t*)(0x69d714)  = (int32_t)thPracParam.rank;
                *(int32_t*)(0x69d718)  = (int32_t)thPracParam.rank;
            }

            THSectionPatch();
        }
        thPracParam._playLock = true;

        if (THPauseMenu::singleton().el_bgm_signal) {
            THPauseMenu::singleton().el_bgm_signal = false;
            pCtx->Eip = 0x41c18a;
        } else if (THBGMTest()) {
            pCtx->Eax += 0x310;
            pCtx->Eip = 0x41c17f;
        }
    })
    EHOOK_DY(th06_restart, 0x435901, 5, {
        if (!threstartflag_normalgame && !thRestartFlag) {
            th06_white_screen.Disable();
        }
        if (threstartflag_normalgame)
        {
            th06_white_screen.Enable();
            threstartflag_normalgame = false;
            pCtx->Eip = 0x436DCB;
        }
        if (thRestartFlag) {
            th06_white_screen.Enable();
            thRestartFlag = false;
            pCtx->Eip = 0x43738c;
        } else {
            thPracParam.Reset();
        }
    })
    EHOOK_DY(th06_title, 0x41ae2c, 7, {
        if (thPracParam.mode != 0 && thPracParam.section) {
            pCtx->Eip = 0x41af35;
        }
    })
    PATCH_DY(th06_preplay_1, 0x42d835, "09")
    EHOOK_DY(th06_preplay_2, 0x418ef9, 5, {
        if (thPracParam.mode && !THGuiRep::singleton().mRepStatus) {
            *(uint32_t*)0x69bca0 = *(uint32_t*)0x69bca4;
            pCtx->Eip = 0x418f0e;
        }
    })
    EHOOK_DY(th06_save_replay, 0x42b03b, 3, {
        char* rep_name = *(char**)(pCtx->Ebp + 0x8);
        if (thPracParam.mode)
            THSaveReplay(rep_name);
    })
    EHOOK_DY(th06_rep_menu_1, 0x438262, 6, {
        THGuiRep::singleton().State(1);
    })
    EHOOK_DY(th06_rep_menu_2, 0x4385d5, 6, {
        THGuiRep::singleton().State(2);
    })
    EHOOK_DY(th06_rep_menu_3, 0x438974, 10, {
        THGuiRep::singleton().State(3);
    })
    EHOOK_DY(th06_fake_shot_type, 0x40b2f9, 6, {
        if (thPracParam.fakeType) {
            *((int32_t*)0x487e44) = thPracParam.fakeType - 1;
            pCtx->Eip = 0x40b2ff;
        }
    })
    EHOOK_DY(th06_patchouli, 0x40c100, 1, {
        int32_t* var = *(int32_t**)(pCtx->Esp + 4);
        if (thPracParam.fakeType) {
            var[618] = ((int32_t*)0x476264)[3 * (thPracParam.fakeType - 1)];
            var[619] = ((int32_t*)0x476268)[3 * (thPracParam.fakeType - 1)];
            var[620] = ((int32_t*)0x47626c)[3 * (thPracParam.fakeType - 1)];
            pCtx->Eip = 0x40c174;
        }
    })
    EHOOK_DY(th06_cancel_muteki, 0x429ec4, 7, {
        if (thPracParam.mode) {
            *(uint8_t*)(pCtx->Eax + 0x9e0) = 0;
            pCtx->Eip = 0x429ecb;
        }
    })
    EHOOK_DY(th06_set_deathbomb_timer, 0x42a09c, 10, {
        if (thPracParam.mode) {
            *(uint32_t*)(pCtx->Eax + 0x9d8) = 6;
            pCtx->Eip = 0x42a0a6;
        }
    })
    EHOOK_DY(th06_hamon_rage, 0x40e1c7, 10, {
        if (thPracParam.mode && thPracParam.stage == 6 && thPracParam.section == TH06_ST7_END_S10 && thPracParam.phase == 1) {
            pCtx->Eip = 0x40e1d8;
        }
    })
    EHOOK_DY(th06_wall_prac_boss_pos, 0x40907F,3,{
        if (thPracParam.mode && thPracParam.stage == 5 && thPracParam.wall_prac_st6 && thPracParam.section == TH06_ST6_BOSS9) {
            DWORD penm = *(DWORD*)(pCtx->Ebp + 0x8);
            g_last_boss_x = *(float*)(penm + 0xC6C);
            g_last_boss_y = *(float*)(penm + 0xC70);
        }
    })
    EHOOK_DY(th06_wall_prac, 0x40D57C,7,{
        if (thPracParam.mode && thPracParam.stage == 5  && thPracParam.wall_prac_st6) {
            auto GetRandF = []() -> float {
                unsigned int(__fastcall * sb_41E7F0_rand_int)(DWORD thiz);
                sb_41E7F0_rand_int = (decltype(sb_41E7F0_rand_int))0x41E7F0;
                unsigned int randi = sb_41E7F0_rand_int(0x69D8F8);
                return (double)randi / 4294967296.0;
            }; // rand from 0 to 1
            if (thPracParam.section == TH06_ST6_BOSS6)
            {
                float posb1 = 0.4;
                float posb2 = 0.9;
                float posb3 = 0.2;
               
                float* wall_angle = (float*)(pCtx->Ebp - 0x68);
                DWORD penm = *(DWORD*)(pCtx->Ebp + 0x8);
                float bossx = *(float*)(penm + 0xC6C);
                float bossy = *(float*)(penm + 0xC70);
                float plx = *(float*)(0x6CAA68);
                float ply = *(float*)(0x6CAA6C);
                float angle_pl = atan2f(ply - bossy, plx - bossx);
                float dist_pl = hypotf(ply - bossy, plx - bossx);
                
                float decision = GetRandF();
                
                // - 1.570796f + GetRandF() * 1.745329f
                if (decision < posb1) {
                    float min_dist_bt = 99999.0f;
                    for (int i = 0; i < 640; i++) {
                        DWORD pbt = 0x005AB5F8 + i * 0x5C4;
                        if (*(WORD*)(pbt + 0x5BE)
                            && *(WORD*)(pbt + 0x5BE) != 5
                            && *(DWORD*)(pbt + 0xC0)
                            && *(float*)(*(DWORD*)(pbt + 0xC0) + 0x2C) < 30.0
                            && *(float*)(pbt + 0x584) == 0.0)
                        {
                            ImVec2 pos = *(ImVec2*)(pbt + 0x560);
                            min_dist_bt = std::min(min_dist_bt, hypotf(pos.x - bossx, pos.y - bossy));
                        }
                    }
                    if (decision < posb3)
                        *wall_angle = angle_pl - min_dist_bt * 3.14159f / 256.0f - 0.5235988f + GetRandF() * 0.5235988f; // -30 deg ~ 0deg
                    else
                        *wall_angle = angle_pl - min_dist_bt * 3.14159f / 256.0f - 1.570796f + GetRandF() * 1.745329f; // -90 deg ~ 10deg
                } else if (decision < posb2) {
                    // angle = randA + dist*pi/256 = pi
                    // => randA = pi - dist*pi/256
                    *wall_angle = 3.14159f - dist_pl * 3.14159f / 256.0f - 0.2617f + GetRandF() * 0.5235988f; //
                }else {
                    *wall_angle = GetRandF() * 6.28318f - 3.1415926f;
                }
            } else if (thPracParam.section == TH06_ST6_BOSS9) {
                float* wall_angle = (float*)(pCtx->Ebp - 0x68);
                DWORD penm = *(DWORD*)(pCtx->Ebp + 0x8);
                float bossx = *(float*)(penm + 0xC6C);
                float bossy = *(float*)(penm + 0xC70);
                float plx = *(float*)(0x6CAA68);
                float ply = *(float*)(0x6CAA6C);
                float angle_pl = atan2f(ply - bossy, plx - bossx);
                float dist_pl = hypotf(ply - bossy, plx - bossx);

                float decision = GetRandF();
                if (decision < 0.8) {
                    float min_dist_bt = 99999.0f;
                    ImVec2 bt_pos = { 0.0f, 0.0f };
                    for (int i = 0; i < 640; i++) {
                        DWORD pbt = 0x005AB5F8 + i * 0x5C4;
                        if (*(WORD*)(pbt + 0x5BE)
                            && *(WORD*)(pbt + 0x5BE) != 5
                            && *(DWORD*)(pbt + 0xC0)
                            && *(float*)(*(DWORD*)(pbt + 0xC0) + 0x2C) < 30.0
                            && *(float*)(pbt + 0x584) == 0.0) {
                            ImVec2 pos = *(ImVec2*)(pbt + 0x560);
                            auto dist = hypotf(pos.x - g_last_boss_x, pos.y - g_last_boss_y);
                            if (min_dist_bt > dist) {
                                min_dist_bt = dist;
                                bt_pos = pos;
                            }
                        }
                    }
                    *wall_angle = angle_pl - hypotf(bt_pos.x - bossx, bt_pos.y - bossy) * 3.14159f / 256.0f + (GetRandF() - 0.5f) * 2.0f * 0.34f;
                }else {
                    *wall_angle = GetRandF() * 6.28318f - 3.1415926f;
                }
            }
        }
    })
    EHOOK_DY(th06_wall_prac2, 0x0040D900,6,{
        if (thPracParam.mode && thPracParam.stage == 5 && thPracParam.wall_prac_st6 && thPracParam.section == TH06_ST6_BOSS9 
            && (thPracParam.snipeF > 0 || thPracParam.snipeN > 0)
            ) {
                float* angle = (float*)(pCtx->Ebp - 0x70);
                DWORD pbt = *(DWORD*)(pCtx->Ebp - 0x60);
                ImVec2 pos = *(ImVec2*)(pbt + 0x560);
                float plx = *(float*)(0x6CAA68);
                float ply = *(float*)(0x6CAA6C);
                float dist_pl = hypotf(plx - pos.x, ply - pos.y);
                float angle_pl = atan2f(ply - pos.y, plx - pos.x);
                float random_near = 1.0f - thPracParam.snipeN/100.0f;
                float random_far = 1.0f - thPracParam.snipeF / 100.0f;
                if (dist_pl > 400.0f) {
                    dist_pl = 400.0f;
                }
                *angle = *angle * (dist_pl * (random_far - random_near) / 400.0f + random_near) + angle_pl;

        }
    })
    PATCH_DY(th06_disable_menu, 0x439ab2, "9090909090")
    // fix igi render problem
    PATCH_DY(th06_background_fix_1, 0x42073B, "909090909090")
    PATCH_DY(th06_background_fix_2, 0x419F4B, "909090909090")
    PATCH_DY(th06_background_fix_3, 0x419F81, "909090909090")
    
    
    EHOOK_DY(th06_books_position_test, 0x0041188A,3,{
        if (*(DWORD*)0x69d6d4 == 4){
            DWORD pCode = *(DWORD*)0x487e50;
            DWORD pCodeOfs = *(DWORD*)(pCtx->Ebp - 0x14) - 8;
            float posx = *(float*)(pCtx->Ebp - 0x20);
            float posy = *(float*)(pCtx->Ebp - 0x1C);
            int n = (pCodeOfs - pCode - 0xCDD4) / 0x1C;
            g_books_pos[n] = { posx, posy };
        }
    })

    EHOOK_DY(th06_update, 0x41caac, 1, {
        GameGuiBegin(IMPL_WIN32_DX8, !THAdvOptWnd::singleton().IsOpen());
        
        // Gui components update
        Gui::KeyboardInputUpdate(VK_ESCAPE);
        THPauseMenu::singleton().Update();
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
        TH06InGameInfo::singleton().Update();
        TH06InGameInfo::singleton().IncreaseGameTime();

        auto p = ImGui::GetOverlayDrawList();
        RenderPlHitbox(ImGui::GetBackgroundDrawList());
        if (*(DWORD*)(0x6C6EA4) == 2) {
            RenderBlindView(8, *(DWORD*)0x6c6d20, *(ImVec2*)(0x6CAA68), { 0.0f, 0.0f }, { 32.0f, 16.0f }, 1.0f);
        }
        RenderRepMarker(p);
        RenderBtHitbox(p);
        RenderLockTimer(p);
        if (g_adv_igi_options.show_keyboard_monitor && (*(DWORD*)(0x6C6EA4) == 2)) {
            g_adv_igi_options.keyboard_style.size = { 48.0f, 48.0f };
            KeysHUD(6, { 1280.0f, 0.0f }, { 833.0f, 0.0f }, g_adv_igi_options.keyboard_style);
        }
        {
            if (THAdvOptWnd::singleton().forceBossMoveDown) {
                auto sz = ImGui::CalcTextSize(S(TH_BOSS_FORCE_MOVE_DOWN));
                p->AddRectFilled({ 120.0f, 0.0f }, { sz.x + 120.0f, sz.y }, 0xFFCCCCCC);
                p->AddText({ 120.0f, 0.0f }, 0xFFFF0000, S(TH_BOSS_FORCE_MOVE_DOWN));
            }
        }
        
        GameGuiEnd(THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen() || THPauseMenu::singleton().IsOpen());
    })
    EHOOK_DY(th06_render, 0x41cb6d, 1, {
        GameGuiRender(IMPL_WIN32_DX8);
        if (Gui::KeyboardInputUpdate(VK_HOME) == 1)
            THSnapshot::Snapshot(*(IDirect3DDevice8**)0x6c6d20);
    })
    EHOOK_DY(th06_stage_color_fix, 0x4039E5,3,
    {
        if (g_adv_igi_options.th06_bg_fix) {
            pCtx->Edx = 0x00000000;
        }
    })
    EHOOK_DY(th06_player_state, 0x4288C0,1,
    {
        if (g_adv_igi_options.show_keyboard_monitor)
            RecordKey(6, *(WORD*)(0x69D904));
    })
    EHOOK_DY(th06_rep_seed,0x42A97E,10,{
       g_last_rep_seed = *(uint16_t*)(0x0069D8F8);
    })
    EHOOK_DY(th06_fix_seed, 0x41BE47,7,{
        if (g_adv_igi_options.th06_fix_seed)
        {
            if ((*(DWORD*)(0x69d6d4) == 1 || *(DWORD*)(0x69d6d4) == 7)) {
                if ((!thPracParam.mode) || thPracParam.section == 10000 + 1 * 100 + 1 || thPracParam.section == 0)
                {
                    *(uint16_t*)(0x69D8F8) = (uint16_t)((int32_t)g_adv_igi_options.th06_seed & 0xFFFF);
                }
            }
        }
    })
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInGameInfo)
    EHOOK_DY(th06_enter_game, 0x41BDE8,4, // set inner misscount to 0
    {
        TH06InGameInfo::singleton().mMissCount = 0;
        TH06InGameInfo::singleton().Retry();
    })
    EHOOK_DY(miss_spellcard_get_failed, 0x4277C3,3,
    {
        is_died = true;
    })
    EHOOK_DY(th06_miss, 0x428DD9,2,// dec life
    {
        TH06InGameInfo::singleton().mMissCount++;
    })

    EHOOK_DY(th06_lock_timer1, 0x41B27C,3, // initialize
    {
        g_lock_timer = 0;
    })
    EHOOK_DY(th06_lock_timer2, 0x409A10,6, // set timeout case 115
    {
        g_lock_timer = 0;
    })
    EHOOK_DY(th06_lock_timer3, 0x408DDA,2, // set boss mode case 101
    {
        g_lock_timer = 0;
    })
    EHOOK_DY(th06_lock_timer4, 0x411F88,6, // decrease time (update)
    {
        g_lock_timer++;
    })

    EHOOK_DY(th06_autoName_score,0x42BE49,5,{
        if (g_adv_igi_options.th06_autoname){
            int size = strlen(g_adv_igi_options.th06_autoname_name);
            if (size > 8)
                size = 8;
            if (size >= 1){
                *(DWORD*)(pCtx->Eax + 0x10) = size - 1;
                for (int i = 0; i < size; i++) {
                    *(char*)(pCtx->Eax + 0x5193 + i) = (g_adv_igi_options.th06_autoname_name[i] == '~') ? '\xA5' : g_adv_igi_options.th06_autoname_name[i]; // red slash = 0xA5
                }
            }
        }
    })
    EHOOK_DY(th06_autoName_rep_overwrite, 0x42D085,7,
    {
        if (g_adv_igi_options.th06_autoname) {
            bool set_name = true;
            int name_len = 0;
            for (int i = 0; i < 8; i++) {
                char ch = *(char*)(pCtx->Eax + 0x34 + i);
                if (ch != ' ' && ch!=0) {
                    set_name = false;
                    name_len = i;
                }
            }
            if (set_name) {
                int size = strlen(g_adv_igi_options.th06_autoname_name);
                if (size > 8)
                    size = 8;
                if (size >= 1) {
                    *(DWORD*)(pCtx->Eax + 0x10) = size - 1;
                    for (int i = 0; i < 8; i++) {
                        if (i < size)
                            *(char*)(pCtx->Eax + 0x34 + i) = (g_adv_igi_options.th06_autoname_name[i] == '~') ? '\xA5' : g_adv_igi_options.th06_autoname_name[i]; // red slash = 0xA5
                        else
                            *(char*)(pCtx->Eax + 0x34 + i) = ' ';
                    }
                }
            }else{
                *(DWORD*)(pCtx->Eax + 0x10) = name_len;
            }
        }
    })
    EHOOK_DY(th06_autoName_rep, 0x42C8A0,2,
    {
        if (g_adv_igi_options.th06_autoname) {
            bool set_name = true;
            int name_len = 0;
            for (int i = 0; i < 8; i++){
                char ch = *(char*)(pCtx->Eax + 0x34 + i);
                if (ch != ' ' && ch != 0) {
                    set_name = false;
                    name_len = i;
                }
            }
            if (set_name){
                int size = strlen(g_adv_igi_options.th06_autoname_name);
                if (size > 8)
                    size = 8;
                if (size >= 1) {
                    *(DWORD*)(pCtx->Eax + 0x10) = size - 1;
                    for (int i = 0; i < 8; i++) {
                        if (i < size)
                            *(char*)(pCtx->Eax + 0x34 + i) = (g_adv_igi_options.th06_autoname_name[i] == '~') ? '\xA5' : g_adv_igi_options.th06_autoname_name[i]; // red slash = 0xA5
                        else
                            *(char*)(pCtx->Eax + 0x34 + i) = ' ';
                    }
                    pCtx->Eip = 0x42C91C;
                }
            }else{
                *(DWORD*)(pCtx->Eax + 0x10) = name_len;
                pCtx->Eip = 0x42C91C;
            }
        }
       
    })
    HOOKSET_ENDDEF()

     static __declspec(noinline) void THGuiCreate()
    {
         if (ImGui::GetCurrentContext()) {
             return;
         }

        // Init
        GameGuiInit(IMPL_WIN32_DX8, 0x6c6d20, 0x6c6bd4,
            Gui::INGAGME_INPUT_GEN1, 0x69d904, 0x69d908, 0x69d90c,
            -1);
         SetDpadHook(0x41D330, 3);
        // g_adv_igi_options.th06_showHitbox
        g_hitbox_textureID = ReadImage(8, *(DWORD*)0x6c6d20, "hitbox.png", hitbox_file, sizeof(hitbox_file));
        D3DSURFACE_DESC desc;
        ((LPDIRECT3DTEXTURE8)g_hitbox_textureID)->GetLevelDesc(0, &desc);
        g_hitbox_sz.x = desc.Width, g_hitbox_sz.y = desc.Height;

        // Gui components creation
        THGuiPrac::singleton();
        THPauseMenu::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();
        TH06InGameInfo::singleton();
        TH06InGameInfo::singleton().Init();
        // Hooks
        EnableAllHooks(THMainHook);
        EnableAllHooks(THInGameInfo);

        th06_white_screen.Setup();
        th06_result_screen_create.Setup();

        // Reset thPracParam
        thPracParam.Reset();
    }

    HOOKSET_DEFINE(THInitHook)
    EHOOK_DY(th06_gui_init_1, 0x43596f, 3, {
        THGuiCreate();
        self->Disable();
    })
    EHOOK_DY(th06_gui_init_2, 0x42140c, 1, {
        THGuiCreate();
        self->Disable();
    })
    EHOOK_DY(th06_close, 0x420669,2,{
        TH06InGameInfo::singleton().SaveAll();
    })
    HOOKSET_ENDDEF()
}
#include <d3d9types.h>
void TH06Init()
{
    EnableAllHooks(TH06::THInitHook);
    TryKeepUpRefreshRate((void*)0x420f59);
}

}
