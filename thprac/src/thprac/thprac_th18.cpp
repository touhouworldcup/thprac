#include "thprac_games.h"
#include "thprac_utils.h"
#include <metrohash128.h>
#include <format>

#include <d3d9.h>

namespace THPrac {
namespace TH18 {
#define play_sound_centered(id) asm_call<0x476BE0, Stdcall>(id, UNUSED_DWORD)
#pragma warning(push)
#pragma warning(disable : 4100)
     // MSVC WILL generate suboptimal code here if I don't hand-write my own assembly here
     __declspec(naked) void __fastcall globals_assign_hooked(uint32_t* glob1, void*, uint32_t* glob2)
    {
        __asm {
            push esi
            mov esi, [ecx+0x20]
            push [esp+0x8]
            mov eax, 0x417A60
            call eax
            mov [eax+0x20], esi
            pop esi
            ret 0x4
        }
    }
#pragma warning(pop)
    enum sound_id : uint32_t {
        SND_INVALID = 16,
    };

    static inline constexpr uint32_t COUNTERSTOP = 999999999;
    static inline constexpr int32_t STAGE_COUNT = 8;

    int g_lock_timer = 0;
    bool g_lock_timer_flag = false;

    enum addrs {
        REPLAY_MANAGER_PTR = 0x4cf418,
        CARD_PRICE_TABLE = 0x4b35c4,
        CARD_DESC_LIST = 0x4c53c0,
        MENU_INPUT = 0x4ca21c,
        MODEFLAGS = 0x4cccc8,
        STAGE_NUM = 0x4cccdc,
        NEXT_STAGE_NUM = 0x4ccce0,
        SCORE = 0x4cccfc,
        FUNDS = 0x4ccd34,
        POWER = 0x4ccd38,
        LIVES = 0x4ccd48,
        BOMBS = 0x4ccd58,
        ABILITY_MANAGER_PTR = 0x4cf298,
        ABILITY_SHOP_PTR = 0x4cf2a4,
        ASCII_MANAGER_PTR = 0x4cf2ac,
        TRANSITION_STG_PTR = 0x4cf2b0,
        BULLET_MANAGER_PTR = 0x4cf2bc,
        MUKADE_ADDR = 0x4cf2d4,
        GAME_THREAD_PTR = 0x4cf2e4,
        ITEM_MANAGER_PTR = 0x4cf2ec,
        PAUSE_MENU_PTR = 0x4cf40c,
        PLAYER_PTR = 0x4cf410,
        SCOREFILE_MANAGER_PTR = 0x4cf41c,
        MAIN_MENU_PTR = 0x4cf43c,
        ANM_MANAGER_PTR = 0x51f65c,
        WINDOW_PTR = 0x568c30,
    };

    enum funcs {
        SAVE_REPLAY = 0x461e90,
        SET_MENU = 0x4646e0,
    };
    
   enum cards {
        KOZUCHI = 42,
        KANAME,
        MOON,
        MIKOFLASH,
        VAMPIRE,
        SUN,
        LILY,
        BASSDRUM,
        PSYCO,
        CYLINDER = 52,
        RICEBALL,
        MUKADE
    };

 struct Thread {
       void* vtable; // 0x0
       void* thread; // 0x4
       uint32_t tid; // 0x8
       int32_t __bool_c; // 0xc
       int32_t __bool_10; // 0x10
       HINSTANCE* phModule; // 0x14
       char filler_24[0x4]; // 0x18
   };

    struct CardBase {
       struct VTableCard* vtable; // 0x0
       int32_t card_id; // 0x4
       int32_t array_index___plus_1_i_think; // 0x8
       ThList<CardBase> list_node; // 0xc
       int32_t anm_id_for_ingame_effect; // 0x1c
       Timer recharge_timer; // 0x20
       Timer _recharge_timer; // 0x34
       int32_t recharge_time; // 0x48
       struct TableCardData* table_entry; // 0x4c
       int32_t flags; // 0x50
    };

    struct CardLily : public CardBase {
        int32_t count; // 0x54
    };

    struct TableCardData {
        char* internal_name;
        uint32_t card_id;
        uint32_t in_trial;
        uint32_t category;
        uint32_t price;
        uint32_t weight;
        uint32_t appearance_condition;
        uint32_t allow_duplicates;
        uint32_t menu_equippable;
        uint32_t default_unlock;
        uint32_t __not_basic_resource;
        uint32_t sprite_large;
        uint32_t sprite_small;
    };

    struct AbilityManager {
        char filler_0[0x4]; // 0x0
        struct UpdateFunc* on_tick; // 0x4
        struct UpdateFunc* on_draw; // 0x8
        struct AnmLoaded* ability_anm; // 0xc
        struct AnmLoaded* abcard_anm; // 0x10
        struct AnmLoaded* abmenu_anm; // 0x14
        ThList<CardBase> card_list_head; // 0x18
        int32_t num_total_cards; // 0x28
        int32_t num_active_cards; // 0x2c
        int32_t num_equipment_cards; // 0x30
        int32_t num_passive_cards; // 0x34
        CardBase* selected_active_card; // 0x38
        int32_t __id_3c; // 0x3c
        char filler_64[0xc]; // 0x40 - 0x48
        int32_t __id_4c; // 0x4c
        char filler_80[0x4]; // 0x50
        int32_t flags; // 0x54
        int32_t __array_1[0x100]; // 0x58
        int32_t __array_2[0x100]; // 0x458
        int32_t __array_3[0x100]; // 0x858
        char filler_3160[0xc]; // 0xc58
        int32_t __created_ability_txt; // 0xc64
        struct Thread __thread; // 0xc68
        int32_t bought_flags[0x40]; // 0xc84
    };

    struct AnmSprite { // size 0x44
        char _pad0[0x8];
        int32_t __index_8; // 0x8
        char _pad1[0x38];
    };

    struct AnmImage { // size 0x18
        LPDIRECT3DTEXTURE9 d3d_texture; // 0x0
        char _pad0[0x14];
    };

    struct AnmLoaded { // size 0x13c
        char _pad0[0x11c];
        AnmSprite* sprites; // 0x11c
        char _pad1[0x4];
        AnmImage* images; // 0x124
        char _pad2[0x14];
    };

    struct AnmManager {
        char _pad0[0x312072c];
        AnmLoaded* loaded_anm_files[33]; // 0x312072c
    };

    struct ReplayStageData { // size 0x28
        struct ReplayFrameInput* input_start;
        struct ReplayFrameInput* input_current; // 0x4
        uint8_t* fps_counts_start; // 0x8
        uint8_t* fps_counts_current; // 0xC
        struct ReplayGameState* gamestate_start; // 0x10
        int32_t current_frame; // 0x14
        ThList<ReplayStageData> list_node; // 0x18
    };

    struct Replay { // size 0x31C
        char _pad0[0xc];
        uint32_t replay_mode; // 0xc
        int __dword_10; // 0x10
        struct ReplayHeader* header; // 0x14
        struct ReplayInfo* info; // 0x18
        struct ReplayGameState* game_states[STAGE_COUNT]; // 0x1c
        ThList<struct ReplayChunk> chunk_lists[STAGE_COUNT]; // 0x3c
        ThList<struct ReplayChunk>* current_chunk_node; // 0xbc
        int __chunk_count; // 0xc0
        struct ReplayStageData stage_data[STAGE_COUNT]; // 0xc4
        void* file_buffer; // 0x204
        uint8_t __byte_208; // 0x208
        char _pad1[0x3]; // 0x209
        int32_t __int_20C; // 0x20c
        struct UpdateFunc* on_tick_func_B; // 0x210
        int32_t stage_number; // 0x214
        uint32_t flags; // 0x218
        char file_path[0x100]; // 0x21c
    };

    using std::pair;
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
        int32_t power;
        int32_t funds;

        int32_t kozuchi;
        int32_t kaname;
        int32_t moon;
        int32_t mikoflash;
        int32_t vampire;
        int32_t sun;
        int32_t lily_count;
        int32_t lily_cycle;
        int32_t lily_cd;
        int32_t bassdrum;
        int32_t psyco;
        int32_t cylinder;
        int32_t riceball;
        int32_t mukade;

        bool dlg;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
            lily_cycle = -1; // to detect older replays not having it
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th18");
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
            GetJsonValue(power);
            GetJsonValue(funds);
            GetJsonValue(kozuchi);
            GetJsonValue(kaname);
            GetJsonValue(moon);
            GetJsonValue(mikoflash);
            GetJsonValue(vampire);
            GetJsonValue(sun);
            GetJsonValue(lily_count);
            GetJsonValue(lily_cycle);
            GetJsonValue(lily_cd);
            GetJsonValue(bassdrum);
            GetJsonValue(psyco);
            GetJsonValue(cylinder);
            GetJsonValue(riceball);
            GetJsonValue(mukade);

            return true;
        }
        std::string GetJson()
        {
            if (mode == 1) {
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th18", jalloc);
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
                AddJsonValue(power);
                AddJsonValue(funds);
                AddJsonValue(kozuchi);
                AddJsonValue(kaname);
                AddJsonValue(moon);
                AddJsonValue(mikoflash);
                AddJsonValue(vampire);
                AddJsonValue(sun);
                AddJsonValue(lily_count);
                AddJsonValue(lily_cycle);
                AddJsonValue(lily_cd);
                AddJsonValue(bassdrum);
                AddJsonValue(psyco);
                AddJsonValue(cylinder);
                AddJsonValue(riceball);
                AddJsonValue(mukade);

                ReturnJson();
            } else if (mode == 2) {
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th18", jalloc);
                AddJsonValue(mode);

                if (phase)
                    AddJsonValue(phase);

                ReturnJson();
            }

            CreateJson();
            jalloc; // Dummy usage to silence C4189
            ReturnJson();
        }
    };
    THPracParam thPracParam {};

    
    EHOOK_ST(th18_free_blank, 0x411f4b, 2, {
        pCtx->Eip = 0x411f52;
        self->Disable();
    });

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mMode = 1;
            *mLife = 7;
            *mBomb = 7;
            *mPower = 400;

            for (auto& card : mCardToggles) {
                if (card.first == MUKADE)
                    continue;

                auto& cardSlider = *(card.second.first);

                cardSlider.SetCurrentStep(100);
                *cardSlider = 10000;
            }

            mMukade.SetCurrentStep(100);
            *mMukade = 800;
            *mLilyCycle = 1; // match vanilla behavior for counter=10 by default

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            SetViewport((void*)0x4cd420);
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
                mDiffculty = *((int32_t*)0x4c9ab0);
                ToggleCards();

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
                thPracParam.power = *mPower;
                thPracParam.funds = *mFunds;

                thPracParam.kozuchi = 10000 - *mKozuchi;
                thPracParam.kaname = 10000 - *mKaname;
                thPracParam.moon = 10000 - *mMoon;
                thPracParam.mikoflash = 10000 - *mMikoflash;
                thPracParam.vampire = 10000 - *mVampire;
                thPracParam.sun = 10000 - *mSun;
                thPracParam.lily_count = *mLilyCount;
                thPracParam.lily_cycle = *mLilyCycle;
                thPracParam.lily_cd = 10000 - *mLilyCD;
                thPracParam.bassdrum = 10000 - *mBassdrum;
                thPracParam.psyco = 10000 - *mPsyco;
                thPracParam.cylinder = 10000 - *mCylinder;
                thPracParam.riceball = 10000 - *mRiceball;
                thPracParam.mukade = *mMukade * 20;

                break;
            case 4:
                Close();
                *mNavFocus = 0;
                break;
            default:
                break;
            }
        }

        void ToggleCards()
        {
            for (auto& pair : mCardToggles)
                pair.second.second = false;

            uint32_t* list = nullptr;
            for (uint32_t* i = (uint32_t*)GetMemContent(ABILITY_MANAGER_PTR, 0x1c); i; i = (uint32_t*)i[1]) {
                list = i;
                auto cardId = ((uint32_t**)list)[0][1];

                if (mCardToggles.find(cardId) != mCardToggles.end())
                    mCardToggles[cardId].second = true;
            }
        }

    protected:

        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_MENU));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSizeRel(0.5f, 0.7f);
                SetPosRel(0.27f, 0.18f);
                SetItemWidthRel(-0.100f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(0.6f, 0.7f);
                SetPosRel(0.215f, 0.18f);
                SetItemWidthRel(-0.100f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(0.56f, 0.7f);
                SetPosRel(0.230f, 0.18f);
                SetItemWidthRel(-0.105f);
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
            if (section == TH18_ST6_BOSS10) {
                return TH_SPELL_PHASE2;
            } else if (section == TH18_ST7_END_S6) {
                return TH_SPELL_PHASE1;
            } else if (section == TH18_ST7_END_S10) {
                return TH_SPELL_PHASE3;
            } else if (section == 10000 + 4 * 100 + 4) {
                return TH_PHASE_INF_MODE;
            } else if (section == 10000 + 3 * 100 + 6) {
                return TH_PHASE_INF_MODE;
            } else if (section == 10000 + 5 * 100 + 5) {
                return TH_PHASE_INF_MODE;
            } else if (section == 10000 + 5 * 100 + 8) {
                return TH_PHASE_INF_MODE;
            }
            return nullptr;
        }
        void PracticeMenu()
        {
            mMode();
            if (mStage())
                *mSection = *mChapter = 0;
            if (*mMode == 1) {
                if (mWarp())
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
                mFunds();

                mScore();
                mScore.RoundDown(10);

                if (mCardToggles[MUKADE].second) {
                    char str[8];
                    sprintf_s(str, "1.%03d", *mMukade);
                    mMukade(str);
                }

                for (auto& card : mCardToggles) {
                    auto& [slider, isActive] = card.second;

                    if (isActive) {
                        if (card.first == MUKADE)
                            continue;

                       if (card.first == LILY) {
                            mLilyCount();

                            if (*mLilyCount >= 10)
                                mLilyCycle();
                        }

                        char str[20];
                        sprintf(str, "%.2f %%%%", **slider * 0.01f);
                        (*slider)(str);
                    }
                }
            }

            mNavFocus();
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
        bool SectionHasDlg(int32_t section) {
            switch (section) {
            case TH18_ST1_BOSS1:
            case TH18_ST2_BOSS1:
            case TH18_ST3_BOSS1:
            case TH18_ST4_BOSS1:
            case TH18_ST5_BOSS1:
            case TH18_ST6_BOSS1:
            case TH18_ST7_END_NS1:
            case TH18_ST7_MID1:
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
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cba[*mStage][*mWarp - 2][*mSection]))
                    mDlg();
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                if (mSection(TH_WARP_SELECT[*mWarp],
                        th_sections_cbt[*mStage][*mWarp - 4],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cbt[*mStage][*mWarp - 4][*mSection]))
                    mDlg();
                break;
            default:
                break;
            }
        }
        void OnPreUpdate() override
        {
            OnLocaleChange();
        }

        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage { TH_STAGE, TH_STAGE_SELECT };
        Gui::GuiCombo mWarp { TH_WARP, TH_WARP_SELECT };
        Gui::GuiCombo mSection { TH_MODE };
        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiCheckBox mDlg { TH_DLG };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 42949672950, 10, 10000000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 7 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLifeFragment { TH_LIFE_FRAGMENT, 0, 2 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 7 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBombFragment { TH_BOMB_FRAGMENT, 0, 2 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 100, 400 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mFunds { TH18_FUNDS, 0, 999990, 1, 100000 };

        Gui::GuiSlider<int, ImGuiDataType_S32> mKozuchi { TH18_KOZUCHI_CD, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mKaname { TH18_KANAME_CD, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mMoon { TH18_MOON_CD, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mMikoflash { TH18_MIKOFLASH_CD, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mVampire { TH18_VAMPIRE_CD, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mSun { TH18_SUN_CD, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLilyCount { TH18_LILY_COUNT, 0, 10, 1, 1, 1 };
        Gui::GuiCombo mLilyCycle { TH18_LILY_CYCLE, TH18_LILY_CYCLE_LIST };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLilyCD { TH18_LILY_CD, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBassdrum { TH18_BASSDRUM_CD, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPsyco { TH18_PSYCO_CD, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mCylinder { TH18_CYLINDER_CD, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mRiceball { TH18_RICEBALL_CD, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mMukade { TH18_MUKADE, 0, 800, 1, 100 };

        std::unordered_map<int, std::pair<Gui::GuiSlider<int, ImGuiDataType_S32>*, bool>> mCardToggles = {
            { KOZUCHI, { &mKozuchi, false } },
            { KANAME, { &mKaname, false } },
            { MOON, { &mMoon, false } },
            { MIKOFLASH, { &mMikoflash, false } },
            { VAMPIRE, { &mVampire, false } },
            { SUN, { &mSun, false } },
            { LILY, { &mLilyCD, false } },
            { BASSDRUM, { &mBassdrum, false } },
            { PSYCO, { &mPsyco, false } },
            { CYLINDER, { &mCylinder, false } },
            { RICEBALL, { &mRiceball, false } },
            { MUKADE, { &mMukade, false } },
        };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP, TH_DLG,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_SCORE, TH_LIFE, TH_LIFE_FRAGMENT, TH_BOMB, TH_BOMB_FRAGMENT,
            TH_POWER, TH18_FUNDS, TH18_MUKADE, TH18_KOZUCHI_CD, TH18_KANAME_CD,
            TH18_MOON_CD, TH18_MIKOFLASH_CD, TH18_VAMPIRE_CD, TH18_SUN_CD,
            TH18_LILY_COUNT, TH18_LILY_CYCLE, TH18_LILY_CD, TH18_BASSDRUM_CD,
            TH18_PSYCO_CD,TH18_CYLINDER_CD, TH18_RICEBALL_CD };

        int mChapterSetup[7][2] {
            { 3, 2 },
            { 3, 2 },
            { 3, 3 },
            { 4, 4 },
            { 4, 5 },
            { 3, 2 },
            { 6, 5 },
        };

        int mDiffculty = 0;
    };

     struct LoadedReplayData {
        std::wstring originalName;
        std::wstring originalPath;
        uint64_t metroHash[2];
        uint32_t header[9];
        void* decoded = nullptr;
        void* extraData = nullptr;
        size_t extraSize = 0;
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
        uint32_t mSelectedRepScores[STAGE_COUNT];
        LoadedReplayData mSelectedRepData;
        std::wstring mSelectedRepDir;
        std::wstring mSelectedRepName;
        std::wstring mSelectedRepPath;
        std::wstring mAppdataPath;
        uint64_t mRepMetroHash[2];

        void DisableCardFix();
        void EnableCardFix(LoadedReplayData& rd);
        void ResetCardFix();
        void ResetScoreFix();

        void CheckReplay()
        {
            const uint32_t index = GetMemContent(MAIN_MENU_PTR, 0x5aac);
            const Replay replay = *GetMemContent<Replay*>(MAIN_MENU_PTR, index * 4 + 0x5ab4);

            std::wstring repName = mb_to_utf16(replay.file_path, 932);
            std::wstring repDir(mAppdataPath);
            repDir.append(L"\\ShanghaiAlice\\th18\\replay\\");
            mSelectedRepName = repName;
            mSelectedRepDir = repDir;

            // if selected replay changes, reset fixed score cache
            if (mSelectedRepPath != repDir + repName) {
                mSelectedRepPath = repDir + repName;
                ResetCardFix();
                ResetScoreFix();
            }

            // load thprac params if in replay
            std::string param;
            if (ReplayLoadParam(mSelectedRepPath.c_str(), param) && mRepParam.ReadJson(param))
                mParamStatus = true;
            else
                mRepParam.Reset();

            
            // determine scores & start/end stages of replay
            for (size_t st = 1; st <= 7; ++st) {
                if (replay.stage_data[st].input_start) {
                    if (!mSelectedRepStartStage)
                        mSelectedRepStartStage = st;
                    mSelectedRepEndStage = st;

                    mSelectedRepScores[st - 1] = GetMemContent((uintptr_t)&replay.stage_data[st].gamestate_start, 0x68 + 0x20);
                }
            }
            mSelectedRepScores[mSelectedRepEndStage] = GetMemContent((uintptr_t)&replay.info, 0x18);

            // load & decrypt file data
            LoadSelectedReplayData();
            EnableCardFix(mSelectedRepData);
        }

        bool mRepStatus = false;
        void State(int state)
        {
            switch (state) {
            case 1:
                mRepMetroHash[0] = 0;
                mRepMetroHash[1] = 0;
                mRepSelected = false;
                mRepStatus = false;
                mParamStatus = false;
                mSelectedRepStartStage = 0;
                mSelectedRepEndStage = 0;
                mSelectedRepPlaybackStartStage = 0;
                memset(mSelectedRepScores, 0, sizeof(mSelectedRepScores));
                UnloadReplayData();
                DisableCardFix();

                thPracParam.Reset();
                break;
            case 2:
                CheckReplay();
                mRepSelected = true;
                break;
            case 3:
                mRepStatus = true;
                mSelectedRepPlaybackStartStage = GetMemContent(RVA(MAIN_MENU_PTR), 0x24) + 1;

                if (mParamStatus)
                    memcpy(&thPracParam, &mRepParam, sizeof(THPracParam));
                CalcFileHash(mSelectedRepPath.c_str(), mRepMetroHash);
                break;
            default:
                break;
            }
        }

        #define ThDecrypt(data, size1, param1, param2, param3, size2) asm_call<0x401e40, Fastcall>(data, size1, param1, param2, param3, size2)
#define ThUnlzss(dataBuffer, dataSize, outBuffer, outSize) asm_call<0x46f840, Fastcall>(dataBuffer, dataSize, outBuffer, outSize)

        __declspec(noinline) void LoadSelectedReplayData()
        {
            // Load replay file
            MappedFile file(THGuiRep::singleton().mSelectedRepPath.c_str());
            mSelectedRepData.originalName = THGuiRep::singleton().mSelectedRepName;
            mSelectedRepData.originalPath = THGuiRep::singleton().mSelectedRepPath;

            // Decode and copy data
            void* mRepDataRaw = nullptr;
            memcpy(mSelectedRepData.header, file.fileMapView, 0x24);
            mRepDataRaw = malloc(mSelectedRepData.header[7]);
            mSelectedRepData.decoded = malloc(mSelectedRepData.header[8]);
            mSelectedRepData.extraSize = file.fileSize - mSelectedRepData.header[3];
            mSelectedRepData.extraData = malloc(mSelectedRepData.extraSize);
            memcpy(mRepDataRaw, (void*)((uint32_t)file.fileMapView + 0x24), mSelectedRepData.header[7]);
            memcpy(mSelectedRepData.extraData, (void*)((uint32_t)file.fileMapView + mSelectedRepData.header[3]), mSelectedRepData.extraSize);
            ThDecrypt(mRepDataRaw, mSelectedRepData.header[7], 0x5c, 0xe1, 0x400, mSelectedRepData.header[7]);
            ThDecrypt(mRepDataRaw, mSelectedRepData.header[7], 0x7d, 0x3a, 0x100, mSelectedRepData.header[7]);
            ThUnlzss(mRepDataRaw, mSelectedRepData.header[7], mSelectedRepData.decoded, mSelectedRepData.header[8]);
            free(mRepDataRaw);

            // Calc Hash
            mSelectedRepData.metroHash[0] = 0;
            mSelectedRepData.metroHash[1] = 0;
            MetroHash128::Hash((uint8_t*)file.fileMapView, file.fileSize, (uint8_t*)mSelectedRepData.metroHash);
        }

        __declspec(noinline) void UnloadReplayData()
        {
            if (mSelectedRepData.decoded) {
                free(mSelectedRepData.decoded);
                mSelectedRepData.decoded = nullptr;
            }
            if (mSelectedRepData.extraData) {
                free(mSelectedRepData.extraData);
                mSelectedRepData.extraData = nullptr;
                mSelectedRepData.extraSize = 0;
            }
        }

    protected:
        bool mParamStatus = false;
    };

     const uint8_t* GetEquippedCardCounts()
    {
        uint32_t* list = nullptr;
        static uint8_t cardIdArray[64];
        memset(cardIdArray, 0, 64);

        for (uint32_t* i = (uint32_t*)GetMemContent(ABILITY_MANAGER_PTR, 0x1c); i; i = (uint32_t*)i[1]) {
            list = i;
            auto cardId = ((uint32_t**)list)[0][1];
            cardIdArray[cardId] += 1;
        }

        return cardIdArray;
    }

    void AddIndicateCard()
    {
        if (GetMemContent(0x4ccd00) == 4) {
            th18_free_blank.Enable();
            asm_call<0x411460, Thiscall>(GetMemContent(ABILITY_MANAGER_PTR), 0, 2);
        } else if (!GetEquippedCardCounts()[55]) {
            asm_call<0x411460, Thiscall>(*(uint32_t*)ABILITY_MANAGER_PTR, 55, 2);
        }
    }
    PATCH_ST(th18_pause_skip_1, 0x458692, "E93F010000");
    PATCH_ST(th18_pause_skip_2, 0x4588e3, "0F1F8000000000");
    PATCH_ST(th18_shop_disable, 0x4181ff, "00000000");
    EHOOK_ST(th18_shop_escape_1, 0x4181f9, 10, {
        pCtx->Eip = 0x4183d9;
        self->Disable();
    });
    EHOOK_ST(th18_shop_escape_2, 0x418402, 6, {
        pCtx->Eip = 0x4184a0;
        self->Disable();
    });

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

            th18_pause_skip_1.Setup();
            th18_pause_skip_2.Setup();
            th18_shop_disable.Setup();
            th18_shop_escape_1.Setup();
            th18_shop_escape_2.Setup();
            th18_free_blank.Setup();
        }
        SINGLETON(THOverlay)

    protected:
        inline void ResetCardMenu()
        {
            th18_pause_skip_1.Enable();
            th18_pause_skip_2.Enable();
            asm_call<0x458680, Thiscall>(GetMemContent(0x4cf40c));
            *(uint32_t*)GetMemAddr(ABILITY_SHOP_PTR, 0xe38) = 0;
            th18_pause_skip_1.Disable();
            th18_pause_skip_2.Disable();
        }
        inline void AddCard(uint32_t cardId)
        {
            if (cardId < 55)
                asm_call<0x411460, Thiscall>(*(uint32_t*)ABILITY_MANAGER_PTR, cardId, 2);
                asm_call<0x418de0, Fastcall>(cardId, 0);
        }
        void CheckMarket()
        {
            bool isMarketAvail = GetMemContent(ABILITY_SHOP_PTR);
            if (isInMarket != isMarketAvail) {
                th18_shop_disable.Disable();
                th18_shop_escape_1.Disable();
                th18_shop_escape_2.Disable();
                isInMarket = isMarketAvail;
            }
            if (!isInMarket) {
                isManipMarket = false;
            }
        }

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
            mTimeLock.SetTextOffsetRel(x_offset_1, x_offset_2);
            mAutoBomb.SetTextOffsetRel(x_offset_1, x_offset_2);
            mElBgm.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInGameInfo.SetTextOffsetRel(x_offset_1, x_offset_2);
        }
        virtual void OnContentUpdate() override
        {
            if (!isManipMarket) {
                mMuteki();
                mInfLives();
                mInfBombs();
                mInfPower();
                mInfFunds();
                mTimeLock();
                mAutoBomb();
                mZeroCD();
                mElBgm();
                if (isInMarket && GetMemContent(ABILITY_SHOP_PTR, 0xe38) == 2) {
                    if (mMarketManip()) {
                        *mMarketManip = false;
                        isManipMarket = true;
                        th18_shop_disable.Enable();
                        AddIndicateCard();
                    }
                } else {
                    mOpenMarket();
                }
            } else {
                ImGui::TextUnformatted(S(TH18_MARKET_MANIP_DESC1));
                ImGui::TextUnformatted(S(TH18_MARKET_MANIP_DESC2));
                ImGui::TextUnformatted(S(TH18_MARKET_MANIP_DESC3));
                ImGui::TextUnformatted(Gui::HotkeyChordToLabel(Gui::GetBackspaceMenuChord()).c_str());
                ImGui::SameLine();
                ImGui::TextUnformatted(S(TH18_MARKET_MANIP_DESC4));
            }
            mInGameInfo();
        }
        virtual void OnPreUpdate() override
        {
            CheckMarket();
            if (isInMarket && isManipMarket) {
                Open();
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255.0f, 0.0f, 0.0f, 255.0f));
                popColor = true;

                if (Gui::GetChordPressed(Gui::GetBackspaceMenuChord())) {
                    th18_shop_escape_1.Enable();
                    th18_shop_escape_2.Enable();
                } else {
                    bool mmKeyStatus[10];
                    for (int i = 0; i <= 8; ++i) {
                        mmKeyStatus[i] = (Gui::KeyboardInputUpdate('0' + i) == 1);
                    }
                    for (int i = 1; i <= 8; ++i) {
                        if (mmKeyStatus[i]) {
                            uint32_t cardsCount = GetMemContent(ABILITY_SHOP_PTR, 0xa2c);
                            uint32_t cardsSrcIndex = (i - 1) * cardsCount + 1;
                            uint32_t cardsSrcOffset = (CARD_DESC_LIST + 0x34 * cardsSrcIndex);
                            uint32_t* cardsDestOffset = (uint32_t*)GetMemAddr(ABILITY_SHOP_PTR, 0xa30);

                            for (uint32_t _ = 0; _ < cardsCount; ++_) {
                                if (cardsSrcIndex >= 55) {
                                    *cardsDestOffset = CARD_DESC_LIST + 0x34 * 56;
                                } else {
                                    *cardsDestOffset = cardsSrcOffset;
                                    cardsSrcOffset += 0x34;
                                    cardsSrcIndex++;
                                }
                                cardsDestOffset++;
                            }

                            ResetCardMenu();

                            break;
                        }
                    }
                    if (mmKeyStatus[0]) {
                        if (Gui::KeyboardInputGetRaw(VK_CONTROL)) {
                            uint32_t cardsCount = GetMemContent(ABILITY_SHOP_PTR, 0xa2c);
                            for (uint32_t i = 0; i < cardsCount; ++i) {
                                auto cardId = GetMemContent(ABILITY_SHOP_PTR, 0xa30 + i * 4, 4);
                                AddCard(cardId);
                            }

                        } else {
                            auto cardIndex = GetMemContent(ABILITY_SHOP_PTR, 0xe4);
                            auto cardId = GetMemContent(ABILITY_SHOP_PTR, 0xa30 + cardIndex * 4, 4);
                            AddCard(cardId);
                        }
                    }
                }
            } else if (mMenu(false) && !ImGui::IsAnyItemActive()) {
                if (*mMenu) {
                    Open();
                } else {
                    Close();
                }
            }
        }
        virtual void OnPostUpdate() override
        {
            if (popColor) {
                ImGui::PopStyleColor();
                popColor = false;
            }
        }

        Gui::GuiHotKeyChord mMenu { "ModMenuToggle", "BACKSPACE", Gui::GetBackspaceMenuChord() };
        
        HOTKEY_DEFINE(mMuteki, TH_MUTEKI, "F1", VK_F1)
        PATCH_HK(0x45d4ea, "01")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfBombs, TH_INFBOMBS, "F3", VK_F3)
        PATCH_HK(0x4574d3, "90909090"),
        PATCH_HK(0x40a3ed, "909090909090"),
        PATCH_HK(0x40a42c, "909090909090")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfPower, TH_INFPOWER, "F4", VK_F4)
        PATCH_HK(0x45748e, NOP(2)),
        PATCH_HK(0x418283, NOP(2)),
        EHOOK_HK(0x418427, 3, { // add indicator card for fraudulent purchases
            uint32_t price = GetMemContent(CARD_PRICE_TABLE + 0x4 * ((TableCardData*)pCtx->Eax)->price);
            if (price > GetMemContent(FUNDS) + GetMemContent(POWER) - 100)
                AddIndicateCard();
        })
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfFunds, TH18_INFFUNDS, "F5", VK_F5)
        PATCH_HK(0x45c244, "909090909090"),
        PATCH_HK(0x40d96f, "90909090909090909090"),
        PATCH_HK(0x418496, "90909090909090909090"),
        PATCH_HK(0x418465, NOP(2)),
        PATCH_HK(0x418225, NOP(2)),
        EHOOK_HK(0x41842a, 5, { // add indicator card for fraudulent purchases
            uint32_t price = GetMemContent(CARD_PRICE_TABLE + 0x4 * ((TableCardData*)pCtx->Eax)->price);
            if (price > GetMemContent(FUNDS) + GetMemContent(POWER) - 100)
                AddIndicateCard();
        })
        HOTKEY_ENDDEF();
        
        
        HOTKEY_DEFINE(mAutoBomb, TH_AUTOBOMB, "F7", VK_F7)
        PATCH_HK(0x45c2bd, "909090909090")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mZeroCD, TH18_ZERO_CD, "F8", VK_F8)
        EHOOK_HK(0x45c0e3, 2, {
            struct Timer {
                int32_t prev;
                int32_t cur;
                float cur_f;
                void* unused;
                uint32_t control;
            };
            Timer* timer = (Timer*)(pCtx->Ecx + 0x34);
            *timer = { -1, 0, 0, 0, 0 };
        })
        HOTKEY_ENDDEF();

        Gui::GuiHotKey mMarketManip { TH18_MARKET_MANIP, "F10", VK_F10 };
        bool isInMarket = false;
        bool isManipMarket = false;
        bool popColor = false;

    public:
        Gui::GuiHotKey mInfLives { TH_INFLIVES2, "F2", VK_F2 };

        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F6", VK_F6)
        PATCH_HK(0x429eef, "eb"),
        PATCH_HK(0x43021b, "058d")
        HOTKEY_ENDDEF();

        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F9", VK_F9 };
        Gui::GuiHotKey mOpenMarket { TH18_OPEN_MARKET, "F10", VK_F10 };
        Gui::GuiHotKey mInGameInfo { THPRAC_INGAMEINFO, "1", '1' };
    };

    class TH18InGameInfo : public Gui::GameGuiWnd {
        TH18InGameInfo() noexcept
        {
            SetTitle("igi");
            SetFade(0.9f, 0.9f);
            SetPosRel(900.0f / 1280.0f, 840.0f / 960.0f);
            SetSizeRel(340.0f / 1280.0f, 0.0f);
            SetWndFlag(ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | 
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
            OnLocaleChange();
        }
        SINGLETON(TH18InGameInfo)

    public:
        int32_t mMissCount;
        int32_t mBombCount;
        int32_t mDeadBombCount;

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
            ImGui::Columns(2);
            ImGui::Text(S(THPRAC_INGAMEINFO_MISS_COUNT));
            ImGui::NextColumn();
            ImGui::Text("%6d (%d)", mMissCount, mDeadBombCount);
            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_BOMB_COUNT));
            ImGui::NextColumn();
            ImGui::Text("%8d", mBombCount);
            
        }

        virtual void OnPreUpdate() override
        {
            if (GetMemContent(PLAYER_PTR)) {
                GameUpdateInner(18);
            } else {
            }
            if (*(THOverlay::singleton().mInGameInfo) && GetMemContent(PLAYER_PTR)) {
                SetPosRel(900.0f / 1280.0f, 840.0f / 960.0f);
                SetSizeRel(340.0f / 1280.0f, 0.0f);
                Open();
            } else {
                Close();
            }
        }

    public:
    };

    class THGuiSP : public Gui::GameGuiWnd {
        THGuiSP() noexcept
        {
            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiSP)

    public:
        int mState = 0;
        __declspec(noinline) int State()
        {
            uint32_t spell_id;

            switch (mState) {
            case 0:
                spell_id = GetMemContent(0x4b77f0, GetMemContent(0x4b77f0, 0x24) * 4 + 0x5ddc);
                if (mSpellId != spell_id) {
                    *mPhase = 0;
                    mSpellId = spell_id;
                }
                if (!SpellPhase())
                    return 1;
                SetFade(0.8f, 0.1f);
                Open();
                thPracParam.Reset();
                mState = 1;
                break;
            case 1:
                if (IsOpen()) {
                    if (Gui::InGameInputGetConfirm()) {
                        SetFade(0.8f, 0.1f);
                        Close();

                        // Fill Param
                        thPracParam.mode = 2;
                        thPracParam.phase = *mPhase;

                        // Reset Frame Counter
                        *(int32_t*)(*((int32_t*)0x4b77f0) + 0x2b0) = 0;

                        mState = 0;

                        return 1;
                    } else if (Gui::InGameInputGet('X')) {
                        mState = 2;
                    }
                }
                break;
            case 2:
                SetFade(0.8f, 0.1f);
                Close();
                if (IsClosed())
                    mState = 0;
            default:
                break;
            }

            return 0;
        }

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_SPELL_PRAC));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSizeRel(0.38f, 0.12f);
                SetPosRel(0.35f, 0.45f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(0.38f, 0.12f);
                SetPosRel(0.35f, 0.45f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(0.38f, 0.12f);
                SetPosRel(0.35f, 0.45f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(S(TH_SPELL_PRAC));
            ImGui::Separator();

            PracticeMenu();
        }
        th_glossary_t* SpellPhase()
        {
            if (mSpellId >= 84 && mSpellId <= 87) {
                return nullptr;
            } else if (mSpellId == 100) {
                return nullptr;
            }
            return nullptr;
        }
        void PracticeMenu()
        {
            mPhase(TH_PHASE, SpellPhase());
            mNavFocus();
        }

        unsigned int mSpellId = UINT_MAX;

        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiNavFocus mNavFocus { TH_PHASE };
    };

    static const char* scoreDispFmt = "%s  %.8u%u";

    static constinit HookCtx scoreUncapStageTrFix[2] = {
        { .addr = 0x4179c2, .data = PatchCode("e800000000") },
        { .addr = 0x463040, .data = PatchCode("e800000000") },
    };
    static constinit HookCtx scoreUncapHooks[] = {
        { .addr = 0x419e70, .data = PatchCode("ffffffff") },
        { .addr = 0x42a7fd, .data = PatchCode("ffffffff") },
        { .addr = 0x42a80f, .data = PatchCode("ffffffff") },
        { .addr = 0x430eab, .data = PatchCode("ffffffff") },
        { .addr = 0x430eb6, .data = PatchCode("ffffffff") },
        { .addr = 0x44476b, .data = PatchCode("ffffffff") },
        { .addr = 0x44477a, .data = PatchCode("ffffffff") },
        { .addr = 0x444ad9, .data = PatchCode("ffffffff") },
        { .addr = 0x444ade, .data = PatchCode("ffffffff") },
        { .addr = 0x444c00, .data = PatchCode("ffffffff") },
        { .addr = 0x444c05, .data = PatchCode("ffffffff") },
        { .addr = 0x4462eb, .data = PatchCode("ffffffff") },
        { .addr = 0x446302, .data = PatchCode("ffffffff") },
        { .addr = 0x4463a1, .data = PatchCode("ffffffff") },
        { .addr = 0x4463b1, .data = PatchCode("ffffffff") },
        { .addr = 0x44656e, .data = PatchCode("ffffffff") },
        { .addr = 0x446578, .data = PatchCode("ffffffff") },
        { .addr = 0x446ac6, .data = PatchCode("ffffffff") },
        { .addr = 0x446ad7, .data = PatchCode("ffffffff") },
        { .addr = 0x446d09, .data = PatchCode("ffffffff") },
        { .addr = 0x446d1a, .data = PatchCode("ffffffff") },
        { .addr = 0x45f2c4, .data = PatchCode("ffffffff") },
        { .addr = 0x45f2cf, .data = PatchCode("ffffffff") },
    };
   
    PATCH_ST(th18_all_clear_bonus_1, 0x4448ab, "eb0b909090");
    EHOOK_ST(th18_all_clear_bonus_2, 0x444afa, 7, {
        *(int32_t*)(GetMemAddr(0x4cf2e0, 0x158)) = *(int32_t*)(SCORE);
        if (GetMemContent(MODEFLAGS) & 0x10) {
            typedef void (*PScoreFunc)();
            PScoreFunc a = (PScoreFunc)0x458bd0;
            a();
            pCtx->Eip = 0x4448b0;
        }
    });
    EHOOK_ST(th18_all_clear_bonus_3, 0x444c49, 7, {
        *(int32_t*)(GetMemAddr(0x4cf2e0, 0x158)) = *(int32_t*)(SCORE);
        if (GetMemContent(MODEFLAGS) & 0x10) {
            typedef void (*PScoreFunc)();
            PScoreFunc a = (PScoreFunc)0x458bd0;
            a();
            pCtx->Eip = 0x4448b0;
        }
    });
    EHOOK_ST(th18_score_uncap_replay_fix, 0x4620b9, 3, {
        if (pCtx->Eax > COUNTERSTOP)
            pCtx->Eax = COUNTERSTOP;
    });
    EHOOK_ST(th18_score_uncap_replay_disp, 0x468405, 1, {
        *(const char**)(pCtx->Esp) = scoreDispFmt;
    });

    float g_bossMoveDownRange = BOSS_MOVE_DOWN_RANGE_INIT;
    EHOOK_ST(th18_bossmovedown, 0x00433347, 5, {
        float* y_pos = (float*)(pCtx->Edi + 0x4FE4);
        float* y_range = (float*)(pCtx->Edi + 0x4FEC);
        float y_max = (*y_pos) + (*y_range) * 0.5f;
        float y_min2 = y_max - (*y_range) * (1.0f - g_bossMoveDownRange);
        *y_pos = (y_max + y_min2) * 0.5f;
        *y_range = (y_max - y_min2);
    });
    HOOKSET_DEFINE(th18_master_disable)
    PATCH_DY(th18_master_disable1a, 0x42A26E, "eb")
    PATCH_DY(th18_master_disable1b, 0x42A2B3, "eb")
    PATCH_DY(th18_master_disable1c, 0x42A1C6, "03")
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(th18_bossmovement)
    // opposite
    PATCH_DY(th18_bossmovement0, 0x4334D4, "eb")
    PATCH_DY(th18_bossmovement1, 0x4334A3, "9090")
    // samedir
    PATCH_DY(th18_bossmovement2, 0x4334D4, "9090")
    PATCH_DY(th18_bossmovement3, 0x4334A3, "eb")
    HOOKSET_ENDDEF()

    extern HookCtx th18_static_mallet_replay_gold;
    extern HookCtx th18_static_mallet_replay_green;
    extern HookCtx th18_rep_card_fix;

    class THAdvOptWnd : public Gui::PPGuiWnd {
    private:
        THAdvOptWnd() noexcept
        {
            SetWndFlag(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
            SetFade(0.8f, 0.8f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);

            InitUpdFunc([&]() { ContentUpdate(); },
                [&]() { LocaleUpdate(); },
                [&]() { PreUpdate(); },
                []() {});

            OnLocaleChange();
            FpsInit();
            GameplayInit();
            MasterDisableInit();
            ScoreUncapInit();
            BossMovementInit();
            th18_rep_card_fix.Setup();
            th18_static_mallet_replay_gold.Setup();
            th18_static_mallet_replay_green.Setup();
            th18_bossmovedown.Setup();
        }
        SINGLETON(THAdvOptWnd)

    public:
        static void StaticMalletConversion(PCONTEXT pCtx) {
            int32_t mallet_cancel_item_type = GetMemContent(BULLET_MANAGER_PTR, 0x7a41d0) % 30;
            
            switch (mallet_cancel_item_type) {
                case 0:
                case 3:
                case 6:
                case 8:
                case 11:
                case 14:
                case 16:
                case 19:
                case 22:
                case 25:
                case 28:
                    pCtx->Eip = 0x429222; // gold
                    break;
                default:
                    pCtx->Eip = 0x42917b; // green
            }
        }

        bool scoreUncapChkbox = true;
        bool scoreUncapOverride = false;
        bool staticMalletReplay = false;
    public:
        int forceBossMoveDir = 0;
        bool forceBossMoveDown = false;
        

        bool st6FinalFix = false;
        bool scrollFix = false;
        bool mukadeFix = false;
        bool restartResetMarket = false;
        bool activeCardIdFix = false;
        bool eirinEikiCardFix = false;
        bool funcCallFix = false;
        bool activeCardRepFix = false;

        struct FixData {
            uint32_t stage;

            int32_t activeCardComboIdx;
            std::vector<int32_t> activeCardVec;
            std::vector<int32_t> activeCardLabelVec;
            int32_t activeCardId;
            uint32_t activeCardIdPtr;
        };
        bool mShowFixInstruction = false;
        std::vector<FixData> mFixData;
        uint32_t mScoreOverwrites[STAGE_COUNT];
        
        __declspec(noinline) uint32_t* FindCardDesc(uint32_t id)
        {
            for (uint32_t i = CARD_DESC_LIST; true; i += 0x34) {
                if (*(uint32_t*)(i + 4) == id) {
                    return (uint32_t*)i;
                }
            }
        }

#define ThEncrypt(data, size1, param1, param2, param3, size2) asm_call<0x401f50, Fastcall>(data, size1, param1, param2, param3, size2)
#define ThLzss(dataBuffer, dataSize, outSize)                 asm_call<0x46f5b0, Fastcall, void*>(dataBuffer, dataSize, outSize)
#define _builtin_free(buffer)                                 asm_call<0x491a3f, Cdecl>(buffer)

        __declspec(noinline) bool SaveReplayWithData(LoadedReplayData& rd, void* repDataCopy)
        {
            // Setup header copy
            uint32_t repHeader[9];
            memcpy(repHeader, rd.header, sizeof(repHeader));

            // Re-encode data from data copy
            uint32_t repDataEncodedSize;
            auto repDataEncoded = ThLzss(repDataCopy, repHeader[8], &repDataEncodedSize);
            repHeader[7] = repDataEncodedSize;
            repHeader[3] = repDataEncodedSize + 0x24;
            ThEncrypt(repDataEncoded, rd.header[7], 0x7d, 0x3a, 0x100, repHeader[7]);
            ThEncrypt(repDataEncoded, rd.header[7], 0x5c, 0xe1, 0x400, repHeader[7]);
            free(repDataCopy);

            // Setup file picker
            DWORD bytesProcessed;
            OPENFILENAMEW ofn;
            wchar_t szFile[512];
            wcscpy_s(szFile, L"th18_ud----.rpy");
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = *(HWND*)WINDOW_PTR;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Replay File\0*.rpy\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = nullptr;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = THGuiRep::singleton().mSelectedRepDir.c_str();
            ofn.lpstrDefExt = L".rpy";
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

            // Open file picker & write file
            if (GetSaveFileNameW(&ofn)) {
                auto outputFile = CreateFileW(szFile, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (outputFile == INVALID_HANDLE_VALUE) {
                    MsgBox(MB_ICONERROR | MB_OK, S(TH_ERROR), S(TH_REPFIX_SAVE_ERROR_DEST), nullptr, ofn.hwndOwner);
                    goto end;
                }
                SetFilePointer(outputFile, 0, nullptr, FILE_BEGIN);
                SetEndOfFile(outputFile);
                WriteFile(outputFile, repHeader, sizeof(repHeader), &bytesProcessed, nullptr);
                WriteFile(outputFile, repDataEncoded, repDataEncodedSize, &bytesProcessed, nullptr);
                WriteFile(outputFile, rd.extraData, rd.extraSize, &bytesProcessed, nullptr);
                CloseHandle(outputFile);

                // OK checkbox & close replay menu to force a reload
                MsgBox(MB_ICONINFORMATION | MB_OK, S(TH_REPFIX_SAVE_SUCCESS), S(TH_REPFIX_SAVE_SUCCESS_DESC), utf16_to_utf8(szFile).c_str(), ofn.hwndOwner);
                uintptr_t mainMenu = GetMemContent(MAIN_MENU_PTR);
                if (mainMenu) {
                    THGuiRep::singleton().State(1);
                    asm_call<SET_MENU, Thiscall>(mainMenu, 1);
                }

                _builtin_free(repDataEncoded);
                return true;
            }
        end:
            _builtin_free(repDataEncoded);
            return false;
        }
        
        __declspec(noinline) bool SaveReplayScoreFix()
        {
            auto& guiRep = THGuiRep::singleton();
            LoadedReplayData rd = guiRep.mSelectedRepData;

            // Setup copy
            void* repDataCopy = malloc(rd.header[8]);
            memcpy(repDataCopy, rd.decoded, rd.header[8]);

            // Overwrite score data in replay data copy
            uint32_t repData = (uint32_t)rd.decoded + 0xc8;
            int limit = *(int*)((uint32_t)rd.decoded + 0xa8);
            if (limit >= 8)
                limit = 6;

            for (int i = 0; i < limit; ++i) {
                const uint16_t stage = GetMemContent<uint16_t>(repData);
                const uint32_t newScore = mScoreOverwrites[stage - 1];

                if (stage > 1 && stage <= 7 && newScore && guiRep.mSelectedRepScores[stage - 1] < newScore) {
                    uintptr_t trScoreAddr = (uint32_t)repDataCopy + (repData - (uint32_t)rd.decoded) + 0x88;
                    *(uint32_t*)trScoreAddr = newScore;
                }

                repData += *(uint32_t*)(repData + 8) + 0x126c;
            }

            uintptr_t finalScoreAddr = (uint32_t)repDataCopy + 0x18;
            *(uint32_t*)finalScoreAddr = (uint32_t)mScoreOverwrites[guiRep.mSelectedRepEndStage];

            return SaveReplayWithData(rd, repDataCopy);
        }

        __declspec(noinline) void ParseReplayCardData(LoadedReplayData& rd)
        {
            if (!rd.decoded)
                return;

            uint32_t repData = (uint32_t)rd.decoded + 0xc8;
            int limit = *(int*)((uint32_t)rd.decoded + 0xa8);
            if (limit >= 8)
                limit = 6;

            for (int i = 0; i < limit; ++i) {
                if (*(uint16_t*)(repData) >= 1 && *(uint16_t*)(repData) <= 7) {
                    FixData data;
                    data.stage = *(uint16_t*)(repData);

                    uint8_t cardTrigger[57];
                    memset(cardTrigger, 0, 57);
                    data.activeCardId = *(int32_t*)(repData + 0x964);
                    data.activeCardIdPtr = repData + 0x964 - (uint32_t)rd.decoded;
                    for (int32_t* j = (int32_t*)((uint32_t)repData + 0x164); *j >= 0; j++) {
                        auto cardStruct = FindCardDesc(*j);

                        if (cardStruct && cardStruct[3] == 0 && !cardTrigger[*j]) {
                            data.activeCardVec.push_back(*j);
                            data.activeCardLabelVec.push_back(TH18_CARD_LIST[*j]);
                            cardTrigger[*j] = 1;
                        }
                    }

                    data.activeCardLabelVec.push_back(0);
                    data.activeCardComboIdx = 0;
                    for (size_t k = 0; k < data.activeCardVec.size(); ++k) {
                        if (data.activeCardId == data.activeCardVec[k]) {
                            data.activeCardComboIdx = k;
                            break;
                        }
                    }

                    mFixData.push_back(data);
                }

                repData += *(uint32_t*)(repData + 8) + 0x126c;
            }
        }
        

        __declspec(noinline) bool SaveReplayCardFix()
        {
            LoadedReplayData& mRepLoaded = THGuiRep::singleton().mSelectedRepData;

            // Setup copy
            void* repDataCopy = malloc(mRepLoaded.header[8]);
            memcpy(repDataCopy, mRepLoaded.decoded, mRepLoaded.header[8]);
            // Overwrite card data in replay copy
            for (auto& fix : mFixData) {
                if (fix.activeCardVec.size()) {
                    int32_t* cardIdx = (int32_t*)(fix.activeCardIdPtr + (uint32_t)repDataCopy);
                    *cardIdx = fix.activeCardVec[fix.activeCardComboIdx];

                }
               
            }

            return SaveReplayWithData(mRepLoaded, repDataCopy);
        }
        
        bool GetAvailability()
        {
            LoadedReplayData& mRepLoaded = THGuiRep::singleton().mSelectedRepData;
            if (!mRepLoaded.decoded) {
                return false;
            }
            auto& repMenu = THGuiRep::singleton();
            if (GetMemContent(GAME_THREAD_PTR) && !GetMemContent(GAME_THREAD_PTR, 0xd0)) {
                return false;
            }
            if (repMenu.mRepStatus && (repMenu.mRepMetroHash[0] != mRepLoaded.metroHash[0] || repMenu.mRepMetroHash[1] != mRepLoaded.metroHash[1])) {
                return false;
            }
            return true;
        }
        bool ReplayMenu()
        {
            bool wndFocus = true;

            if (BeginOptGroup<TH_REPLAY_FIX>()) {
                // Counterstop replay fix tool
                CustomMarker(S(TH_REPFIX_NO_THPRAC), S(TH_REPFIX_NO_THPRAC_DESC));
                ImGui::SameLine();
                ImGui::TextUnformatted(S(TH18_CS_REPFIX));
                ImGui::SameLine();
                HelpMarker(S(TH18_CS_REPFIX_DESC));

                auto& guiReplay = THGuiRep::singleton();
                uint32_t finalScore = guiReplay.mSelectedRepScores[guiReplay.mSelectedRepEndStage];

                if (!scoreUncapChkbox)
                    ImGui::TextDisabled(S(TH18_CS_REPFIX_NO_UNCAP));
                else if (!finalScore)
                    ImGui::TextDisabled(S(TH_REPFIX_SELECTED_NONE));
                else if (finalScore < COUNTERSTOP)
                    ImGui::TextDisabled(S(TH18_CS_REPFIX_SELECTED_NO_CS));
                else if (finalScore > COUNTERSTOP)
                    ImGui::TextDisabled(S(TH_REPFIX_SELECTED_ALREADY_FIXED));
                else {
                    ImGui::Text(S(TH_REPFIX_SELECTED), THGuiRep::singleton().mSelectedRepName.c_str());
                    const uint32_t curStage = GetMemContent(RVA(STAGE_NUM));
                    const bool startedOnCS = guiReplay.mSelectedRepScores[guiReplay.mSelectedRepPlaybackStartStage - 1] == COUNTERSTOP;
                    const bool inTransition = GetMemContent(TRANSITION_STG_PTR);

                    uint32_t firstStageCS = 0;
                    uint32_t counterStopCount = 0;
                    uint32_t unknownCount = 0;

                    ImGui::Columns(2, 0, false);

                    for (size_t st = guiReplay.mSelectedRepStartStage; st <= guiReplay.mSelectedRepEndStage; st++) {
                        const uint32_t stScore = guiReplay.mSelectedRepScores[st];
                        const uint32_t stScoreOverwrite = mScoreOverwrites[st];
                        const uint32_t curScore = GetMemContent(SCORE);

                        if (stScore == COUNTERSTOP) {
                            if (!firstStageCS)
                                firstStageCS = st;
                            if (stScoreOverwrite <= stScore)
                                unknownCount += 1;

                            counterStopCount += 1;
                        }

                        ImGui::Text(S(st == 7 ? TH18_CS_REPFIX_EXTRA : TH18_CS_REPFIX_STAGE), st);
                        ImGui::SameLine();

                        if (stScoreOverwrite > stScore) {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.9f, 0.3f, 1.0f));
                            ImGui::Text("%s", FormatNumberWithCommas(((int64_t)stScoreOverwrite * 10)));
                            ImGui::PopStyleColor();
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip(S(TH18_CS_REPFIX_READY_HINT));

                        } else if (stScore == COUNTERSTOP) {
                            if (guiReplay.mRepStatus && (curStage == st || inTransition) && curScore && !startedOnCS) {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.3f, 1.0f));
                                ImGui::Text("%s", FormatNumberWithCommas(((int64_t)curScore * 10)));
                                ImGui::PopStyleColor();
                                if (ImGui::IsItemHovered())
                                    ImGui::SetTooltip(S(TH18_CS_REPFIX_RECORDING_HINT));

                            } else {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
                                ImGui::Text(S(TH_TYPE_UNKOWN));
                                ImGui::PopStyleColor();
                                if (ImGui::IsItemHovered())
                                    ImGui::SetTooltip(S(firstStageCS == st ? TH18_CS_REPFIX_FIRST_UNKNOWN_HINT
                                                                           : TH18_CS_REPFIX_UNKNOWN_HINT),
                                        firstStageCS);
                            }
                        } else {
                            ImGui::Text("%s", FormatNumberWithCommas(((int64_t)stScore * 10)));
                        }

                        if (st == 3)
                            ImGui::NextColumn();
                    }
                    ImGui::Columns(1);

                    if (unknownCount) {
                        char buttonLabel[64];
                        snprintf(buttonLabel, sizeof(buttonLabel), S(TH_REPFIX_SAVE_PROGRESS),
                            counterStopCount - unknownCount, counterStopCount);

                        ImGui::BeginDisabled();
                        ImGui::Button(buttonLabel);
                        ImGui::EndDisabled();
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip(S(TH18_CS_REPFIX_SAVE_PROGRESS_HINT), unknownCount, firstStageCS);

                    } else if (ImGui::Button(S(TH_REPFIX_SAVE)))
                        SaveReplayScoreFix();
                }

                // Seperate tools
                ImGui::NewLine();
                ImGui::Separator();

                // Active card replay desync fix tool
                CustomMarker(S(TH_REPFIX_NO_THPRAC), S(TH_REPFIX_NO_THPRAC_DESC));
                ImGui::SameLine();
                ImGui::TextUnformatted(S(TH18_AC_REPFIX));
                ImGui::SameLine();
                HelpMarker(S(TH18_AC_REPFIX_DESC));

                ImGui::SameLine();
                ImGui::Checkbox(S(TH_TOOL_SHOW_TOGGLE), &activeCardRepFix);

                if (activeCardRepFix) {
                    if (THGuiRep::singleton().mRepSelected) {
                        bool hasFixOptions = false;
                        for (auto& data : mFixData) {
                            if (data.activeCardId != -1) {
                                hasFixOptions = true;
                                break;
                            }
                        }

                        if (hasFixOptions) {
                            ImGui::Text(S(TH_REPFIX_SELECTED), THGuiRep::singleton().mSelectedRepName.c_str());

                            auto isAvailable = GetAvailability();
                            if (!isAvailable) {
                                ImGui::TextUnformatted(S(TH18_AC_REPFIX_MISMATCH));
                                ImGui::BeginDisabled();
                            }

                            char comboId[64];
                            auto fontSize = ImGui::GetFontSize();
                            ImGui::Columns(2, 0, false);

                            for (auto& data : mFixData) {
                                if (data.activeCardId != -1) {
                                    ImGui::Text(S(data.stage == 7 ? TH_EXTRA : TH_STAGE_NUM), data.stage);
                                    ImGui::SameLine();
                                    ImGui::TextUnformatted(S(TH18_AC_REPFIX_INITIAL_CARD));
                                    ImGui::SameLine(0.0f, 0.0f);

                                    sprintf_s(comboId, "##active_card_idx_st%d", data.stage);
                                    ImGui::PushItemWidth(fontSize * 10.0f);
                                    ImGui::ComboSectionsDefault(comboId, &data.activeCardComboIdx, data.activeCardLabelVec.data(), Gui::LocaleGetCurrentGlossary(), "");
                                    if (ImGui::IsPopupOpen(comboId)) {
                                        wndFocus = false;
                                    }
                                    ImGui::PopItemWidth();
                                }

                                if (data.stage == 3)
                                    ImGui::NextColumn();
                            }
                            ImGui::Columns(1);

                            if (!isAvailable)
                                ImGui::EndDisabled();

                            ImGui::PushID("CARDFIX_SAVE");
                            if (ImGui::Button(S(TH_REPFIX_SAVE)))
                                SaveReplayCardFix();
                            ImGui::PopID();

                        } else {
                            ImGui::TextDisabled(S(TH18_AC_REPFIX_NOTHING));
                        }
                    } else {
                        ImGui::TextDisabled(S(TH_REPFIX_SELECTED_NONE));
                    }
                }

                EndOptGroup();
            }

            return wndFocus;
        }

    public:
        void RestartResetMarket()
        {
            if (restartResetMarket) {
                if (*(uint32_t*)STAGE_NUM == *(uint32_t*)NEXT_STAGE_NUM) {
                    uint32_t* list = nullptr;
                    uint8_t cardIdArray[64];
                    memset(cardIdArray, 0, 64);
                    for (uint32_t* i = (uint32_t*)GetMemContent(ABILITY_MANAGER_PTR, 0x1c); i; i = (uint32_t*)i[1]) {
                        list = i;
                        auto cardId = ((uint32_t**)list)[0][1];
                        cardIdArray[cardId] += 1;
                    }

                    for (int i = 0; i < 56; ++i) {
                        *(uint32_t*)GetMemAddr(ABILITY_MANAGER_PTR, 0xc84 + i * 4) = cardIdArray[i] ? 1 : 0;
                    }
                }
            }
        }

    private:
        void FpsInit()
        {
            if ((mOptCtx.vpatch_base = (uintptr_t)GetModuleHandleW(L"openinputlagpatch.dll")) != NULL) {
                OILPInit(mOptCtx);
            } else if (*(uint8_t*)0x4cd011 == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x472e55, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x472e55 = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x472e55, 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 3) {
                mOptCtx.oilp_set_game_fps(mOptCtx.fps);
                mOptCtx.oilp_set_replay_skip_fps(mOptCtx.fps_replay_fast);
                mOptCtx.oilp_set_replay_slow_fps(mOptCtx.fps_replay_slow);
            } else if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
            }
        }
        void GameplayInit()
        {
            th18_all_clear_bonus_1.Setup();
            th18_all_clear_bonus_2.Setup();
            th18_all_clear_bonus_3.Setup();
        }
        void GameplaySet()
        {
            th18_all_clear_bonus_1.Toggle(mOptCtx.all_clear_bonus);
            th18_all_clear_bonus_2.Toggle(mOptCtx.all_clear_bonus);
            th18_all_clear_bonus_3.Toggle(mOptCtx.all_clear_bonus);
        }
        void MasterDisableInit()
        {
            for (int i = 0; i < 3; i++)
                th18_master_disable[i].Setup();
            for (int i = 0; i < 3; i++)
                th18_master_disable[i].Toggle(g_adv_igi_options.disable_master_autoly);
        }

        void BossMovementInit()
        {
            for (int i = 0; i < 4; i++)
                th18_bossmovement[i].Setup();
        }
        void ScoreUncapInit()
        {
            for (size_t i = 0; i < elementsof(scoreUncapHooks); i++) {
                scoreUncapHooks[i].Setup();
            }

            th18_score_uncap_replay_fix.Setup();
            th18_score_uncap_replay_disp.Setup();
            {
                *(uintptr_t*)((uintptr_t)scoreUncapStageTrFix[0].data.buffer.ptr + 1) = (uintptr_t)&globals_assign_hooked - 0x4179c7;
                *(uintptr_t*)((uintptr_t)scoreUncapStageTrFix[1].data.buffer.ptr + 1) = (uintptr_t)&globals_assign_hooked - 0x463045;
                scoreUncapStageTrFix[0].Setup();
                scoreUncapStageTrFix[1].Setup();
            }
        }
        void ScoreUncapSet()
        {
            for (auto& hook : scoreUncapHooks) {
                hook.Toggle(scoreUncapChkbox);
            }
            th18_score_uncap_replay_fix.Toggle(scoreUncapOverride);
            th18_score_uncap_replay_disp.Toggle(scoreUncapChkbox);
            scoreUncapStageTrFix[0].Toggle(scoreUncapChkbox);
            scoreUncapStageTrFix[1].Toggle(scoreUncapChkbox);
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
        void ContentUpdate()
        {
            bool wndFocus = true;

            ImGui::TextUnformatted(S(TH_ADV_OPT));
            ImGui::Separator();
            ImGui::BeginChild("Adv. Options", ImVec2(0.0f, 0.0f));

            if (BeginOptGroup<TH_GAME_SPEED>()) {
                if (GameFPSOpt(mOptCtx))
                    FpsSet();
                EndOptGroup();
            }
            if (BeginOptGroup<TH_GAMEPLAY>()) {
                DisableKeyOpt();
                KeyHUDOpt();
                InfLifeOpt();
                ImGui::Separator();

                ImGui::Checkbox(S(THPRAC_TH18_FORCE_CARD), &g_adv_igi_options.th18_force_card);
                if (g_adv_igi_options.th18_force_card)
                {
                    if (ImGui::TreeNode(S(THPRAC_TH18_CARD))) {
                        static const char* card_names[56];
                        { // set cards
                            static bool is_inited = false;
                            static int language = Gui::LocaleGet();
                            if (Gui::LocaleGet() != language) {
                                is_inited = false;
                            }
                            if (!is_inited) {
                                is_inited = true;
                                language = Gui::LocaleGet();
                                for (int i = 0; i < 56; i++) {
                                    card_names[i] = S(TH18_CARD_LIST[i]);
                                }
                            }
                        }
                        ImGui::Combo(S(THPRAC_TH18_CARD_1), &g_adv_igi_options.th18_cards[0], card_names, 56);
                        ImGui::Combo(S(THPRAC_TH18_CARD_2), &g_adv_igi_options.th18_cards[1], card_names, 56);
                        ImGui::Combo(S(THPRAC_TH18_CARD_3), &g_adv_igi_options.th18_cards[2], card_names, 56);
                        ImGui::Combo(S(THPRAC_TH18_CARD_4), &g_adv_igi_options.th18_cards[3], card_names, 56);
                        ImGui::Combo(S(THPRAC_TH18_CARD_5), &g_adv_igi_options.th18_cards[4], card_names, 56);
                        ImGui::Combo(S(THPRAC_TH18_CARD_7), &g_adv_igi_options.th18_cards[5], card_names, 56);
                        ImGui::TreePop();
                    }
                }

                if (ImGui::Checkbox(S(TH_BOSS_FORCE_MOVE_DOWN), &forceBossMoveDown)) {
                    th18_bossmovedown.Toggle(forceBossMoveDown);
                }
                ImGui::SameLine();
                HelpMarker(S(TH_BOSS_FORCE_MOVE_DOWN_DESC));
                ImGui::SameLine();
                ImGui::SetNextItemWidth(150.0f);
                if (ImGui::DragFloat(S(TH_BOSS_FORCE_MOVE_DOWN_RANGE), &g_bossMoveDownRange, 0.002f, 0.0f, 1.0f))
                    g_bossMoveDownRange = std::clamp(g_bossMoveDownRange, 0.0f, 1.0f);
                ImGui::SameLine();
                ImGui::InvisibleButton("##align", { 50.0f, 1.0f });
                ImGui::SameLine();
                const char* const chs[3] = { S(TH_BOSS_MOVE_DEFAULT), S(TH_BOSS_FORCE_MOVE_OPPOSITE_DIR), S(TH_BOSS_FORCE_MOVE_SAME_DIR) };
                ImGui::SetNextItemWidth(180.0f);
                if (ImGui::SliderInt(S(TH_BOSS_HORIZONTAL), &forceBossMoveDir, 0, 2, chs[std::clamp(forceBossMoveDir, 0, 2)]))
                {
                    forceBossMoveDir = std::clamp(forceBossMoveDir, 0, 2);
                    for(int i=0;i<4;i++)
                        th18_bossmovement[i].Toggle(false);
                    if (forceBossMoveDir == 1){
                        th18_bossmovement[0].Toggle(true);
                        th18_bossmovement[1].Toggle(true);
                    }else if (forceBossMoveDir == 2){
                        th18_bossmovement[2].Toggle(true);
                        th18_bossmovement[3].Toggle(true);
                    }
                }

                if (ImGui::Checkbox(S(TH_DISABLE_MASTER), &g_adv_igi_options.disable_master_autoly)) {
                    for (int i = 0; i < 3; i++)
                        th18_master_disable[i].Toggle(g_adv_igi_options.disable_master_autoly);
                }
                ImGui::SameLine();
                HelpMarker(S(TH_DISABLE_MASTER_DESC));
                ImGui::Checkbox(S(TH_ENABLE_LOCK_TIMER), &g_adv_igi_options.enable_lock_timer_autoly);

                if (GameplayOpt(mOptCtx))
                    GameplaySet();
                if (ImGui::Checkbox(S(TH18_UNCAP), &scoreUncapChkbox))
                    ScoreUncapSet();
                ImGui::SameLine();
                HelpMarker(S(TH18_UNCAP_DESC));

                /* Inclusion of this option is more confusing than it's worth
                * Note that score uncap already affects replay scores (e.g. st5)
                * & all the override does is force counterstop when writing the
                * last stage's score
                if (!scoreUncapChkbox) ImGui::BeginDisabled();
                if (ImGui::Checkbox(S(TH18_UNCAP_OVERRIDE), &scoreUncapOverride))
                    ScoreUncapSet();
                if (!scoreUncapChkbox) ImGui::EndDisabled();*/
                
                if (ImGui::Checkbox(S(TH18_STATIC_MALLET), &staticMalletReplay)) {
                    th18_static_mallet_replay_gold.Toggle(staticMalletReplay);
                    th18_static_mallet_replay_green.Toggle(staticMalletReplay);
                }
                ImGui::SameLine();
                HelpMarker(S(TH18_STATIC_MALLET_DESC));
                
                EndOptGroup();
            }
            //if (BeginOptGroup<TH_BUGFIX>()) {
            //ImGui::TextUnformatted(S(TH18_BUGFIX_DESC));
            ImGui::TextUnformatted(S(TH_BUGFIX_AUTO));
            ImGui::SameLine();
            HelpMarker(S(TH18_BUGFIX_AUTO_DESC));
            //EndOptGroup();
            //}

            wndFocus &= ReplayMenu();
            InGameReactionTestOpt();
            AboutOpt();
            ImGui::EndChild();
            // if (wndFocus)
            //     ImGui::SetWindowFocus();
        }
        void PreUpdate()
        {
            LocaleUpdate();
        }

        adv_opt_ctx mOptCtx;
    };

     __declspec(noinline) void THGuiRep::DisableCardFix()
    {
        th18_rep_card_fix.Disable();
    }

    __declspec(noinline) void THGuiRep::EnableCardFix(LoadedReplayData& rd)
    {
        auto& advOptWnd = THAdvOptWnd::singleton();

        if (!advOptWnd.mFixData.size())
            advOptWnd.ParseReplayCardData(rd);
        for (auto& data : advOptWnd.mFixData) {
            if (data.activeCardId != -1) {
                th18_rep_card_fix.Enable();
                return;
            }
        }
    }

    __declspec(noinline) void THGuiRep::ResetCardFix()
    {
        THAdvOptWnd::singleton().mFixData.clear();
        th18_rep_card_fix.Disable();
    }

    __declspec(noinline) void THGuiRep::ResetScoreFix()
    {
        auto& advOptWnd = THAdvOptWnd::singleton();
        memset(advOptWnd.mScoreOverwrites, 0, sizeof(advOptWnd.mScoreOverwrites));
    }

    EHOOK_ST(th18_rep_card_fix, 0x462e4b, 5, {
        auto& advOptWnd = THAdvOptWnd::singleton();

        if (advOptWnd.activeCardRepFix && advOptWnd.GetAvailability()) {
            auto& fixVec = advOptWnd.mFixData;
            for (auto& fix : fixVec) {
                if (fix.stage == *(uint32_t*)0x4CCCDC) {
                    *(int32_t*)pCtx->Esp = fix.activeCardVec[fix.activeCardComboIdx];
                    break;
                }
            }
        }
    });
    EHOOK_ST(th18_static_mallet_replay_gold, 0x429222, 6, {
        if (THGuiRep::singleton().mRepStatus)
            THAdvOptWnd::StaticMalletConversion(pCtx);
    });
    EHOOK_ST(th18_static_mallet_replay_green, 0x42921d, 5, {
        if (THGuiRep::singleton().mRepStatus)
            THAdvOptWnd::StaticMalletConversion(pCtx);
    });

    void ECLStdExec(ECLHelper& ecl, unsigned int start, int std_id, int ecl_time = 0)
    {
        if (start)
            ecl.SetPos(start);
        ecl << ecl_time << 0x00140276 << 0x01ff0000 << 0x00000000 << std_id;
    }
    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        if (start)
            ecl.SetPos(start);
        else
            start = ecl.GetPos();
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }

    constexpr unsigned int st1PostMaple = 0xa750;
    constexpr unsigned int st2PostMaple = 0x90b4;
    constexpr unsigned int st3PostMaple = 0x83a0;
    constexpr unsigned int st4PostMaple = 0x5a04;
    constexpr unsigned int st5PostMaple = 0x96ec;
    constexpr unsigned int st6PostMaple = 0x7e70;
    constexpr unsigned int st7PostMaple = 0xa8fc;
    constexpr unsigned int stdInterruptSize = 0x14;
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, st1PostMaple, 0xab90, 60, 90);
                ECLJump(ecl, 0x6fc8, 0x7088, 0);
                break;
            case 3:
                ECLJump(ecl, st1PostMaple, 0xab90, 60, 90);
                ECLJump(ecl, 0x6fc8, 0x70bc, 0);
                break;
            case 4:
                ECLStdExec(ecl, st1PostMaple, 1, 1);
                ECLJump(ecl, 0, 0xac2c, 60, 90);
                break;
            case 5:
                ECLStdExec(ecl, st1PostMaple, 1, 1);
                ECLJump(ecl, 0, 0xac2c, 60, 90);
                ECLJump(ecl, 0x7184, 0x71f4, 0);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, st2PostMaple, 0x94cc, 60, 90); // 0x9550, 0x9594, 0x9600
                ECLJump(ecl, 0x55e0, 0x5614, 0, 0);
                break;
            case 3:
                ECLJump(ecl, st2PostMaple, 0x94cc, 60, 90); // 0x9550, 0x9594, 0x9600
                ECLJump(ecl, 0x55e0, 0x56a8, 0, 0);
                break;
            case 4:
                ECLStdExec(ecl, st2PostMaple, 1, 1);
                ECLJump(ecl, 0, 0x9594, 60, 90); // 0x9550, 0x9594, 0x9600
                break;
            case 5:
                ECLStdExec(ecl, st2PostMaple, 1, 1);
                ECLJump(ecl, 0, 0x9594, 60, 90); // 0x9550, 0x9594, 0x9600
                ECLJump(ecl, 0x573c, 0x5770, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, st3PostMaple, 0x86ec, 60, 90); // 0x8784, 0x87c8, 0x8848
                ECLJump(ecl, 0x4c34, 0x4c68, 0, 0);
                break;
            case 3:
                ECLJump(ecl, st3PostMaple, 0x86ec, 60, 90); // 0x8784, 0x87c8, 0x8848
                ECLJump(ecl, 0x4c34, 0x4cc8, 0, 0);
                break;
            case 4:
                // ECLStdExec(ecl, 0x83a0, 1, 1);
                ECLStdExec(ecl, st3PostMaple, 0, 1);
                ECLJump(ecl, 0, 0x87c8, 60, 90); // 0x8784, 0x87c8, 0x8848
                break;
            case 5:
                // ECLStdExec(ecl, 0x83a0, 1, 1);
                ECLStdExec(ecl, st3PostMaple, 0, 1);
                ECLJump(ecl, 0, 0x87c8, 60, 90); // 0x8784, 0x87c8, 0x8848
                ECLJump(ecl, 0x4d90, 0x4dd8, 0, 0);
                break;
            case 6:
                // ECLStdExec(ecl, 0x83a0, 1, 1);
                if (thPracParam.phase == 1) {
                    ECLJump(ecl, 0x66D0, 0x6590, 0, 0);
                    ECLJump(ecl, 0x68B0, 0x6770, 0, 0);
                }
                ECLStdExec(ecl, st3PostMaple, 0, 1);
                ECLJump(ecl, 0, 0x87c8, 60, 90); // 0x8784, 0x87c8, 0x8848
                ECLJump(ecl, 0x4d90, 0x4e38, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, st4PostMaple, 0x5d50, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x33cc, 0x3400, 0, 0);
                break;
            case 3:
                ECLJump(ecl, st4PostMaple, 0x5d50, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x33cc, 0x3434, 0, 0);
                break;
            case 4:
                if(thPracParam.phase==1) {
                    ECLJump(ecl, 0x43E4, 0x4364, 0, 0);
                }
                ECLJump(ecl, st4PostMaple, 0x5d50, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x33cc, 0x3468, 0, 0);
                break;
            case 5:
                ECLStdExec(ecl, st4PostMaple, 1, 1);
                ECLJump(ecl, 0, 0x5dd8, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                break;
            case 6:
                ECLStdExec(ecl, st4PostMaple, 1, 1);
                ECLJump(ecl, 0, 0x5dd8, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x34fc, 0x3598, 0, 0);
                break;
            case 7:
                ECLStdExec(ecl, st4PostMaple, 1, 1);
                ECLJump(ecl, 0, 0x5dd8, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x34fc, 0x35cc, 0, 0);
                break;
            case 8:
                ECLStdExec(ecl, st4PostMaple, 1, 1);
                ECLJump(ecl, 0, 0x5dd8, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x34fc, 0x3640, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                ECLJump(ecl, st5PostMaple, 0x9b28, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ecl << pair{0x4c50, 0};
                break;
            case 2:
                ECLJump(ecl, st5PostMaple, 0x9b28, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4c40, 0x4c88, 0, 0);
                break;
            case 3:
                ECLJump(ecl, st5PostMaple, 0x9b28, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4c40, 0x4cbc, 0, 0);
                break;
            case 4:
                ECLJump(ecl, st5PostMaple, 0x9b28, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4c40, 0x4cf0, 0, 0);
                break;
            case 5:
                if (thPracParam.phase == 1) {
                    ECLJump(ecl, 0x5D7C, 0x5CDC, 0, 0);
                }
                ECLJump(ecl, st5PostMaple, 0x9bd8, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ecl << pair{0x4d94, 0};
                break;
            case 6:
                ECLJump(ecl, st5PostMaple, 0x9bd8, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4d84, 0x4df8, 0, 0);
                break;
            case 7:
                ECLStdExec(ecl, st5PostMaple, 1, 1);
                ECLJump(ecl, 0, 0x9bd8, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4d84, 0x4e2c, 0, 0);
                break;
            case 8:
                if (thPracParam.phase == 1) {
                    ECLJump(ecl, 0x675C, 0x6630, 0, 0);
                }
                ECLStdExec(ecl, st5PostMaple, 1, 1);
                ECLJump(ecl, 0, 0x9bd8, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4d84, 0x4e8c, 0, 0);
                break;
            case 9:
                ECLStdExec(ecl, st5PostMaple, 1, 1);
                ECLJump(ecl, 0, 0x9bd8, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4d84, 0x4ec0, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                ECLJump(ecl, st6PostMaple, 0x82ac, 60, 90); // 0x8318, 0x835c, 0x83dc
                ecl << pair{0x5ca0, 0};
                break;
            case 2:
                ECLJump(ecl, st6PostMaple, 0x82ac, 60, 90); // 0x8318, 0x835c, 0x83dc
                ECLJump(ecl, 0x5c90, 0x5d0c, 0, 0);
                break;
            case 3:
                ECLJump(ecl, st6PostMaple, 0x82ac, 60, 90); // 0x8318, 0x835c, 0x83dc
                ECLJump(ecl, 0x5c90, 0x5d74, 0, 0);
                break;
            case 4:
                ECLStdExec(ecl, st6PostMaple, 1, 1);
                ECLJump(ecl, 0, 0x835c, 60, 90); // 0x8318, 0x835c, 0x83dc
                ECLJump(ecl, 0x5e3c, 0x5e7c, 0, 0);
                break;
            case 5:
                ECLStdExec(ecl, st6PostMaple, 1, 1);
                ECLJump(ecl, 0, 0x835c, 60, 90); // 0x8318, 0x835c, 0x83dc
                ECLJump(ecl, 0x5e3c, 0x5eb0, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                ECLJump(ecl, st7PostMaple, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ecl << pair{0x5a18, 0};
                break;
            case 2:
                ECLJump(ecl, st7PostMaple, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5a08, 0x5a50, 0, 0);
                break;
            case 3:
                ECLJump(ecl, st7PostMaple, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5a08, 0x5a84, 0, 0);
                break;
            case 4:
                ECLJump(ecl, st7PostMaple, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5a08, 0x5ab8, 0, 0);
                break;
            case 5:
                ECLJump(ecl, st7PostMaple, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5a08, 0x5b18, 0, 0);
                break;
            case 6:
                ECLJump(ecl, st7PostMaple, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5a08, 0x5b4c, 0, 0);
                break;
            case 7:
                ECLStdExec(ecl, st7PostMaple, 1, 1);
                ECLJump(ecl, 0, 0xafe4, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ecl << pair{0x5bd4, 0};
                break;
            case 8:
                ECLStdExec(ecl, st7PostMaple, 1, 1);
                ECLJump(ecl, 0, 0xafe4, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5bb4, 0x5c38, 0, 0);
                break;
            case 9:
                ECLStdExec(ecl, st7PostMaple, 1, 1);
                ECLJump(ecl, 0, 0xafe4, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5bb4, 0x5c6c, 0, 0);
                break;
            case 10:
                ECLStdExec(ecl, st7PostMaple, 1, 1);
                ECLJump(ecl, 0, 0xafe4, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5bb4, 0x5ccc, 0, 0);
                break;
            case 11:
                ECLStdExec(ecl, st7PostMaple, 1, 1);
                ECLJump(ecl, 0, 0xafe4, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5bb4, 0x5d00, 0, 0);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        constexpr unsigned int st7BossCreateCall = 0xb064;
        constexpr unsigned int st7EndPreDialogue = 0x57cc;
        constexpr unsigned int st7EndPostDialogue = 0x585c;
        constexpr unsigned int st7bsPreDialogue = 0x718;
        constexpr unsigned int st7bsPostDialogue = 0x758;
        constexpr unsigned int st7bsPrePushSpellID = 0x80c;
        constexpr unsigned int st7bsNonSubCallOrd = 0x1174 + 0x18;
        constexpr unsigned int st7BossSpawnY = 0x56c + 0x14;

        switch (section) {
        case THPrac::TH18::TH18_ST1_MID1:
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, 0, 0xabe8, 60);
            break;
        case THPrac::TH18::TH18_ST1_BOSS1:
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0xacac, 60);
            else
                ECLJump(ecl, 0, 0xacc0, 60);
            break;
        case THPrac::TH18::TH18_ST1_BOSS2:
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, 0, 0xacc0, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3e4, 0x4cc, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4dc, 2100}; // Set Health
            ecl << pair{0x4fc, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST1_BOSS3:
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, 0, 0xacc0, 60);
            ecl.SetFile(2);
            ecl << pair{0x61c, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1350, (int16_t)0} << pair{0x147c, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1270, (int16_t)0}; // Disable Invincible
            ECLJump(ecl, 0x15d0, 0x1650, 0); // Skip wait
            break;
        case THPrac::TH18::TH18_ST1_BOSS4:
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, 0, 0xacc0, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3e4, 0x4cc, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4dc, 2200}; // Set Health
            ecl << pair{0x4fc, (int8_t)0x32}; // Set Spell Ordinal
            break;


        case THPrac::TH18::TH18_ST2_MID1:
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x9550, 60); // 0x9550, 0x9594, 0x9600
            break;
        case THPrac::TH18::TH18_ST2_BOSS1:
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x95ec, 60);
            else
                ECLJump(ecl, 0, 0x9600, 60); // 0x9550, 0x9594, 0x9600
            break;
        case THPrac::TH18::TH18_ST2_BOSS2:
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x9600, 60); // 0x9550, 0x9594, 0x9600

            ecl.SetFile(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2300}; // Set Health
            ecl << pair{0x524, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST2_BOSS3:
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x9600, 60); // 0x9550, 0x9594, 0x9600

            ecl.SetFile(2);
            ecl << pair{0x748, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x113c, (int16_t)0} << pair{0x1268, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x105c, (int16_t)0}; // Disable Invincible
            ECLJump(ecl, 0x13bc, 0x13fc, 0); // Skip wait
            break;
        case THPrac::TH18::TH18_ST2_BOSS4:
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x9600, 60); // 0x9550, 0x9594, 0x9600

            ecl.SetFile(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2300}; // Set Health
            ecl << pair{0x524, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST2_BOSS5:
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x9600, 60); // 0x9550, 0x9594, 0x9600

            ecl.SetFile(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2700}; // Set Health
            ecl << pair{0x524, (int8_t)0x33}; // Set Spell Ordinal
            break;


        case THPrac::TH18::TH18_ST3_MID1:
            //ECLStdExec(ecl, 0x83a0, 1, 1);
            ECLStdExec(ecl, st3PostMaple, 0, 1);
            ECLJump(ecl, 0, 0x8784, 60); // 0x8784, 0x87c8, 0x8848
            ecl.SetFile(3);
            ecl << pair{0x324, (int16_t)100};
            ecl.SetPos(0x400);
            ecl << 0 << 0x001401ff << 0x01ff0000 << 0 << 13000;
            ECLJump(ecl, 0, 0x47c, 0);
            break;
        case THPrac::TH18::TH18_ST3_BOSS1:
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x8834, 60);
            else
                ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848
            break;
        case THPrac::TH18::TH18_ST3_BOSS2:
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848

            ecl.SetFile(2);
            ECLJump(ecl, 0x46c, 0x554, 1); // Utilize Spell Practice Jump
            ecl << pair{0x564, 2800}; // Set Health
            ecl << pair{0x584, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST3_BOSS3:
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848

            ecl.SetFile(2);
            ecl << pair{0x7c8, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x14cc, (int16_t)30}; // Disable Invincible
            ecl << pair{0x15ac, (int16_t)0} << pair{0x16d8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x183c, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST3_BOSS4:
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848

            ecl.SetFile(2);
            ECLJump(ecl, 0x46c, 0x554, 1); // Utilize Spell Practice Jump
            ecl << pair{0x564, 2800}; // Set Health
            ecl << pair{0x584, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST3_BOSS5:
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848

            ecl.SetFile(2);
            ecl << pair{0x7c8, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x24f4, (int16_t)30}; // Disable Invincible
            ecl << pair{0x25d4, (int16_t)0} << pair{0x2700, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2864, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST3_BOSS6:
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848

            ecl.SetFile(2);
            ECLJump(ecl, 0x46c, 0x554, 1); // Utilize Spell Practice Jump
            ecl << pair{0x564, 2600}; // Set Health
            ecl << pair{0x584, (int8_t)0x33}; // Set Spell Ordinal
            break;


        case THPrac::TH18::TH18_ST4_MID1:
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x5d94, 60); // 0x5d94, 0x5dd8, 0x5e58
            break;
        case THPrac::TH18::TH18_ST4_BOSS1:
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x5e44, 60); // 0x5d94, 0x5dd8, 0x5e58
            else
                ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58
            break;
        case THPrac::TH18::TH18_ST4_BOSS2:
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58

            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4f4, 3400}; // Set Health
            ecl << pair{0x514, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST4_BOSS3:
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58

            ecl.SetFile(2);
            ecl << pair{0x758, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x12b4, (int16_t)30}; // Disable Invincible
            ecl << pair{0x1394, (int16_t)0} << pair{0x14c0, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1624, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST4_BOSS4:
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58

            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4f4, 3400}; // Set Health
            ecl << pair{0x514, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST4_BOSS5:
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58

            ecl.SetFile(2);
            ecl << pair{0x758, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x20e0, (int16_t)30}; // Disable Invincible
            ecl << pair{0x21c0, (int16_t)0} << pair{0x22ec, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2450, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST4_BOSS6:
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58

            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4f4, 4800}; // Set Health
            ecl << pair{0x514, (int8_t)0x33}; // Set Spell Ordinal
            break;


        case THPrac::TH18::TH18_ST5_MID1:
            ECLJump(ecl, st5PostMaple, 0x9b94, 60); // 0x9b94, 0x9bd8, 0x9c58
            ecl.SetFile(3);
            ecl << pair{0x324, (int16_t)100};
            ecl.SetPos(0x3ac);
            ecl << 0 << 0x002401ff << 0x01ff0000 << 0 << 13000;
            break;
        case THPrac::TH18::TH18_ST5_BOSS1:
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x9c44, 60); // 0x9b94, 0x9bd8, 0x9c58
            else {
                ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4a44, 0x4a94, 3);
            }
            break;
        case THPrac::TH18::TH18_ST5_BOSS2:
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x430, 0x518, 1); // Utilize Spell Practice Jump
            ecl << pair{0x528, 3200}; // Set Health
            ecl << pair{0x548, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST5_BOSS3:
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ecl << pair{0x890, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x10b0, (int16_t)30}; // Disable Invincible
            ecl << pair{0x1190, (int16_t)0} << pair{0x12bc, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x13f4, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST5_BOSS4:
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x430, 0x518, 1); // Utilize Spell Practice Jump
            ecl << pair{0x528, 3200}; // Set Health
            ecl << pair{0x548, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST5_BOSS5:
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ecl << pair{0x890, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x1b9c, (int16_t)30}; // Disable Invincible
            ecl << pair{0x1c7c, (int16_t)0} << pair{0x1da8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1f0c, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST5_BOSS6:
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x430, 0x518, 1); // Utilize Spell Practice Jump
            ecl << pair{0x528, 3000}; // Set Health
            ecl << pair{0x548, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST5_BOSS7:
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x430, 0x518, 1); // Utilize Spell Practice Jump
            ecl << pair{0x528, 4500}; // Set Health
            ecl << pair{0x548, (int8_t)0x34}; // Set Spell Ordinal
            break;


        case THPrac::TH18::TH18_ST6_MID1:
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, 0, 0x8318, 60); // 0x8318, 0x835c, 0x83dc
            ecl.SetFile(3);
            ecl << pair{0x324, (int16_t)100};
            ecl.SetPos(0x3ac);
            ecl << 0 << 0x002401ff << 0x01ff0000 << 0 << 14000;
            break;
        case THPrac::TH18::TH18_ST6_BOSS1:
            ECLStdExec(ecl, st6PostMaple, 2, 1);
            if (thPracParam.dlg) {
                ECLJump(ecl, 0, 0x83c8, 60);
            } else {
                ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
                ECLJump(ecl, 0x5a94, 0x5ae4, 3); // Skip dialogue
                ecl.SetFile(2);
                ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            }
            break;
        case THPrac::TH18::TH18_ST6_BOSS2:
            ECLStdExec(ecl, st6PostMaple, 2, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ECLJump(ecl, 0x5d8, 0x6c0, 2); // Utilize Spell Practice Jump
            ecl << pair{0x6d0, 2000}; // Set Health
            ecl << pair{0x6f0, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST6_BOSS3:
            ECLStdExec(ecl, st6PostMaple, 2, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ecl << pair{0xc64, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1db0, (int16_t)30}; // Disable Invincible
            ecl << pair{0x1e70, (int16_t)0} << pair{0x1ec8, (int16_t)0} << pair{0x2008, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x216c, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST6_BOSS4:
            ECLStdExec(ecl, st6PostMaple, 2, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ECLJump(ecl, 0x5d8, 0x6c0, 2); // Utilize Spell Practice Jump
            ecl << pair{0x6d0, 4500}; // Set Health
            ecl << pair{0x6f0, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST6_BOSS5:
            ECLStdExec(ecl, st6PostMaple, 3, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ecl << pair{0x25e8, (int16_t)0};
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ecl << pair{0xc64, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x2528, (int16_t)30}; // Disable Invincible
            ecl << pair{0x261c, (int16_t)0} << pair{0x2760, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x28c4, (int16_t)0};
            ecl << pair{0x370, 176.0f};
            break;
        case THPrac::TH18::TH18_ST6_BOSS6:
            ECLStdExec(ecl, st6PostMaple, 3, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ECLJump(ecl, 0x5d8, 0x6c0, 2); // Utilize Spell Practice Jump
            ecl << pair{0x6d0, 3000}; // Set Health
            ecl << pair{0x6f0, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST6_BOSS7:
            ECLStdExec(ecl, st6PostMaple, 4, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ecl << pair{0x2d70, (int16_t)0};
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ecl << pair{0xc64, (int8_t)0x34}; // Change Nonspell
            ecl << pair{0x2cb0, (int16_t)30}; // Disable Invincible
            ecl << pair{0x2da4, (int16_t)0} << pair{0x2ee8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x304c, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST6_BOSS8:
            ECLStdExec(ecl, st6PostMaple, 4, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ECLJump(ecl, 0x5d8, 0x6c0, 2); // Utilize Spell Practice Jump
            ecl << pair{0x6d0, 2700}; // Set Health
            ecl << pair{0x6f0, (int8_t)0x34}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST6_BOSS9:
            ECLStdExec(ecl, st6PostMaple, 5, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ECLJump(ecl, 0x5d8, 0x6c0, 2); // Utilize Spell Practice Jump
            ecl << pair{0x6d0, 3500}; // Set Health
            ecl << pair{0x6f0, (int8_t)0x35}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST6_BOSS10:
            ECLStdExec(ecl, st6PostMaple, 5, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ECLJump(ecl, 0x5d8, 0x6c0, 2); // Utilize Spell Practice Jump
            ecl << pair{0x6d0, 7000}; // Set Health
            ecl << pair{0x6f0, (int8_t)0x36}; // Set Spell Ordinal

            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0x6d0, 5000};
                ECLJump(ecl, 0x99c4, 0x9b64, 120, 122);
                break;
            case 2:
                ecl << pair{0x6d0, 3500};
                ECLJump(ecl, 0x99c4, 0x9d40, 120, 122);
                break;
            case 3:
                ecl << pair{0x6d0, 1700};
                ECLJump(ecl, 0x99c4, 0x9d40, 120, 122);
                ECLJump(ecl, 0x9dd0, 0x9ffc, 120, 0);
                break;
            default:
                break;
            }
            break;

        case THPrac::TH18::TH18_ST7_MID1:
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0xaf78, 124); // 0xafa0, 0xafe4, 0xb064
            else
                ECLJump(ecl, 0, 0xafa0, 124); // 0xafa0, 0xafe4, 0xb064
            ecl.SetFile(3);
            ecl << pair{0x2d0, 99999} << pair{0x2d8, (int16_t)0} << pair{0x368, 90};
            ecl.SetPos(0x388);
            ecl << 0 << 0x00140017 << 0x01ff0000 << 0 << 90;
            ECLJump(ecl, 0, 0x428, 0);
            break;
        case THPrac::TH18::TH18_ST7_MID2:
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, 0xafa0, 124); // 0xafa0, 0xafe4, 0xb064
            ecl.SetFile(3);
            ecl << pair{0x2d0, 99999} << pair{0x2d8, (int16_t)0} << pair{0x368, 90};
            ecl.SetPos(0x388);
            ecl << 0 << 0x00140017 << 0x01ff0000 << 0 << 90;
            ECLJump(ecl, 0, 0x4c4, 0);
            break;
        case THPrac::TH18::TH18_ST7_MID3:
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, 0xafa0, 124); // 0xafa0, 0xafe4, 0xb064
            ecl.SetFile(3);
            ecl << pair{0x2d0, 99999} << pair{0x2d8, (int16_t)0} << pair{0x368, 90};
            ecl.SetPos(0x388);
            ecl << 0 << 0x00140017 << 0x01ff0000 << 0 << 90;
            ECLJump(ecl, 0, 0x560, 0);
            break;
        case THPrac::TH18::TH18_ST7_END_NS1: {
            constexpr unsigned int st7DialogRead = 0xb050;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, st7DialogRead, 124); // 0xafa0, 0xafe4, 0xb064
            else {
                ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);
                ecl.SetFile(2);
                ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            }
            break;
        }
        case THPrac::TH18::TH18_ST7_END_S1: {
            constexpr unsigned int st7bsSpell1SPLifeSet = 0x8ac;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ECLJump(ecl, st7bsPrePushSpellID, st7bsSpell1SPLifeSet, 9); // Utilize Spell Practice Jump
            break;
        }

        case THPrac::TH18::TH18_ST7_END_NS2: {
            constexpr unsigned int st7bsNon2BossItemCallOp = 0x1cb8 + 0x4;
            constexpr unsigned int st7bsNon2PlaySoundOp = 0x1de4 + 0x4;
            constexpr unsigned int st7bsSpell1Bounds = 0x60e0;
            constexpr unsigned int st7bsSpell1PostBounds = 0x6100;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsSpell1Bounds, 2); // Skip dialogue while setting
            ECLJump(ecl, st7bsSpell1PostBounds, st7bsPostDialogue, 9); // movement bounds from Spell1
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x32 } // Change Nonspell (2)
                << pair { st7bsNon2BossItemCallOp, (int16_t)0 } // Disable item drops
                << pair { st7bsNon2PlaySoundOp, (int16_t)0 }; // Disable drop SFX
            break;
        }
        case THPrac::TH18::TH18_ST7_END_S2: {
            constexpr unsigned int st7bsSpell2SPLifeSet = 0x948;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ECLJump(ecl, st7bsPrePushSpellID, st7bsSpell2SPLifeSet, 9); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH18::TH18_ST7_END_NS3: {
            constexpr unsigned int st7bsNon3InvulnVal = 0x23f4 + 0x10;
            constexpr unsigned int st7bsNon3BossItemCallOp = 0x24d8 + 0x4;
            constexpr unsigned int st7bsNon3PlaySoundOp = 0x2604 + 0x4;
            constexpr unsigned int st7bsNon3InterruptTimeVal = 0x2714 + 0x18;
            constexpr unsigned int st7bsNon3PreWait = 0x2770;
            constexpr unsigned int st7bsNon3PostWait = 0x27a4;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x33 } // Change Nonspell (3)
                << pair { st7bsNon3InvulnVal, (int16_t)0 } // Disable iframes
                << pair { st7bsNon3BossItemCallOp, (int16_t)0 } // Disable item drops
                << pair { st7bsNon3PlaySoundOp, (int16_t)0 } // Disable drop SFX
                << pair { st7bsNon3InterruptTimeVal, (int16_t)(3600 - 120) }; // Reduce timer by wait time skipped
            ECLJump(ecl, st7bsNon3PreWait, st7bsNon3PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH18::TH18_ST7_END_S3: {
            constexpr unsigned int st7bsSpell3SPLifeSet = 0x9e4;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ECLJump(ecl, st7bsPrePushSpellID, st7bsSpell3SPLifeSet, 9); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH18::TH18_ST7_END_NS4: {
            constexpr unsigned int st7bsNon4InvulnVal = 0x2c7c + 0x10;
            constexpr unsigned int st7bsNon4BossItemCallOp = 0x2d60 + 0x4;
            constexpr unsigned int st7bsNon4PlaySoundOp = 0x2e8c + 0x4;
            constexpr unsigned int st7bsNon4InterruptTimeVal = 0x2f9c + 0x18;
            constexpr unsigned int st7bsNon4PreWait = 0x2ff8;
            constexpr unsigned int st7bsNon4PostWait = 0x303c;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x34 } // Change Nonspell (4)
                << pair { st7bsNon4InvulnVal, (int16_t)0 } // Disable iframes
                << pair { st7bsNon4BossItemCallOp, (int16_t)0 } // Disable item drops
                << pair { st7bsNon4PlaySoundOp, (int16_t)0 } // Disable drop SFX
                << pair { st7bsNon4InterruptTimeVal, (int16_t)(3600 - 120) }; // Reduce timer by wait time skipped
            ECLJump(ecl, st7bsNon4PreWait, st7bsNon4PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH18::TH18_ST7_END_S4: {
            constexpr unsigned int st7bsSpell4SPLifeSet = 0xa80;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ECLJump(ecl, st7bsPrePushSpellID, st7bsSpell4SPLifeSet, 9); // Utilize Spell Practice Jump

            break;
        }
        case THPrac::TH18::TH18_ST7_END_NS5: {
            constexpr unsigned int st7bsNon5InvulnVal = 0x3514 + 0x10;
            constexpr unsigned int st7bsNon5BossItemCallOp = 0x35f8 + 0x4;
            constexpr unsigned int st7bsNon5PlaySoundOp = 0x3724 + 0x4;
            constexpr unsigned int st7bsNon5InterruptTimeVal = 0x3834 + 0x18;
            constexpr unsigned int st7bsNon5PreWait = 0x3890;
            constexpr unsigned int st7bsNon5PostWait = 0x38c4;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ecl << pair { st7BossSpawnY, 96.0f } // Adjust boss spawn Y
                << pair { st7bsNonSubCallOrd, (int8_t)0x35 } // Change Nonspell (5)
                << pair { st7bsNon5InvulnVal, (int16_t)0 } // Disable iframes
                << pair { st7bsNon5BossItemCallOp, (int16_t)0 } // Disable item drops
                << pair { st7bsNon5PlaySoundOp, (int16_t)0 } // Disable drop SFX
                << pair { st7bsNon5InterruptTimeVal, (int16_t)(4200 - 120) }; // Reduce timer by wait time skipped
            ECLJump(ecl, st7bsNon5PreWait, st7bsNon5PostWait, 0); // Skip wait (120f)
            break;
        }
        case THPrac::TH18::TH18_ST7_END_S5: {
            constexpr unsigned int st7bsNon5Bounds = 0x38c4;
            constexpr unsigned int st7bsNon5PostBounds = 0x38e4;
            constexpr unsigned int st7bsSpell5SPLifeSet = 0xb1c;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsNon5Bounds, 2); // Skip dialogue while setting
            ECLJump(ecl, st7bsNon5PostBounds, st7bsPostDialogue, 9); // movement bounds from Non5
            ECLJump(ecl, st7bsPrePushSpellID, st7bsSpell5SPLifeSet, 9); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH18::TH18_ST7_END_NS6: {
            constexpr unsigned int st7bsNon6InvulnVal = 0x3d9c + 0x10;
            constexpr unsigned int st7bsNon6BossItemCallOp = 0x3eb0 + 0x4;
            constexpr unsigned int st7bsNon6PlaySoundOp = 0x3fdc + 0x4;
            constexpr unsigned int st7bsNon6InterruptTimeVal = 0x40ec + 0x18;
            constexpr unsigned int st7bsNon6PreWait = 0x4158;
            constexpr unsigned int st7bsNon6PostWait = 0x418c;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x36 } // Change Nonspell (6)
                << pair { st7bsNon6InvulnVal, (int16_t)0 } // Disable iframes
                << pair { st7bsNon6BossItemCallOp, (int16_t)0 } // Disable item drops
                << pair { st7bsNon6PlaySoundOp, (int16_t)0 } // Disable drop SFX
                << pair { st7bsNon6InterruptTimeVal, (int16_t)(4200 - 120) }; // Reduce timer by wait time skipped
            ECLJump(ecl, st7bsNon6PreWait, st7bsNon6PostWait, 0); // Skip wait (120f)
            break;
        }
        case THPrac::TH18::TH18_ST7_END_S6: {
            constexpr unsigned int st7bsSpell6SPLifeSet = 0xbb8;
            constexpr unsigned int st7bsSpell6FloatInterpTime = 0xc678 + 0x18;
            constexpr unsigned int st7bsSpell6FloatInterpInit = 0xc678 + 0x20;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ECLJump(ecl, st7bsPrePushSpellID, st7bsSpell6SPLifeSet, 9); // Utilize Spell Practice Jump

            switch (thPracParam.phase) {
            case 1:
                ecl << pair { st7bsSpell6FloatInterpTime, 1 }
                    << pair { st7bsSpell6FloatInterpInit, 60.0f }; // Already start at min. delay
                break;
            default:
                break;
            }
            break;
        }
        case THPrac::TH18::TH18_ST7_END_NS7: {
            constexpr unsigned int st7bsNon7InvulnVal = 0x4664 + 0x10;
            constexpr unsigned int st7bsNon7BossItemCallOp = 0x4748 + 0x4;
            constexpr unsigned int st7bsNon7PlaySoundOp = 0x4874 + 0x4;
            constexpr unsigned int st7bsNon7InterruptTimeVal = 0x4984 + 0x18;
            constexpr unsigned int st7bsNon7PreWait = 0x49e0;
            constexpr unsigned int st7bsNon7PostWait = 0x4a24;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x37 } // Change Nonspell (7)
                << pair { st7bsNon7InvulnVal, (int16_t)0 } // Disable iframes
                << pair { st7bsNon7BossItemCallOp, (int16_t)0 } // Disable item drops
                << pair { st7bsNon7PlaySoundOp, (int16_t)0 } // Disable drop SFX
                << pair { st7bsNon7InterruptTimeVal, (int16_t)(4200 - 120) }; // Reduce timer by wait time skipped
            ECLJump(ecl, st7bsNon7PreWait, st7bsNon7PostWait, 0); // Skip wait (120f)
            break;
        }
        case THPrac::TH18::TH18_ST7_END_S7: {
            constexpr unsigned int st7bsSpell7SPLifeSet = 0xc54;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ECLJump(ecl, st7bsPrePushSpellID, st7bsSpell7SPLifeSet, 9); // Utilize Spell Practice Jump

            break;
        }
        case THPrac::TH18::TH18_ST7_END_NS8: {
            constexpr unsigned int st7bsNon8InvulnVal = 0x4efc + 0x10;
            constexpr unsigned int st7bsNon8BossItemCallOp = 0x4fe0 + 0x4;
            constexpr unsigned int st7bsNon8PlaySoundOp = 0x510c + 0x4;
            constexpr unsigned int st7bsNon8InterruptTimeVal = 0x521c + 0x18;
            constexpr unsigned int st7bsNon8PreWait = 0x5278;
            constexpr unsigned int st7bsNon8PostWait = 0x52bc;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x38 } // Change Nonspell (8)
                << pair { st7bsNon8InvulnVal, (int16_t)0 } // Disable iframes
                << pair { st7bsNon8BossItemCallOp, (int16_t)0 } // Disable item drops
                << pair { st7bsNon8PlaySoundOp, (int16_t)0 } // Disable drop SFX
                << pair { st7bsNon8InterruptTimeVal, (int16_t)(4200 - 120) }; // Reduce timer by wait time skipped
            ECLJump(ecl, st7bsNon8PreWait, st7bsNon8PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH18::TH18_ST7_END_S8: {
            constexpr unsigned int st7bsSpell8SPLifeSet = 0xcf0;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ECLJump(ecl, st7bsPrePushSpellID, st7bsSpell8SPLifeSet, 9); // Utilize Spell Practice Jump

            break;
        }
        case THPrac::TH18::TH18_ST7_END_S9: {
            constexpr unsigned int st7bsSpell9SPLifeSet = 0xd8c;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsPostDialogue, 9); // Skip dialogue
            ECLJump(ecl, st7bsPrePushSpellID, st7bsSpell9SPLifeSet, 9); // Utilize Spell Practice Jump

            break;
        }
        case THPrac::TH18::TH18_ST7_END_S10: {
            constexpr unsigned int st7bsHurtbox = 0x76c;
            constexpr unsigned int st7bsPostHurtbox = 0x784;
            constexpr unsigned int st7bsSpell8Hurtbox = 0xf908;
            constexpr unsigned int st7bsSpell8PostHurtbox = 0xf934;
            constexpr unsigned int st7bsSpell10SPLifeSet = 0xe28;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, 0, st7BossCreateCall, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, st7EndPreDialogue, st7EndPostDialogue, 3);

            ecl.SetFile(2);
            ECLJump(ecl, st7bsPreDialogue, st7bsSpell8Hurtbox, 2); // Skip dialogue
            ECLJump(ecl, st7bsSpell8PostHurtbox, st7bsPostDialogue, 9); // and grab the Dragon Eater hurtbox
            ECLJump(ecl, st7bsHurtbox, st7bsPostHurtbox, 9); // Avoid overwriting hurtbox (while still setting chapter 43 just in case)
            ECLJump(ecl, st7bsPrePushSpellID, st7bsSpell10SPLifeSet, 9); // Utilize Spell Practice Jump

            switch (thPracParam.phase) {
            case 1:
                ecl << pair { st7bsSpell10SPLifeSet + 0x10, 9000 };
                ECLJump(ecl, 0x68f4, 0x69a0, 90);
                break;
            case 2:
                ecl << pair { st7bsSpell10SPLifeSet + 0x10, 6500 };
                ECLJump(ecl, 0x68f4, 0x6a4c, 90);
                break;
            case 3:
                ecl << pair { st7bsSpell10SPLifeSet + 0x10, 4000 };
                ECLJump(ecl, 0x68cc, 0x6b40, 90);
                break;
            case 4:
                ECLJump(ecl, 0x68cc, 0x6b40, 90);
                ECLJump(ecl, 0x6b6c, 0x6c18, 90);
                break;
            default:
                break;
            }
            break;
        }
        default:
            break;
        }
    }
    __declspec(noinline) void THPatchSP(ECLHelper& ecl)
    {
        uint32_t spell_id = *(int32_t*)(0x4b5a14);
        switch (spell_id) {
        case 84:
        case 85:
        case 86:
        case 87:
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xb98, 7000};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                break;
            case 2:
                ecl << pair{0xb98, 4500};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                ECLJump(ecl, 0xf2d0, 0xf448, 120);
                break;
            case 3:
                ecl << pair{0xb98, 2500};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                ECLJump(ecl, 0xf2d0, 0xf448, 120);
                ECLJump(ecl, 0xf508, 0xf680, 120);
                break;
            default:
                break;
            }
            break;
        case 100:
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xe2c, 7000};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                break;
            case 2:
                ecl << pair{0xe2c, 4500};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                break;
            case 3:
                ecl << pair{0xe2c, 2000};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                ECLJump(ecl, 0x6e38, 0x6e80, 90);
                break;
            case 4:
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                ECLJump(ecl, 0x6e38, 0x6e80, 90);
                ECLJump(ecl, 0x6ec8, 0x6f10, 90);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    __declspec(noinline) void THSectionPatch()
    {
        ECLHelper ecl;
        ecl.SetBaseAddr((void*)GetMemAddr(0x4cf2d0, 0x188, 0xc));

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
        else if (thPracParam.dlg)
            return 0;
        else
            return th_sections_bgm[thPracParam.section];
    }
    void THSaveReplay(char* repName)
    {
        ReplaySaveParam(utf8_to_utf16(repName).c_str(), thPracParam.GetJson());
    }

    static void RenderLockTimer(ImDrawList* p)
    {
        if (g_lock_timer_flag) {
            g_lock_timer++;
            g_lock_timer_flag = false;
        }

        if (g_adv_igi_options.enable_lock_timer_autoly && *THOverlay::singleton().mTimeLock) {
            std::string time_text = std::format("{:.2f}", (float)g_lock_timer / 60.0f);
            auto sz = ImGui::CalcTextSize(time_text.c_str());
            p->AddRectFilled({ 64.0f, 0.0f }, { 220.0f, sz.y }, 0xFFFFFFFF);
            p->AddText({ 220.0f - sz.x, 0.0f }, 0xFF000000, time_text.c_str());
        }
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th18_inf_lives, 0x0045D1A0,3,
    {
        if ((*(THOverlay::singleton().mInfLives))) {
            if (!g_adv_igi_options.map_inf_life_to_no_continue) {
                pCtx->Eax++;
            } else {
                if (*(DWORD*)(0x4CCD48) == 0)
                    pCtx->Eax++;
            }
        }
    })
    EHOOK_DY(th18_everlasting_bgm, 0x477a50, 1, {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        // 4th stack item = i32 call_addr

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && (thPracParam.mode == 1) && thPracParam.section;
        is_practice = (*((int32_t*)MODEFLAGS) & 0x1);
        result = ElBgmTest<0x4546d3, 0x443762, 0x45873a, 0x45a24e, 0xffffffff>(
            el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, 0xffffffff);

        if (result) {
            pCtx->Eip = 0x477ae6;
        }
    })
    EHOOK_DY(th18_param_reset, 0x465abd, 6, {
        thPracParam.Reset();
        *(uint32_t*)GetMemAddr(0x4cf41c, 0x5f680) = 10;
    })
    EHOOK_DY(th18_prac_menu_1, 0x4673a2, 5, {
        THGuiPrac::singleton().State(1);
    })
    EHOOK_DY(th18_prac_menu_2, 0x4673c5, 3, {
        THGuiPrac::singleton().State(2);
    })
    EHOOK_DY(th18_prac_menu_3, 0x4675f6, 5, {
        THGuiPrac::singleton().State(3);
    })
    EHOOK_DY(th18_prac_menu_4, 0x4676da, 7, {
        THGuiPrac::singleton().State(4);
    })
    PATCH_DY(th18_prac_menu_enter_1, 0x467488, "eb")
    EHOOK_DY(th18_prac_menu_enter_2, 0x46767a, 1, {
        pCtx->Ecx = thPracParam.stage;
    })
    PATCH_DY(th18_disable_prac_menu_1, 0x46789b, "e9b90000009090")
    // Specifying the hook struct manually because code passed to a macro can't declare a macro
    { .addr = 0x4432a7, .name = "th18_patch_main", .callback = []([[maybe_unused]] PCONTEXT pCtx, [[maybe_unused]] HookCtx* self) {
         defer({
             // THAdvOptWnd::singleton().RestartResetMarket();
             thPracParam._playLock = true;
         });

         if (thPracParam.mode != 1)
             return;

#define R(name)                                                                                                                 \
    card->_recharge_timer.current = static_cast<int32_t>(card->recharge_time * (static_cast<float>(thPracParam.name) / 10000)); \
    card->_recharge_timer.current_f = card->recharge_time * (thPracParam.name / 10000.0f)

         *(int32_t*)(SCORE) = (int32_t)(thPracParam.score / 10);
         *(int32_t*)(0x4ccd48) = thPracParam.life;
         *(int32_t*)(0x4ccd4c) = thPracParam.life_fragment;
         *(int32_t*)(0x4ccd58) = thPracParam.bomb;
         *(int32_t*)(0x4ccd5c) = thPracParam.bomb_fragment;
         *(int32_t*)(0x4ccd38) = thPracParam.power;
         *(int32_t*)(0x4ccd30) = *(int32_t*)(0x4ccd34) = thPracParam.funds;

         auto* ability_manager = *(AbilityManager**)ABILITY_MANAGER_PTR;

         for (ThList<CardBase>* entry = &ability_manager->card_list_head; entry; entry = entry->next) {
             CardBase* card = entry->entry;
             if (!GameState_Assert(card != nullptr))
                 continue;

             switch (card->card_id) {
             case KOZUCHI:
                 R(kozuchi);
                 break;
             case KANAME:
                 R(kaname);
                 break;
             case MOON:
                 R(moon);
                 break;
             case MIKOFLASH:
                 R(mikoflash);
                 break;
             case VAMPIRE:
                 R(vampire);
                 break;
             case SUN:
                 R(sun);
                 break;
             case LILY:
                 if (thPracParam.lily_cycle == -1) { // backwards compatibility with old buggy behavior
                     ((CardLily*)card)->count = 10000 - thPracParam.lily_count;
                 } else {
                     ((CardLily*)card)->count = thPracParam.lily_count;

                     if (thPracParam.lily_count >= 10)
                         ((CardLily*)card)->count += thPracParam.lily_cycle + 2;
                 }
                 R(lily_cd);
                 break;
             case BASSDRUM:
                 R(bassdrum);
                 break;
             case PSYCO:
                 R(psyco);
                 break;
             case CYLINDER:
                 R(cylinder);
                 break;
             case RICEBALL:
                 R(riceball);
                 break;
             case MUKADE:
                 *(int32_t*)MUKADE_ADDR = thPracParam.mukade;
                 break;
             }
         }
         THSectionPatch();
#undef R
     },
        .data = PatchHookImpl(1) },
    EHOOK_DY(th18_bgm, 0x444370, 2, {
            if (THBGMTest()) {
                PushHelper32(pCtx, 1);
                pCtx->Eip = 0x444372;
            }
        })
    EHOOK_DY(th18_menu_rank_fix, 0x45a208, 5, {
        *((int32_t*)0x4ccd00) = *((int32_t*)0x4c9ab0); // Restore In-game rank to menu rank
    })
    EHOOK_DY(th18_restart, 0x4594b7, 2, {
        auto s1 = pCtx->Esp + 0xc;
        auto s2 = pCtx->Edi + 0x1e4;
        auto s3 = *(DWORD*)(pCtx->Edi + 0x1e8);

        asm_call<0x476be0, Stdcall>(0x7, pCtx->Ecx);

        uint32_t* ret = asm_call<0x489140, Thiscall, uint32_t*>(s2, s1, 125, pCtx->Ecx);

        asm_call<0x488be0, Stdcall>(*ret, 0x6);

        // Restart New 1
        asm_call<0x488be0, Stdcall>(s3, 0x1);

        // Set restart flag, same under replay save status
        asm_call<0x416ba0, Thiscall>(pCtx->Esi, 0x6);

        // Switch menu state to close
        asm_call<0x4577d0, Thiscall>(pCtx->Edi, 18);

        pCtx->Edx = *(DWORD*)0x4ca21c;
        pCtx->Eip = 0x459562;
    })
    EHOOK_DY(th18_replay_restart, 0x417a70, 3, {
        auto callAddr = *(uint32_t*)(pCtx->Esp + 0x18);
        if (callAddr == 0x4619fe || callAddr == 0x463045) {
            pCtx->Eip = 0x417a73;
        }
    })
    EHOOK_DY(th18_exit, 0x459562, 5, {
        if (Gui::KeyboardInputGetRaw('Q')) {
            pCtx->Eip = 0x459578;
        }
    })
    EHOOK_DY(th18_stage_transition, 0x443e60, 1, {
        auto& guiReplay = THGuiRep::singleton();
        auto& advOptWnd = THAdvOptWnd::singleton();

        const uint32_t stage = GetMemContent(RVA(STAGE_NUM));
        const uint32_t score = GetMemContent(SCORE);
        const bool startedOnCS = guiReplay.mSelectedRepScores[guiReplay.mSelectedRepPlaybackStartStage - 1] == COUNTERSTOP;

        if (guiReplay.mRepStatus && !startedOnCS && score && stage > 1 && advOptWnd.mScoreOverwrites[stage - 1] < score)
            advOptWnd.mScoreOverwrites[stage - 1] = score;
    }) EHOOK_DY(th18_replay_end, 0x4588f0, 1, {
        auto& guiReplay = THGuiRep::singleton();
        auto& advOptWnd = THAdvOptWnd::singleton();

        const uint32_t stage = GetMemContent(RVA(STAGE_NUM));
        const uint32_t score = GetMemContent(SCORE);
        const bool startedOnCS = guiReplay.mSelectedRepScores[guiReplay.mSelectedRepPlaybackStartStage - 1] == COUNTERSTOP;

        if (guiReplay.mRepStatus && !startedOnCS && score && stage && advOptWnd.mScoreOverwrites[stage - 1] < score)
            advOptWnd.mScoreOverwrites[stage] = score;
    })
    // fix AoD timeout in Spell Prac crashing the game
    EHOOK_DY(th18_st6final_fix, 0x438e47, 8, {
        static int st6FinalDummy[4] { 0, 0, 0, 0 };
        uint32_t stageNum = GetMemContent(STAGE_NUM);
        bool inSpellPrac = GetMemContent(MODEFLAGS) & 32;

        if (stageNum == 6 && inSpellPrac && !pCtx->Ecx)
            pCtx->Ecx = (uint32_t)st6FinalDummy - 0x1270;
    })

    // fix Eiki & Eirin cards randomly crashing during transitions
    PATCH_DY(th18_eirin_eiki_card_uninit_fix, 0x411ac2, "54")

    // fix Centipede value being reset on stage transition in replays due to card reinitialization
    EHOOK_DY(th18_mukade_transition_fix, 0x412c76, 10, {
        if (!THGuiRep::singleton().mRepStatus)
            return;
        auto caller = *(uint32_t*)(pCtx->Esp + 0x20);

        if (caller == 0x417974 || // from shop cleanup
            (caller == 0x462e2a && *(uint32_t*)STAGE_NUM != *(uint32_t*)NEXT_STAGE_NUM)) // from replay transition?
            pCtx->Eip = 0x412c80;
    })

    // fix funcCalls used by Momoyo Sp6 taking into account Z axis, leading to desyncs
    PATCH_DY(th18_func_call2_uninit_fix, 0x4390ec, NOP(4)) PATCH_DY(th18_func_call3_uninit_fix, 0x43926c, NOP(4))

    // fix active card CD scroll mult. being reset after shops due to cleanup (desyncs Extra)
    EHOOK_DY(th18_scroll_fix, 0x407e05, 10, { // hooks on card resets
        if (THGuiRep::singleton().mRepStatus
            && *(uint32_t*)(pCtx->Esp + 0x18) == 0x417955 // called from shop cleanup
            && *(uint32_t*)(pCtx->Esp + 0x3c) == 0x417d39) // which was called from on tick
            pCtx->Eip = 0x407e0f; // skip resetting active card cooldown mult
    })

    // fix pre-transition active card selection being written to replay on transition
    EHOOK_DY(th18_active_card_fix, 0x462f33, 3, { // in stage transition record / playback data handling
        if (THGuiRep::singleton().mRepStatus)
            return;
        uint32_t activeCardId = GetMemContent(ABILITY_MANAGER_PTR, 0x38);

        if (activeCardId)
            *(uint32_t*)(pCtx->Esi + 0x964) = GetMemContent(activeCardId + 4);
        else
            *(uint32_t*)(pCtx->Esi + 0x964) = UINT_MAX;
    })

    // factor in stage/clear bonus when playing back replays
    EHOOK_DY(th18_score_uncap_replay_factor, 0x44480d, 5, { // runs on replay's final stage end (i.e. s6 in runs, sX in practice, not for gameovers)
        if (!THGuiRep::singleton().mRepStatus)
            return;
        uint32_t rpyInfo = (uint32_t)GetMemContent<Replay*>(REPLAY_MANAGER_PTR)->info;

        uintptr_t score = GetMemAddr(SCORE);
        uint32_t stageNum = GetMemContent(STAGE_NUM);
        uint32_t lives = GetMemContent(LIVES);
        uint32_t bombs = GetMemContent(BOMBS);

        auto stageBonus = 100000 * stageNum;
        auto clearBonus = 100000 * (lives * 5 + bombs);

        if (*(uint32_t*)(rpyInfo + 0xb8) == 8 && (stageNum == 6 || stageNum == 7))
            *(uint32_t*)score += clearBonus;

        *(uint32_t*)score += stageBonus;

        if (!THAdvOptWnd::singleton().scoreUncapChkbox && *(uint32_t*)score > 999999999)
            *(uint32_t*)score = 999999999;
    })
    EHOOK_DY(th18_add_card, 0x411460, 1, {
       uint32_t* list = nullptr;
       uint32_t sub_count = 0;
       uint32_t cardAddId = *(uint32_t*)(pCtx->Esp + 4);
       uint32_t cardAddType = 0;
       uint8_t cardIdArray[64] = {};

       for (uint32_t* i = (uint32_t*)GetMemContent(ABILITY_MANAGER_PTR, 0x1c); i; i = (uint32_t*)i[1]) {
           list = i;
           auto cardId = ((uint32_t**)list)[0][1];
           cardIdArray[cardId] += 1;
       }

       for (int i = 0; i < 56; ++i) {
           uint32_t* cardsSrcOffset = (uint32_t*)(CARD_DESC_LIST + 0x34 * i);
           if (cardIdArray[cardsSrcOffset[1]]) {
               if (cardsSrcOffset[3] == 1) {
                   sub_count += (cardsSrcOffset[1] == 51 ? 2 : 1) * cardIdArray[cardsSrcOffset[1]];
               }
           }
           if (cardsSrcOffset[1] == cardAddId) {
               cardAddType = cardsSrcOffset[3];
           }
       }

       auto op = *(uint32_t*)(pCtx->Esp + 8);;
       if (op != 3) {
           if (cardAddType == 1) {
               sub_count += (cardAddId == 51 ? 2 : 1);
               if (sub_count > 12) {
                   pCtx->Eip = 0x412d29;
               }
           } else if (cardAddId == 54 && cardIdArray[54]) {
               pCtx->Eip = 0x412d29;
           }
       }
    })
    EHOOK_DY(th18_active_buy_swap_fix, 0x412d94, 3, {
        asm_call<0x408c30, Fastcall>(*(uint32_t*)ABILITY_MANAGER_PTR);
    })
    EHOOK_DY(th18_rep_save, 0x462657, 5, {
        char* repName = (char*)(pCtx->Esp + 0x30);
        if (thPracParam.mode == 1)
            THSaveReplay(repName);
        else if (thPracParam.mode == 2 && thPracParam.phase)
            THSaveReplay(repName);
    })
    EHOOK_DY(th18_rep_menu_1, 0x467c67, 3, {
        THGuiRep::singleton().State(1);
    })
    EHOOK_DY(th18_rep_menu_2, 0x467d87, 5, {
        THGuiRep::singleton().State(2);
    })
    EHOOK_DY(th18_rep_menu_3, 0x467f6f, 2, {
        THGuiRep::singleton().State(3);
    })

    EHOOK_DY(th18_update, 0x4013f5, 1, {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        auto& o = THOverlay::singleton();
        o.Update();
        if (*o.mOpenMarket) {
            *o.mOpenMarket = false;
            if (uint32_t game_thread = GetMemContent(GAME_THREAD_PTR)) {
                *(uint32_t*)(game_thread + 0xB0) |= 0x20000;
            } else {
                play_sound_centered(SND_INVALID);
            }
        }
        THGuiSP::singleton().Update();
        TH18InGameInfo::singleton().Update();

        auto p = ImGui::GetOverlayDrawList();
        // in case boss movedown do not disabled when playing normal games
        {
            if (THAdvOptWnd::singleton().forceBossMoveDown || THAdvOptWnd::singleton().forceBossMoveDir) {
                auto sz = ImGui::CalcTextSize(S(TH_BOSS_FORCE_MOVE_DOWN));
                p->AddRectFilled({ 240.0f, 0.0f }, { sz.x + 240.0f, sz.y }, 0xFFCCCCCC);
                p->AddText({ 240.0f, 0.0f }, 0xFFFF0000, S(TH_BOSS_FORCE_MOVE_DOWN));
            }
        }

        if (g_adv_igi_options.show_keyboard_monitor && GetMemContent(PLAYER_PTR))
            KeysHUD(18, { 1280.0f, 0.0f }, { 840.0f, 0.0f }, g_adv_igi_options.keyboard_style);
        
        RenderLockTimer(p);
        GameUpdateOuter(p, 18);
        
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen() || THGuiSP::singleton().IsOpen();
        GameGuiEnd(drawCursor);
    })
     EHOOK_DY(th18_render, 0x401510, 1, {
        GameGuiRender(IMPL_WIN32_DX9);
    })
    EHOOK_DY(th18_player_state, 0x45BE90,1,{
        if (g_adv_igi_options.show_keyboard_monitor)
            RecordKey(18, *(DWORD*)(0x4CA428));
    })
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInGameInfo)
    EHOOK_DY(th18_force_card, 0x00417310,2,{
        if (g_adv_igi_options.th18_force_card){
            int stage = *(DWORD*)0x4CCCDC;
            if (stage >= 1 && stage <= 7 && stage != 6){
                if (stage == 7)
                    stage = 5;
                else
                    stage = stage - 1;
                int card_id2 = -1;
                for (int i = 0; i < 56; i++) { 
                    if (g_adv_igi_options.th18_cards[stage] == *(int32_t*)(CARD_DESC_LIST + 0x34 * i + 4)){
                        card_id2 = i;
                        break;
                    }
                }
                if (card_id2 != -1){
                    *(DWORD*)(pCtx->Edi + 0xA2C) = 1;
                    *(DWORD*)(pCtx->Eax) = CARD_DESC_LIST + 0x34 * card_id2;
                    *(DWORD*)(pCtx->Esp + 8) = 1;
                    pCtx->Eax = pCtx->Eax + 4;
                }
                    
            }
        }
    })
    EHOOK_DY(th18_game_start, 0x44278F,5, // gamestart-bomb set
    {
        TH18InGameInfo::singleton().mBombCount = 0;
        TH18InGameInfo::singleton().mMissCount = 0;
        TH18InGameInfo::singleton().mDeadBombCount = 0;
    })
    EHOOK_DY(th18_bomb_dec, 0x4574D3,4, // bomb dec
    {
        TH18InGameInfo::singleton().mBombCount++;
    })
    EHOOK_DY(th18_cylinder, 0x410F22,5, // cylinder
    {
        TH18InGameInfo::singleton().mBombCount++;
    })
    EHOOK_DY(th18_life_dec, 0x45D1A3,5, // life dec
    {
        TH18InGameInfo::singleton().mMissCount++;
        FastRetry(thPracParam.mode);
    })
    EHOOK_DY(th18_deadbomb1, 0x40DA1C,5, // rokumon
    {
        TH18InGameInfo::singleton().mDeadBombCount++;
    })
    EHOOK_DY(th18_deadbomb2, 0x40A534,5, // autobomb
    {
        TH18InGameInfo::singleton().mDeadBombCount++;
    })
    EHOOK_DY(th18_lock_timer1, 0x43A836,10, // initialize
    {
        g_lock_timer = 0;
    })
    EHOOK_DY(th18_lock_timer2, 0x4389F0,1, // SetNextPattern case 514
    {
        g_lock_timer = 0;
    })
    EHOOK_DY(th18_lock_timer3, 0x433BA6,6, // set boss mode case 512
    {
        g_lock_timer = 0;
    })
    EHOOK_DY(th18_lock_timer4, 0x42EF1D,6, // decrease time (update)
    {
        g_lock_timer_flag = true;
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext()) {
            return;
        }
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4ccdf8, WINDOW_PTR,
            Gui::INGAGME_INPUT_GEN2, 0x4ca21c, 0x4ca218, 0,
            -2, *(float*)0x56aca0, 0.0f);
        SetDpadHook(0x4016EF, 3);
        // Gui components creation
        THGuiPrac::singleton();
        THGuiPrac::singleton();
        THGuiPrac::singleton();
        THGuiPrac::singleton();
        TH18InGameInfo::singleton();

        // Hooks
        EnableAllHooks(THMainHook);

        // Replay user menu (null) fix
        DWORD oldProtect;
        VirtualProtect((void*)0x4b7ad8, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
        *(const char**)(0x4b7ad8) = "%s  %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%";
        VirtualProtect((void*)0x4b7ad8, 4, oldProtect, &oldProtect); 

        EnableAllHooks(THInGameInfo);

        // Reset thPracParam

        //Gui::ImplDX9NewFrame();
        thPracParam.Reset();
    }
    HOOKSET_DEFINE(THInitHook)
    PATCH_DY(th18_disable_demo, 0x464f7e, "ffffff7f")
    PATCH_DY(th18_disable_mutex, 0x474435, "eb")
    PATCH_DY(th18_disable_topmost, 0x4722CC, "00")
    PATCH_DY(th18_startup_1, 0x464c4f, "9090")
    PATCH_DY(th18_startup_2, 0x465bb0, "eb")
    EHOOK_DY(th18_gui_init_1, 0x465ce5, 6, {
        self->Disable();
        THGuiCreate();
    })
    EHOOK_DY(th18_gui_init_2, 0x4740c0, 1, {
        self->Disable();
        THGuiCreate();
    })
    HOOKSET_ENDDEF()
}

void TH18Init()
{
    EnableAllHooks(TH18::THInitHook);
    InitHook(18);
}
}
