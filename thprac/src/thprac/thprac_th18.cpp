﻿#include "thprac_games.h"
#include "thprac_utils.h"
#include <metrohash128.h>

namespace THPrac {
namespace TH18 {
#define play_sound_centered(id) asm_call<0x476BE0, Stdcall>(id, UNUSED_DWORD)

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

    enum sound_id : uint32_t {
        SND_INVALID = 16,
    };

    enum addrs {
        GAME_THREAD_PTR = 0x4cf2e4,
        BULLET_MANAGER_PTR = 0x4cf2bc,
        ITEM_MANAGER_PTR = 0x4cf2ec,
        ABILTIY_MANAGER_PTR = 0x4cf298,
        ABILITY_SHOP_PTR = 0x4cf2a4,
        CARD_DESC_LIST = 0x4c53c0,
        MUKADE_ADDR = 0x4cf2d4,
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
        void* vtable;
        void* thread;
        uint32_t tid;
        int32_t __bool_c;
        int32_t __bool_10;
        HINSTANCE* phModule;
        char filler_24[0x4];
    };

    struct CardBase {
        struct VTableCard* vtable;
        int32_t card_id;
        int32_t array_index___plus_1_i_think;
        ThList<CardBase> list_node;
        int32_t anm_id_for_ingame_effect;
        Timer recharge_timer;
        Timer _recharge_timer;
        int32_t recharge_time;
        struct TableCardData* table_entry;
        int32_t flags;
    };

    struct CardLily : public CardBase {
        int32_t count;
    };

    struct AbilityManager {
        char filler_0[0x4];
        struct UpdateFunc* on_tick;
        struct UpdateFunc* on_draw;
        struct AnmLoaded* ability_anm;
        struct AnmLoaded* abcard_anm;
        struct AnmLoaded* abmenu_anm;
        ThList<CardBase> card_list_head;
        int32_t num_total_cards;
        int32_t num_active_cards;
        int32_t num_equipment_cards;
        int32_t num_passive_cards;
        CardBase* selected_active_card;
        int32_t __id_3c;
        char filler_64[0xc];
        int32_t __id_4c;
        char filler_80[0x4];
        int32_t flags;
        int32_t __array_1[0x100];
        int32_t __array_2[0x100];
        int32_t __array_3[0x100];
        char filler_3160[0xc];
        int32_t __created_ability_txt;
        struct Thread __thread;
        char filler_3208[0xe8];
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

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            SetViewport((void*)0x4cd420);
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
                thPracParam.lily_count = 10000 - *mLilyCount;
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
            for (uint32_t* i = (uint32_t*)GetMemContent(ABILTIY_MANAGER_PTR, 0x1c); i; i = (uint32_t*)i[1]) {
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

                        if (card.first == LILY)
                            mLilyCount();

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

                if (chapterCounts[1] == 0 && chapterCounts[2] != 0) {
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
            TH18_LILY_COUNT, TH18_LILY_CD, TH18_BASSDRUM_CD, TH18_PSYCO_CD,
            TH18_CYLINDER_CD, TH18_RICEBALL_CD };

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
    class THGuiRep : public Gui::GameGuiWnd {
        THGuiRep() noexcept
        {
            wchar_t appdata[MAX_PATH];
            GetEnvironmentVariableW(L"APPDATA", appdata, MAX_PATH);
            mAppdataPath = appdata;
        }
        SINGLETON(THGuiRep);

    public:
        std::wstring mRepDir;
        std::wstring mRepName;
        uint64_t mRepMetroHash[2];
        bool mRepSelected = false;

        void CheckReplay()
        {
            uint32_t index = GetMemContent(0x4cf43c, 0x5aac);
            char* repName_ = (char*)GetMemAddr(0x4cf43c, index * 4 + 0x5ab4, 0x21c);
            std::wstring repName = mb_to_utf16(repName_, 932);
            std::wstring repDir(mAppdataPath);
            repDir.append(L"\\ShanghaiAlice\\th18\\replay\\");
            repDir.append(repName);
            mRepName = repName;
            mRepDir = repDir;

            std::string param;
            if (ReplayLoadParam(repDir.c_str(), param) && mRepParam.ReadJson(param))
                mParamStatus = true;
            else
                mRepParam.Reset();
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
                thPracParam.Reset();
                break;
            case 2:
                CheckReplay();
                mRepSelected = true;
                break;
            case 3:
                mRepStatus = true;
                if (mParamStatus)
                    memcpy(&thPracParam, &mRepParam, sizeof(THPracParam));
                CalcFileHash(mRepDir.c_str(), mRepMetroHash);
                break;
            default:
                break;
            }
        }

        std::wstring mAppdataPath;

    protected:
        bool mParamStatus = false;
        THPracParam mRepParam;
    };
    void AddIndicateCard()
    {
        if (GetMemContent(0x4ccd00) == 4) {
            th18_free_blank.Enable();
            asm_call<0x411460, Thiscall>(GetMemContent(ABILTIY_MANAGER_PTR), 0, 2);
        } else {
            uint32_t* list = nullptr;
            uint8_t cardIdArray[64];
            memset(cardIdArray, 0, 64);
            for (uint32_t* i = (uint32_t*)GetMemContent(ABILTIY_MANAGER_PTR, 0x1c); i; i = (uint32_t*)i[1]) {
                list = i;
                auto cardId = ((uint32_t**)list)[0][1];
                cardIdArray[cardId] += 1;
            }
            if (!cardIdArray[55]) {
                asm_call<0x411460, Thiscall>(*(uint32_t*)ABILTIY_MANAGER_PTR, 55, 2);
            }
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
        }
        SINGLETON(THOverlay);

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
                asm_call<0x411460, Thiscall>(*(uint32_t*)ABILTIY_MANAGER_PTR, cardId, 2);
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
                ImGui::TextUnformatted(S(TH18_MARKET_MANIP_DESC4));
            }
        }
        virtual void OnPreUpdate() override
        {
            CheckMarket();
            if (isInMarket && isManipMarket) {
                Open();
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255.0f, 0.0f, 0.0f, 255.0f));
                popColor = true;

                if (Gui::KeyboardInputGetRaw(VK_BACK)) {
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

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };

        HOTKEY_DEFINE(mMuteki, TH_MUTEKI, "F1", VK_F1)
        PATCH_HK(0x45d4ea, "01")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfLives, TH_INFLIVES, "F2", VK_F2)
        PATCH_HK(0x45d1a2, "00")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfBombs, TH_INFBOMBS, "F3", VK_F3)
        PATCH_HK(0x4574d3, "90909090"),
        PATCH_HK(0x40a3ed, "909090909090"),
        PATCH_HK(0x40a42c, "909090909090")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfPower, TH_INFPOWER, "F4", VK_F4)
        PATCH_HK(0x45748e, "9090")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfFunds, TH18_INFFUNDS, "F5", VK_F5)
        PATCH_HK(0x45c244, "909090909090"),
        PATCH_HK(0x40d96f, "90909090909090909090"),
        PATCH_HK(0x418496, "90909090909090909090"),
        PATCH_HK(0x418465, "9090")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F6", VK_F6)
        PATCH_HK(0x429eef, "eb"),
        PATCH_HK(0x43021b, "058d")
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
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F9", VK_F9 };
        Gui::GuiHotKey mOpenMarket { TH18_OPEN_MARKET, "F10", VK_F10 };
    };
    class THGuiSP : public Gui::GameGuiWnd {
        THGuiSP() noexcept
        {
            *mBugFix = true;

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiSP);

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

        Gui::GuiCheckBox mBugFix { TH16_BUGFIX };
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
    EHOOK_ST(th18_st6final_fix, 0x438e47, 8, {
        static int st6FinalDummy[4] { 0, 0, 0, 0 };
        if (!pCtx->Ecx) {
            pCtx->Ecx = (uint32_t)st6FinalDummy - 0x1270;
        }
    });
    EHOOK_ST(th18_scroll_fix, 0x407e05, 10, {
        if (GetMemContent(GAME_THREAD_PTR) && GetMemContent(GAME_THREAD_PTR, 0xd0) && *(uint32_t*)(pCtx->Esp + 0x18) == 0x417955 && *(uint32_t*)(pCtx->Esp + 0x3c) == 0x417d39) {
            pCtx->Eip = 0x407e0f;
        }
    });
    EHOOK_ST(th18_mukade_fix, 0x412c76, 10, {
        auto caller = *(uint32_t*)(pCtx->Esp + 0x20);
        if (caller == 0x417974) {
            pCtx->Eip = 0x412c80;
        } else if (caller == 0x462e2a) {
            if (*(uint32_t*)0x4cccdc != *(uint32_t*)0x4ccce0) {
                pCtx->Eip = 0x412c80;
            }
        }
    });
    EHOOK_ST(th18_active_card_fix, 0x462f33, 3, {
        if (GetMemContent(GAME_THREAD_PTR) && !GetMemContent(GAME_THREAD_PTR, 0xd0)) {
            uint32_t activeCardId = GetMemContent(ABILTIY_MANAGER_PTR, 0x38);
            if (activeCardId) {
                *(uint32_t*)(pCtx->Esi + 0x964) = GetMemContent(activeCardId + 4);
            } else {
                *(uint32_t*)(pCtx->Esi + 0x964) = UINT_MAX;
            }
        }
    });
    PATCH_ST(th18_eirin_eiki_card_uninit_fix, 0x411ac2, "54");
    PATCH_ST(th18_func_call2_uninit_fix, 0x4390ec, "0f1f4000");
    PATCH_ST(th18_func_call3_uninit_fix, 0x43926c, "0f1f4000");
    PATCH_ST(th18_all_clear_bonus_1, 0x4448ab, "eb0b909090");
    EHOOK_ST(th18_all_clear_bonus_2, 0x444afa, 7, {
        *(int32_t*)(GetMemAddr(0x4cf2e0, 0x158)) = *(int32_t*)(0x4cccfc);
        if (GetMemContent(0x4cccc8) & 0x10) {
            typedef void (*PScoreFunc)();
            PScoreFunc a = (PScoreFunc)0x458bd0;
            a();
            pCtx->Eip = 0x4448b0;
        }
    });
    EHOOK_ST(th18_all_clear_bonus_3, 0x444c49, 7, {
        *(int32_t*)(GetMemAddr(0x4cf2e0, 0x158)) = *(int32_t*)(0x4cccfc);
        if (GetMemContent(0x4cccc8) & 0x10) {
            typedef void (*PScoreFunc)();
            PScoreFunc a = (PScoreFunc)0x458bd0;
            a();
            pCtx->Eip = 0x4448b0;
        }
    });
    EHOOK_ST(th18_score_uncap_replay_fix, 0x4620b9, 3, {
        if (pCtx->Eax >= 0x3b9aca00) {
            pCtx->Eax = 0x3b9ac9ff;
        }
    });
    EHOOK_ST(th18_score_uncap_replay_disp, 0x468405, 1, {
        *(const char**)(pCtx->Esp) = scoreDispFmt;
    });
    
    extern HookCtx th18_static_mallet_replay_gold;
    extern HookCtx th18_static_mallet_replay_green;
    extern HookCtx th18_score_uncap_replay_factor;
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
            ScoreUncapInit();
            th18_mukade_fix.Setup();
            th18_scroll_fix.Setup();
            th18_st6final_fix.Setup();
            th18_active_card_fix.Setup();
            th18_eirin_eiki_card_uninit_fix.Setup();
            th18_func_call2_uninit_fix.Setup();
            th18_func_call3_uninit_fix.Setup();
            th18_rep_card_fix.Setup();
            th18_static_mallet_replay_gold.Setup();
            th18_static_mallet_replay_green.Setup();
        }
        SINGLETON(THAdvOptWnd);

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
        bool scoreUncapChkbox = false;
        bool scoreUncapOverwrite = false;
        bool scoreReplayFactor = false;
        bool staticMalletReplay = false;
        bool st6FinalFix = false;
        bool scrollFix = false;
        bool mukadeFix = false;
        bool restartFix = false;
        bool activeCardIdFix = false;
        bool eirinEikiCardFix = false;
        bool funcCallFix = false;

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
        std::wstring mRepOriginalName;
        std::wstring mRepOriginalPath;
        uint64_t mRepMetroHash[2];
        uint32_t mRepHeader[9];
        void* mRepDataDecoded = nullptr;
        void* mRepExtraData = nullptr;
        size_t mRepExtraDataSize = 0;
        const char* mStageStr[9] {
            "?", "1", "2", "3", "4", "5", "6", "Extra", "?"
        };
        __declspec(noinline) void MsgBox(UINT type, const wchar_t* title, const wchar_t* msg, const wchar_t* msg2 = nullptr)
        {
            std::wstring _msg = msg;
            if (msg2) {
                _msg += msg2;
            }
            MessageBoxW(*(HWND*)0x568c30, _msg.c_str(), title, type);
        }
        __declspec(noinline) void MsgBox(UINT type, const char* title, const char* msg, const char* msg2 = nullptr)
        {
            wchar_t _title[256];
            wchar_t _msg[256];
            wchar_t _msg2[256];
            MultiByteToWideChar(CP_UTF8, 0, title, -1, _title, 256);
            MultiByteToWideChar(CP_UTF8, 0, msg, -1, _msg, 256);
            if (msg2) {
                MultiByteToWideChar(CP_UTF8, 0, msg2, -1, _msg2, 256);
            }
            MsgBox(type, _title, _msg, msg2 ? _msg2 : nullptr);

        }
        __declspec(noinline) uint32_t* FindCardDesc(uint32_t id)
        {
            for (uint32_t i = CARD_DESC_LIST; true; i += 0x34) {
                if (*(uint32_t*)(i + 4) == id) {
                    return (uint32_t*)i;
                }
            }
        }

#define ThEncrypt(data, size1, param1, param2, param3, size2) asm_call<0x401f50, Fastcall>(data, size1, param1, param2, param3, size2)
#define ThDecrypt(data, size1, param1, param2, param3, size2) asm_call<0x401e40, Fastcall>(data, size1, param1, param2, param3, size2)
#define ThUnlzss(dataBuffer, dataSize, outBuffer, outSize)    asm_call<0x46f840, Fastcall>(dataBuffer, dataSize, outBuffer, outSize)
#define ThLzss(dataBuffer, dataSize, outSize)                 asm_call<0x46f5b0, Fastcall, void*>(dataBuffer, dataSize, outSize)
#define _builtin_free(buffer)                                 asm_call<0x491a3f, Cdecl>(buffer)

        __declspec(noinline) void UnloadReplay()
        {
            if (mRepDataDecoded) {
                free(mRepDataDecoded);
                mRepDataDecoded = nullptr;
            }
            if (mRepExtraData) {
                free(mRepExtraData);
                mRepExtraData = nullptr;
            }

            mFixData.clear();
            th18_rep_card_fix.Disable();
        }
        __declspec(noinline) void OverwriteReplayData(void* replayData)
        {
            for (auto& fix : mFixData) {
                if (fix.activeCardVec.size()) {
                    int32_t* cardIdx = (int32_t*)(fix.activeCardIdPtr + (uint32_t)replayData);
                    *cardIdx = fix.activeCardVec[fix.activeCardComboIdx];
                }
            }
        }
        __declspec(noinline) void ParseReplayData()
        {
            if (!mRepDataDecoded) {
                return;
            }

            uint32_t repData = (uint32_t)mRepDataDecoded + 0xc8;
            int limit = *(int*)((uint32_t)mRepDataDecoded + 0xa8);
            if (limit >= 8) {
                limit = 6;
            }

            for (int i = 0; i < limit; ++i) {
                if (*(uint16_t*)(repData) >= 1 && *(uint16_t*)(repData) <= 7) {
                    FixData data;
                    data.stage = *(uint16_t*)(repData);

                    uint8_t cardTrigger[57];
                    memset(cardTrigger, 0, 57);
                    data.activeCardId = *(int32_t*)(repData + 0x964);
                    data.activeCardIdPtr = repData + 0x964 - (uint32_t)mRepDataDecoded;
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
                repData = repData + *(uint32_t*)(repData + 8) + 0x126c;
            }
        }
        __declspec(noinline) void SaveReplay()
        {
            uint32_t repHeader[9];
            memcpy(repHeader, mRepHeader, sizeof(repHeader));

            void* repDataOutput = malloc(repHeader[8]);
            memcpy(repDataOutput, mRepDataDecoded, repHeader[8]);
            OverwriteReplayData(repDataOutput);

            uint32_t repDataEncodedSize;
            auto repDataEncoded = ThLzss(repDataOutput, repHeader[8], &repDataEncodedSize);
            repHeader[7] = repDataEncodedSize;
            repHeader[3] = repDataEncodedSize + 0x24;
            ThEncrypt(repDataEncoded, mRepHeader[7], 0x7d, 0x3a, 0x100, repHeader[7]);
            ThEncrypt(repDataEncoded, mRepHeader[7], 0x5c, 0xe1, 0x400, repHeader[7]);
            free(repDataOutput);

            DWORD bytesProcessed;
            std::wstring repDir = THGuiRep::singleton().mAppdataPath;
            repDir.append(L"\\ShanghaiAlice\\th18\\replay\\");
            OPENFILENAMEW ofn;
            wchar_t szFile[512];
            wcscpy_s(szFile, L"th18_ud----.rpy");
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = *(HWND*)0x568c30;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Replay File\0*.rpy\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = nullptr;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = repDir.c_str();
            ofn.lpstrDefExt = L".rpy";
            ofn.Flags = OFN_OVERWRITEPROMPT;
            if (GetSaveFileNameW(&ofn)) {
                auto outputFile = CreateFileW(szFile, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (outputFile == INVALID_HANDLE_VALUE) {
                    MsgBox(MB_ICONERROR | MB_OK, S(TH14_ERROR), S(TH14_ERROR_DEST));
                    goto end;
                }
                SetFilePointer(outputFile, 0, nullptr, FILE_BEGIN);
                SetEndOfFile(outputFile);
                WriteFile(outputFile, repHeader, sizeof(repHeader), &bytesProcessed, nullptr);
                WriteFile(outputFile, repDataEncoded, repDataEncodedSize, &bytesProcessed, nullptr);
                WriteFile(outputFile, mRepExtraData, mRepExtraDataSize, &bytesProcessed, nullptr);
                CloseHandle(outputFile);

                MsgBox(MB_ICONINFORMATION | MB_OK, utf8_to_utf16(S(TH14_SUCCESS)).c_str(), utf8_to_utf16(S(TH14_SUCCESS_SAVED)).c_str(), szFile);
            }

            end:
            _builtin_free(repDataEncoded);
        }
        __declspec(noinline) void LoadReplay()
        {
            UnloadReplay();

            // Load replay
            MappedFile file(THGuiRep::singleton().mRepDir.c_str());

            mRepOriginalName = THGuiRep::singleton().mRepName;
            mRepOriginalPath = THGuiRep::singleton().mRepDir;

            // Decode and copy data
            void* mRepDataRaw = nullptr;
            memcpy(mRepHeader, file.fileMapView, 0x24);
            mRepDataRaw = malloc(mRepHeader[7]);
            mRepDataDecoded = malloc(mRepHeader[8]);
            mRepExtraDataSize = file.fileSize - mRepHeader[3];
            mRepExtraData = malloc(mRepExtraDataSize);
            memcpy(mRepDataRaw, (void*)((uint32_t)file.fileMapView + 0x24), mRepHeader[7]);
            memcpy(mRepExtraData, (void*)((uint32_t)file.fileMapView + mRepHeader[3]), mRepExtraDataSize);
            ThDecrypt(mRepDataRaw, mRepHeader[7], 0x5c, 0xe1, 0x400, mRepHeader[7]);
            ThDecrypt(mRepDataRaw, mRepHeader[7], 0x7d, 0x3a, 0x100, mRepHeader[7]);
            ThUnlzss(mRepDataRaw, mRepHeader[7], mRepDataDecoded, mRepHeader[8]);
            free(mRepDataRaw);

            // Calc Hash
            mRepMetroHash[0] = 0;
            mRepMetroHash[1] = 0;
            MetroHash128::Hash((uint8_t*)file.fileMapView, file.fileSize, (uint8_t*)mRepMetroHash);

            ParseReplayData();
            th18_rep_card_fix.Enable();

            return;
        }
        bool GetAvailability()
        {
            if (!mRepDataDecoded) {
                return false;
            }
            auto& repMenu = THGuiRep::singleton();
            if (GetMemContent(GAME_THREAD_PTR) && !GetMemContent(GAME_THREAD_PTR, 0xd0)) {
                return false;
            }
            if (repMenu.mRepStatus && (repMenu.mRepMetroHash[0] != mRepMetroHash[0] || repMenu.mRepMetroHash[1] != mRepMetroHash[1])) {
                return false;
            }
            return true;
        }
        bool ReplayMenu()
        {
            bool wndFocus = true;

            if (BeginOptGroup<TH18_REPLAY_FIX>()) {
                ImGui::TextUnformatted(S(TH18_REPFIX_DESC));
                if (mShowFixInstruction) {
                    if (ImGui::Button(S(TH18_REPFIX_HIDE_INS)))
                        mShowFixInstruction = false;
                } else {
                    if (ImGui::Button(S(TH18_REPFIX_SHOW_INS)))
                        mShowFixInstruction = true;
                }
                if (mShowFixInstruction) {
                    ImGui::PushTextWrapPos(GetRelWidth(0.95f));
                    ImGui::TextUnformatted(S(TH18_REPFIX_INS));
                    ImGui::PopTextWrapPos();
                }
                ImGui::NewLine();

                if (!mRepDataDecoded) {
                    if (THGuiRep::singleton().mRepSelected) {
                        ImGui::Text(S(TH18_REPFIX_SELECTED), THGuiRep::singleton().mRepName.c_str());

                        if (!mRepDataDecoded) {
                            ImGui::SameLine();
                            if (ImGui::Button(S(TH18_REPFIX_LOCK))) {
                                LoadReplay();
                            }
                        }
                    } else {
                        ImGui::TextUnformatted(S(TH18_REPFIX_NOTHING));
                    }
                } else {
                    ImGui::Text(S(TH18_REPFIX_LOCKED), mRepOriginalName.c_str());
                    ImGui::SameLine();
                    if (ImGui::Button(S(TH18_REPFIX_UNLOCK))) {
                        UnloadReplay();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(S(TH18_REPFIX_SAVEAS))) {
                        SaveReplay();
                    }

                    auto isAvailable = GetAvailability();
                    if (!isAvailable) {
                        ImGui::TextUnformatted(S(TH18_REPFIX_MISMATCH));
                        ImGui::BeginDisabled();
                    }

                    char comboId[64];
                    auto fontSize = ImGui::GetFontSize();
                    for (auto& data : mFixData) {
                        if (data.activeCardId != -1) {
                            ImGui::Text("Stage %s:", mStageStr[data.stage]);
                            ImGui::SameLine();
                            ImGui::TextUnformatted(S(TH18_REPFIX_INITIAL_CARD));
                            ImGui::SameLine(0.0f, 0.0f);

                            sprintf_s(comboId, "##active_card_idx_st%d", data.stage);
                            ImGui::PushItemWidth(fontSize * 10.0f);
                            ImGui::ComboSectionsDefault(comboId, &data.activeCardComboIdx, data.activeCardLabelVec.data(), Gui::LocaleGetCurrentGlossary(), "");
                            if (ImGui::IsPopupOpen(comboId)) {
                                wndFocus = false;
                            }
                            ImGui::PopItemWidth();
                        }
                    }

                    if (!isAvailable) {
                        ImGui::EndDisabled();
                    }
                }

                EndOptGroup();
            }

            return wndFocus;
        }
        void RestartFix()
        {
            if (restartFix) {
                if (*(uint32_t*)0x4cccdc == *(uint32_t*)0x4ccce0) {
                    uint32_t* list = nullptr;
                    uint8_t cardIdArray[64];
                    memset(cardIdArray, 0, 64);
                    for (uint32_t* i = (uint32_t*)GetMemContent(ABILTIY_MANAGER_PTR, 0x1c); i; i = (uint32_t*)i[1]) {
                        list = i;
                        auto cardId = ((uint32_t**)list)[0][1];
                        cardIdArray[cardId] += 1;
                    }

                    for (int i = 0; i < 56; ++i) {
                        *(uint32_t*)GetMemAddr(ABILTIY_MANAGER_PTR, 0xc84 + i * 4) = cardIdArray[i] ? 1 : 0;
                    }
                }
            }
        }

    private:
        void FpsInit()
        {
            if (*(uint8_t*)0x4cd011 == 3) {
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
            if (mOptCtx.fps_status == 1) {
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

        void ScoreUncapInit()
        {
            for (size_t i = 0; i < elementsof(scoreUncapHooks); i++) {
                scoreUncapHooks[i].Setup();
            }
            
            th18_score_uncap_replay_fix.Setup();
            th18_score_uncap_replay_disp.Setup();
            th18_score_uncap_replay_factor.Setup();
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
            th18_score_uncap_replay_fix.Toggle(!scoreUncapOverwrite);
            th18_score_uncap_replay_disp.Toggle(scoreUncapChkbox);
            scoreUncapStageTrFix[0].Toggle(scoreUncapChkbox);
            scoreUncapStageTrFix[1].Toggle(scoreUncapChkbox);
        }

    public:
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
                if (GameplayOpt(mOptCtx))
                    GameplaySet();
                if (ImGui::Checkbox(S(TH18_UNCAP), &scoreUncapChkbox)) {
                    if (!scoreUncapChkbox) {
                        scoreUncapOverwrite = false;
                    }
                    ScoreUncapSet();
                }
                ImGui::SameLine();
                if (!scoreUncapChkbox) {
                    ImGui::BeginDisabled();
                }
                if (ImGui::Checkbox(S(TH18_UNCAP_OVERWRITE), &scoreUncapOverwrite)) {
                    ScoreUncapSet();
                }
                if (!scoreUncapChkbox) {
                    ImGui::EndDisabled();
                }

                if (ImGui::Checkbox(S(TH18_REPLAY_BONUS), &scoreReplayFactor)) {
                    th18_score_uncap_replay_factor.Toggle(scoreReplayFactor);
                }
                
                if (ImGui::Checkbox(S(TH18_STATIC_MALLET), &staticMalletReplay)) {
                    th18_static_mallet_replay_gold.Toggle(staticMalletReplay);
                    th18_static_mallet_replay_green.Toggle(staticMalletReplay);
                }
                ImGui::SameLine();
                HelpMarker(S(TH18_STATIC_MALLET_DESC));
                
                EndOptGroup();
            }
            if (BeginOptGroup<TH18_BUG_FIX>()) {
                ImGui::TextUnformatted(S(TH18_BUG_FIX_DESC));

                if (ImGui::Checkbox(S(TH18_MUKADE_FIX), &mukadeFix)) {
                    th18_mukade_fix.Toggle(mukadeFix);
                }
                ImGui::SameLine();
                HelpMarker(S(TH18_MUKADE_FIX_DESC));

                if (ImGui::Checkbox(S(TH18_SCROLL_FIX), &scrollFix)) {
                    th18_scroll_fix.Toggle(scrollFix);
                }
                ImGui::SameLine();
                HelpMarker(S(TH18_SCROLL_FIX_DESC));

                if (ImGui::Checkbox(S(TH18_ST6FINAL_FIX), &st6FinalFix)) {
                    th18_st6final_fix.Toggle(st6FinalFix);
                }
                ImGui::SameLine();
                HelpMarker(S(TH18_ST6FINAL_FIX_DESC));

                ImGui::Checkbox(S(TH18_RESTART_FIX), &restartFix);

                if (ImGui::Checkbox(S(TH18_AC_FIX), &activeCardIdFix)) {
                    th18_active_card_fix.Toggle(activeCardIdFix);
                }
                ImGui::SameLine();
                HelpMarker(S(TH18_AC_FIX_DESC));

                if (ImGui::Checkbox(S(TH18_EIRIN_EIKI_FIX), &eirinEikiCardFix)) {
                    th18_eirin_eiki_card_uninit_fix.Toggle(eirinEikiCardFix);
                }
                ImGui::SameLine();
                HelpMarker(S(TH18_EIRIN_EIKI_FIX_DESC));

                if (ImGui::Checkbox(S(TH18_FUNC_CALL_FIX), &funcCallFix)) {
                    th18_func_call2_uninit_fix.Toggle(funcCallFix);
                    th18_func_call3_uninit_fix.Toggle(funcCallFix);
                }
                ImGui::SameLine();
                HelpMarker(S(TH18_FUNC_CALL_FIX_DESC));

                EndOptGroup();
            }

            wndFocus &= ReplayMenu();

            AboutOpt();
            ImGui::EndChild();
            if (wndFocus)
                ImGui::SetWindowFocus();
        }
        void PreUpdate()
        {
            LocaleUpdate();
        }

        adv_opt_ctx mOptCtx;
    };
    EHOOK_ST(th18_rep_card_fix, 0x462e4b, 5, {
        if (THAdvOptWnd::singleton().GetAvailability()) {
            auto& fixVec = THAdvOptWnd::singleton().mFixData;
            for (auto& fix : fixVec) {
                if (fix.stage == *(uint32_t*)0x4CCCDC) {
                    *(int32_t*)pCtx->Esp = fix.activeCardVec[fix.activeCardComboIdx];
                    break;
                }
            }
        }
    });
    EHOOK_ST(th18_static_mallet_replay_gold, 0x429222, 6, {
        if (GetMemContent(GAME_THREAD_PTR, 0xd0))
            THAdvOptWnd::StaticMalletConversion(pCtx);
    });
    EHOOK_ST(th18_static_mallet_replay_green, 0x42921d, 5, {
        if (GetMemContent(GAME_THREAD_PTR, 0xd0))
            THAdvOptWnd::StaticMalletConversion(pCtx);
    });
    EHOOK_ST(th18_score_uncap_replay_factor, 0x44480d, 5, {
        uint32_t* score = (uint32_t*)0x4cccfc;
        uint32_t* stage_num = (uint32_t*)0x4cccdc;
        uint32_t* lifes = (uint32_t*)0x4ccd48;
        uint32_t* bombs = (uint32_t*)0x4ccd58;

        auto stageBonus = 100000 * *stage_num;
        auto clearBonus = 100000 * (*lifes * 5 + *bombs);
        if (GetMemContent(GAME_THREAD_PTR, 0xd0)) {
            uint32_t rpy = *(uint32_t*)(*(uint32_t*)0x4cf418 + 0x18);
            if (*(uint32_t*)(rpy + 0xb8) == 8 && (*stage_num == 6 || *stage_num == 7))
                *score += clearBonus;
            *score += stageBonus;
            if (!THAdvOptWnd::singleton().scoreUncapChkbox && *score > 999999999)
                *score = 999999999;
        }
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
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xa750, 0xab90, 60, 90);
                ECLJump(ecl, 0x6fc8, 0x7088, 0);
                break;
            case 3:
                ECLJump(ecl, 0xa750, 0xab90, 60, 90);
                ECLJump(ecl, 0x6fc8, 0x70bc, 0);
                break;
            case 4:
                ECLStdExec(ecl, 0xa750, 1, 1);
                ECLJump(ecl, 0, 0xac2c, 60, 90);
                break;
            case 5:
                ECLStdExec(ecl, 0xa750, 1, 1);
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
                ECLJump(ecl, 0x90b4, 0x94cc, 60, 90); // 0x9550, 0x9594, 0x9600
                ECLJump(ecl, 0x55e0, 0x5614, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x90b4, 0x94cc, 60, 90); // 0x9550, 0x9594, 0x9600
                ECLJump(ecl, 0x55e0, 0x56a8, 0, 0);
                break;
            case 4:
                ECLStdExec(ecl, 0x90b4, 1, 1);
                ECLJump(ecl, 0, 0x9594, 60, 90); // 0x9550, 0x9594, 0x9600
                break;
            case 5:
                ECLStdExec(ecl, 0x90b4, 1, 1);
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
                ECLJump(ecl, 0x83a0, 0x86ec, 60, 90); // 0x8784, 0x87c8, 0x8848
                ECLJump(ecl, 0x4c34, 0x4c68, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x83a0, 0x86ec, 60, 90); // 0x8784, 0x87c8, 0x8848
                ECLJump(ecl, 0x4c34, 0x4cc8, 0, 0);
                break;
            case 4:
                ECLStdExec(ecl, 0x83a0, 1, 1);
                ECLJump(ecl, 0, 0x87c8, 60, 90); // 0x8784, 0x87c8, 0x8848
                break;
            case 5:
                ECLStdExec(ecl, 0x83a0, 1, 1);
                ECLJump(ecl, 0, 0x87c8, 60, 90); // 0x8784, 0x87c8, 0x8848
                ECLJump(ecl, 0x4d90, 0x4dd8, 0, 0);
                break;
            case 6:
                ECLStdExec(ecl, 0x83a0, 1, 1);
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
                ECLJump(ecl, 0x5a04, 0x5d50, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x33cc, 0x3400, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x5a04, 0x5d50, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x33cc, 0x3434, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x5a04, 0x5d50, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x33cc, 0x3468, 0, 0);
                break;
            case 5:
                ECLStdExec(ecl, 0x5a04, 1, 1);
                ECLJump(ecl, 0, 0x5dd8, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                break;
            case 6:
                ECLStdExec(ecl, 0x5a04, 1, 1);
                ECLJump(ecl, 0, 0x5dd8, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x34fc, 0x3598, 0, 0);
                break;
            case 7:
                ECLStdExec(ecl, 0x5a04, 1, 1);
                ECLJump(ecl, 0, 0x5dd8, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x34fc, 0x35cc, 0, 0);
                break;
            case 8:
                ECLStdExec(ecl, 0x5a04, 1, 1);
                ECLJump(ecl, 0, 0x5dd8, 60, 90); // 0x5d94, 0x5dd8, 0x5e58
                ECLJump(ecl, 0x34fc, 0x3640, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                ECLJump(ecl, 0x96ec, 0x9b28, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ecl << pair{0x4c50, 0};
                break;
            case 2:
                ECLJump(ecl, 0x96ec, 0x9b28, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4c40, 0x4c88, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x96ec, 0x9b28, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4c40, 0x4cbc, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x96ec, 0x9b28, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4c40, 0x4cf0, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x96ec, 0x9bd8, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ecl << pair{0x4d94, 0};
                break;
            case 6:
                ECLJump(ecl, 0x96ec, 0x9bd8, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4d84, 0x4df8, 0, 0);
                break;
            case 7:
                ECLStdExec(ecl, 0x96ec, 1, 1);
                ECLJump(ecl, 0, 0x9bd8, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4d84, 0x4e2c, 0, 0);
                break;
            case 8:
                ECLStdExec(ecl, 0x96ec, 1, 1);
                ECLJump(ecl, 0, 0x9bd8, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4d84, 0x4e8c, 0, 0);
                break;
            case 9:
                ECLStdExec(ecl, 0x96ec, 1, 1);
                ECLJump(ecl, 0, 0x9bd8, 60, 90); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4d84, 0x4ec0, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                ECLJump(ecl, 0x7e70, 0x82ac, 60, 90); // 0x8318, 0x835c, 0x83dc
                ecl << pair{0x5ca0, 0};
                break;
            case 2:
                ECLJump(ecl, 0x7e70, 0x82ac, 60, 90); // 0x8318, 0x835c, 0x83dc
                ECLJump(ecl, 0x5c90, 0x5d0c, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x7e70, 0x82ac, 60, 90); // 0x8318, 0x835c, 0x83dc
                ECLJump(ecl, 0x5c90, 0x5d74, 0, 0);
                break;
            case 4:
                ECLStdExec(ecl, 0x7e70, 1, 1);
                ECLJump(ecl, 0, 0x835c, 60, 90); // 0x8318, 0x835c, 0x83dc
                ECLJump(ecl, 0x5e3c, 0x5e7c, 0, 0);
                break;
            case 5:
                ECLStdExec(ecl, 0x7e70, 1, 1);
                ECLJump(ecl, 0, 0x835c, 60, 90); // 0x8318, 0x835c, 0x83dc
                ECLJump(ecl, 0x5e3c, 0x5eb0, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                ECLJump(ecl, 0xa8fc, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ecl << pair{0x5a18, 0};
                break;
            case 2:
                ECLJump(ecl, 0xa8fc, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5a08, 0x5a50, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0xa8fc, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5a08, 0x5a84, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0xa8fc, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5a08, 0x5ab8, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0xa8fc, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5a08, 0x5b18, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0xa8fc, 0xaf34, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5a08, 0x5b4c, 0, 0);
                break;
            case 7:
                ECLStdExec(ecl, 0xa8fc, 1, 1);
                ECLJump(ecl, 0, 0xafe4, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ecl << pair{0x5bd4, 0};
                break;
            case 8:
                ECLStdExec(ecl, 0xa8fc, 1, 1);
                ECLJump(ecl, 0, 0xafe4, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5bb4, 0x5c38, 0, 0);
                break;
            case 9:
                ECLStdExec(ecl, 0xa8fc, 1, 1);
                ECLJump(ecl, 0, 0xafe4, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5bb4, 0x5c6c, 0, 0);
                break;
            case 10:
                ECLStdExec(ecl, 0xa8fc, 1, 1);
                ECLJump(ecl, 0, 0xafe4, 124, 90); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x5bb4, 0x5ccc, 0, 0);
                break;
            case 11:
                ECLStdExec(ecl, 0xa8fc, 1, 1);
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
        switch (section) {
        case THPrac::TH18::TH18_ST1_MID1:
            ECLStdExec(ecl, 0xa750, 1, 1);
            ECLJump(ecl, 0, 0xabe8, 60);
            break;
        case THPrac::TH18::TH18_ST1_BOSS1:
            ECLStdExec(ecl, 0xa750, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0xacac, 60);
            else
                ECLJump(ecl, 0, 0xacc0, 60);
            break;
        case THPrac::TH18::TH18_ST1_BOSS2:
            ECLStdExec(ecl, 0xa750, 1, 1);
            ECLJump(ecl, 0, 0xacc0, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3e4, 0x4cc, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4dc, 2100}; // Set Health
            ecl << pair{0x4fc, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST1_BOSS3:
            ECLStdExec(ecl, 0xa750, 1, 1);
            ECLJump(ecl, 0, 0xacc0, 60);
            ecl.SetFile(2);
            ecl << pair{0x61c, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1350, (int16_t)0} << pair{0x147c, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1270, (int16_t)0}; // Disable Invincible
            ECLJump(ecl, 0x15d0, 0x1650, 0); // Skip wait
            break;
        case THPrac::TH18::TH18_ST1_BOSS4:
            ECLStdExec(ecl, 0xa750, 1, 1);
            ECLJump(ecl, 0, 0xacc0, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3e4, 0x4cc, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4dc, 2200}; // Set Health
            ecl << pair{0x4fc, (int8_t)0x32}; // Set Spell Ordinal
            break;


        case THPrac::TH18::TH18_ST2_MID1:
            ECLStdExec(ecl, 0x90b4, 1, 1);
            ECLJump(ecl, 0, 0x9550, 60); // 0x9550, 0x9594, 0x9600
            break;
        case THPrac::TH18::TH18_ST2_BOSS1:
            ECLStdExec(ecl, 0x90b4, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x95ec, 60);
            else
                ECLJump(ecl, 0, 0x9600, 60); // 0x9550, 0x9594, 0x9600
            break;
        case THPrac::TH18::TH18_ST2_BOSS2:
            ECLStdExec(ecl, 0x90b4, 1, 1);
            ECLJump(ecl, 0, 0x9600, 60); // 0x9550, 0x9594, 0x9600

            ecl.SetFile(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2300}; // Set Health
            ecl << pair{0x524, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST2_BOSS3:
            ECLStdExec(ecl, 0x90b4, 1, 1);
            ECLJump(ecl, 0, 0x9600, 60); // 0x9550, 0x9594, 0x9600

            ecl.SetFile(2);
            ecl << pair{0x748, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x113c, (int16_t)0} << pair{0x1268, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x105c, (int16_t)0}; // Disable Invincible
            ECLJump(ecl, 0x13bc, 0x13fc, 0); // Skip wait
            break;
        case THPrac::TH18::TH18_ST2_BOSS4:
            ECLStdExec(ecl, 0x90b4, 1, 1);
            ECLJump(ecl, 0, 0x9600, 60); // 0x9550, 0x9594, 0x9600

            ecl.SetFile(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2300}; // Set Health
            ecl << pair{0x524, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST2_BOSS5:
            ECLStdExec(ecl, 0x90b4, 1, 1);
            ECLJump(ecl, 0, 0x9600, 60); // 0x9550, 0x9594, 0x9600

            ecl.SetFile(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2700}; // Set Health
            ecl << pair{0x524, (int8_t)0x33}; // Set Spell Ordinal
            break;


        case THPrac::TH18::TH18_ST3_MID1:
            ECLStdExec(ecl, 0x83a0, 1, 1);
            ECLJump(ecl, 0, 0x8784, 60); // 0x8784, 0x87c8, 0x8848
            ecl.SetFile(3);
            ecl << pair{0x324, (int16_t)100};
            ecl.SetPos(0x400);
            ecl << 0 << 0x001401ff << 0x01ff0000 << 0 << 13000;
            ECLJump(ecl, 0, 0x47c, 0);
            break;
        case THPrac::TH18::TH18_ST3_BOSS1:
            ECLStdExec(ecl, 0x83a0, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x8834, 60);
            else
                ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848
            break;
        case THPrac::TH18::TH18_ST3_BOSS2:
            ECLStdExec(ecl, 0x83a0, 1, 1);
            ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848

            ecl.SetFile(2);
            ECLJump(ecl, 0x46c, 0x554, 1); // Utilize Spell Practice Jump
            ecl << pair{0x564, 2800}; // Set Health
            ecl << pair{0x584, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST3_BOSS3:
            ECLStdExec(ecl, 0x83a0, 1, 1);
            ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848

            ecl.SetFile(2);
            ecl << pair{0x7c8, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x14cc, (int16_t)30}; // Disable Invincible
            ecl << pair{0x15ac, (int16_t)0} << pair{0x16d8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x183c, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST3_BOSS4:
            ECLStdExec(ecl, 0x83a0, 1, 1);
            ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848

            ecl.SetFile(2);
            ECLJump(ecl, 0x46c, 0x554, 1); // Utilize Spell Practice Jump
            ecl << pair{0x564, 2800}; // Set Health
            ecl << pair{0x584, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST3_BOSS5:
            ECLStdExec(ecl, 0x83a0, 1, 1);
            ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848

            ecl.SetFile(2);
            ecl << pair{0x7c8, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x24f4, (int16_t)30}; // Disable Invincible
            ecl << pair{0x25d4, (int16_t)0} << pair{0x2700, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2864, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST3_BOSS6:
            ECLStdExec(ecl, 0x83a0, 1, 1);
            ECLJump(ecl, 0, 0x8848, 60); // 0x8784, 0x87c8, 0x8848

            ecl.SetFile(2);
            ECLJump(ecl, 0x46c, 0x554, 1); // Utilize Spell Practice Jump
            ecl << pair{0x564, 2600}; // Set Health
            ecl << pair{0x584, (int8_t)0x33}; // Set Spell Ordinal
            break;


        case THPrac::TH18::TH18_ST4_MID1:
            ECLStdExec(ecl, 0x5a04, 1, 1);
            ECLJump(ecl, 0, 0x5d94, 60); // 0x5d94, 0x5dd8, 0x5e58
            break;
        case THPrac::TH18::TH18_ST4_BOSS1:
            ECLStdExec(ecl, 0x5a04, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x5e44, 60); // 0x5d94, 0x5dd8, 0x5e58
            else
                ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58
            break;
        case THPrac::TH18::TH18_ST4_BOSS2:
            ECLStdExec(ecl, 0x5a04, 1, 1);
            ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58

            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4f4, 3400}; // Set Health
            ecl << pair{0x514, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST4_BOSS3:
            ECLStdExec(ecl, 0x5a04, 1, 1);
            ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58

            ecl.SetFile(2);
            ecl << pair{0x758, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x12b4, (int16_t)30}; // Disable Invincible
            ecl << pair{0x1394, (int16_t)0} << pair{0x14c0, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1624, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST4_BOSS4:
            ECLStdExec(ecl, 0x5a04, 1, 1);
            ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58

            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4f4, 3400}; // Set Health
            ecl << pair{0x514, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST4_BOSS5:
            ECLStdExec(ecl, 0x5a04, 1, 1);
            ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58

            ecl.SetFile(2);
            ecl << pair{0x758, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x20e0, (int16_t)30}; // Disable Invincible
            ecl << pair{0x21c0, (int16_t)0} << pair{0x22ec, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2450, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST4_BOSS6:
            ECLStdExec(ecl, 0x5a04, 1, 1);
            ECLJump(ecl, 0, 0x5e58, 60); // 0x5d94, 0x5dd8, 0x5e58

            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4f4, 4800}; // Set Health
            ecl << pair{0x514, (int8_t)0x33}; // Set Spell Ordinal
            break;


        case THPrac::TH18::TH18_ST5_MID1:
            ECLJump(ecl, 0x96ec, 0x9b94, 60); // 0x9b94, 0x9bd8, 0x9c58
            ecl.SetFile(3);
            ecl << pair{0x324, (int16_t)100};
            ecl.SetPos(0x3ac);
            ecl << 0 << 0x002401ff << 0x01ff0000 << 0 << 13000;
            break;
        case THPrac::TH18::TH18_ST5_BOSS1:
            ECLStdExec(ecl, 0x96ec, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x9c44, 60); // 0x9b94, 0x9bd8, 0x9c58
            else {
                ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
                ECLJump(ecl, 0x4a44, 0x4a94, 3);
            }
            break;
        case THPrac::TH18::TH18_ST5_BOSS2:
            ECLStdExec(ecl, 0x96ec, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x430, 0x518, 1); // Utilize Spell Practice Jump
            ecl << pair{0x528, 3200}; // Set Health
            ecl << pair{0x548, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST5_BOSS3:
            ECLStdExec(ecl, 0x96ec, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ecl << pair{0x890, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x10b0, (int16_t)30}; // Disable Invincible
            ecl << pair{0x1190, (int16_t)0} << pair{0x12bc, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x13f4, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST5_BOSS4:
            ECLStdExec(ecl, 0x96ec, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x430, 0x518, 1); // Utilize Spell Practice Jump
            ecl << pair{0x528, 3200}; // Set Health
            ecl << pair{0x548, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST5_BOSS5:
            ECLStdExec(ecl, 0x96ec, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ecl << pair{0x890, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x1b9c, (int16_t)30}; // Disable Invincible
            ecl << pair{0x1c7c, (int16_t)0} << pair{0x1da8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1f0c, (int16_t)0};
            break;
        case THPrac::TH18::TH18_ST5_BOSS6:
            ECLStdExec(ecl, 0x96ec, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x430, 0x518, 1); // Utilize Spell Practice Jump
            ecl << pair{0x528, 3000}; // Set Health
            ecl << pair{0x548, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST5_BOSS7:
            ECLStdExec(ecl, 0x96ec, 1, 1);
            ECLJump(ecl, 0, 0x9c58, 60); // 0x9b94, 0x9bd8, 0x9c58
            ECLJump(ecl, 0x4a44, 0x4a94, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x430, 0x518, 1); // Utilize Spell Practice Jump
            ecl << pair{0x528, 4500}; // Set Health
            ecl << pair{0x548, (int8_t)0x34}; // Set Spell Ordinal
            break;


        case THPrac::TH18::TH18_ST6_MID1:
            ECLStdExec(ecl, 0x7e70, 1, 1);
            ECLJump(ecl, 0, 0x8318, 60); // 0x8318, 0x835c, 0x83dc
            ecl.SetFile(3);
            ecl << pair{0x324, (int16_t)100};
            ecl.SetPos(0x3ac);
            ecl << 0 << 0x002401ff << 0x01ff0000 << 0 << 14000;
            break;
        case THPrac::TH18::TH18_ST6_BOSS1:
            ECLStdExec(ecl, 0x7e70, 2, 1);
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
            ECLStdExec(ecl, 0x7e70, 2, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ECLJump(ecl, 0x5d8, 0x6c0, 2); // Utilize Spell Practice Jump
            ecl << pair{0x6d0, 2000}; // Set Health
            ecl << pair{0x6f0, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST6_BOSS3:
            ECLStdExec(ecl, 0x7e70, 2, 1);
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
            ECLStdExec(ecl, 0x7e70, 2, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ECLJump(ecl, 0x5d8, 0x6c0, 2); // Utilize Spell Practice Jump
            ecl << pair{0x6d0, 4500}; // Set Health
            ecl << pair{0x6f0, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST6_BOSS5:
            ECLStdExec(ecl, 0x7e70, 3, 1);
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
            ECLStdExec(ecl, 0x7e70, 3, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ECLJump(ecl, 0x5d8, 0x6c0, 2); // Utilize Spell Practice Jump
            ecl << pair{0x6d0, 3000}; // Set Health
            ecl << pair{0x6f0, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST6_BOSS7:
            ECLStdExec(ecl, 0x7e70, 4, 1);
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
            ECLStdExec(ecl, 0x7e70, 4, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ECLJump(ecl, 0x5d8, 0x6c0, 2); // Utilize Spell Practice Jump
            ecl << pair{0x6d0, 2700}; // Set Health
            ecl << pair{0x6f0, (int8_t)0x34}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST6_BOSS9:
            ECLStdExec(ecl, 0x7e70, 5, 1);
            ECLJump(ecl, 0, 0x83dc, 60); // 0x8318, 0x835c, 0x83dc
            ECLJump(ecl, 0x5a94, 0x5ae4, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x488, 0x504, 2); // Skip dialogue
            ECLJump(ecl, 0x5d8, 0x6c0, 2); // Utilize Spell Practice Jump
            ecl << pair{0x6d0, 3500}; // Set Health
            ecl << pair{0x6f0, (int8_t)0x35}; // Set Spell Ordinal
            break;
        case THPrac::TH18::TH18_ST6_BOSS10:
            ECLStdExec(ecl, 0x7e70, 5, 1);
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
            ECLStdExec(ecl, 0xa8fc, 1, 1);
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
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xafa0, 124); // 0xafa0, 0xafe4, 0xb064
            ecl.SetFile(3);
            ecl << pair{0x2d0, 99999} << pair{0x2d8, (int16_t)0} << pair{0x368, 90};
            ecl.SetPos(0x388);
            ecl << 0 << 0x00140017 << 0x01ff0000 << 0 << 90;
            ECLJump(ecl, 0, 0x4c4, 0);
            break;
        case THPrac::TH18::TH18_ST7_MID3:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xafa0, 124); // 0xafa0, 0xafe4, 0xb064
            ecl.SetFile(3);
            ecl << pair{0x2d0, 99999} << pair{0x2d8, (int16_t)0} << pair{0x368, 90};
            ecl.SetPos(0x388);
            ecl << 0 << 0x00140017 << 0x01ff0000 << 0 << 90;
            ECLJump(ecl, 0, 0x560, 0);
            break;
        case THPrac::TH18::TH18_ST7_END_NS1:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0xb050, 124); // 0xafa0, 0xafe4, 0xb064
            else {
                ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
                ECLJump(ecl, 0x57cc, 0x585c, 3);
                ecl.SetFile(2);
                ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            }
            break;
        case THPrac::TH18::TH18_ST7_END_S1:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ECLJump(ecl, 0x80c, 0x8ac, 9); // Utilize Spell Practice Jump
            break;
        case THPrac::TH18::TH18_ST7_END_NS2:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ecl << pair{0x118c, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1cbc, (int16_t)0} << pair{0x1de8, (int16_t)0}; // Disable Item Drops & SE
            break;
        case THPrac::TH18::TH18_ST7_END_S2:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ECLJump(ecl, 0x80c, 0x948, 9); // Utilize Spell Practice Jump
            break;
        case THPrac::TH18::TH18_ST7_END_NS3:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ecl << pair{0x118c, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x2404, (int16_t)0}; // Disable Invincible
            ecl << pair{0x24dc, (int16_t)0} << pair{0x2608, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x2770, 0x27a4, 0); // Skip wait
            break;
        case THPrac::TH18::TH18_ST7_END_S3:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ECLJump(ecl, 0x80c, 0x9e4, 9); // Utilize Spell Practice Jump
            break;
        case THPrac::TH18::TH18_ST7_END_NS4:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ecl << pair{0x118c, (int8_t)0x34}; // Change Nonspell
            ecl << pair{0x2c8c, (int16_t)0}; // Disable Invincible
            ecl << pair{0x2d64, (int16_t)0} << pair{0x2e90, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x2ff8, 0x303c, 0); // Skip wait
            break;
        case THPrac::TH18::TH18_ST7_END_S4:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ECLJump(ecl, 0x80c, 0xa80, 9); // Utilize Spell Practice Jump
            break;
        case THPrac::TH18::TH18_ST7_END_NS5:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ecl << pair{0x118c, (int8_t)0x35}; // Change Nonspell
            ecl << pair{0x3524, (int16_t)0}; // Disable Invincible
            ecl << pair{0x35fc, (int16_t)0} << pair{0x3728, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x3890, 0x38c4, 0); // Skip wait
            break;
        case THPrac::TH18::TH18_ST7_END_S5:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ECLJump(ecl, 0x80c, 0xb1c, 9); // Utilize Spell Practice Jump
            break;
        case THPrac::TH18::TH18_ST7_END_NS6:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ecl << pair{0x118c, (int8_t)0x36}; // Change Nonspell
            ecl << pair{0x3dac, (int16_t)0}; // Disable Invincible
            ecl << pair{0x3eb4, (int16_t)0} << pair{0x3fe0, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x4158, 0x418c, 0); // Skip wait
            break;
        case THPrac::TH18::TH18_ST7_END_S6:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ECLJump(ecl, 0x80c, 0xbb8, 9); // Utilize Spell Practice Jump

            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xc690, 1} << pair{0xc690, 1} << pair{0xc698, 60.0f};
                break;
            default:
                break;
            }
            break;
        case THPrac::TH18::TH18_ST7_END_NS7:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ecl << pair{0x118c, (int8_t)0x37}; // Change Nonspell
            ecl << pair{0x4674, (int16_t)0}; // Disable Invincible
            ecl << pair{0x474c, (int16_t)0} << pair{0x4878, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x49e0, 0x4a24, 0); // Skip wait
            break;
        case THPrac::TH18::TH18_ST7_END_S7:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ECLJump(ecl, 0x80c, 0xc54, 9); // Utilize Spell Practice Jump
            break;
        case THPrac::TH18::TH18_ST7_END_NS8:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ecl << pair{0x118c, (int8_t)0x38}; // Change Nonspell
            ecl << pair{0x4f0c, (int16_t)0}; // Disable Invincible
            ecl << pair{0x4fe4, (int16_t)0} << pair{0x5110, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x5278, 0x52bc, 0); // Skip wait
            break;
        case THPrac::TH18::TH18_ST7_END_S8:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ECLJump(ecl, 0x80c, 0xcf0, 9); // Utilize Spell Practice Jump
            break;
        case THPrac::TH18::TH18_ST7_END_S9:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ECLJump(ecl, 0x80c, 0xd8c, 9); // Utilize Spell Practice Jump
            break;
        case THPrac::TH18::TH18_ST7_END_S10:
            ECLStdExec(ecl, 0xa8fc, 1, 1);
            ECLJump(ecl, 0, 0xb064, 124); // 0xafa0, 0xafe4, 0xb064
            ECLJump(ecl, 0x57cc, 0x585c, 3);

            ecl.SetFile(2);
            ECLJump(ecl, 0x718, 0x758, 9); // Skip dialogue
            ECLJump(ecl, 0x80c, 0xe28, 9); // Utilize Spell Practice Jump

            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xe38, 9000};
                ECLJump(ecl, 0x68f4, 0x69a0, 90);
                break;
            case 2:
                ecl << pair{0xe38, 6500};
                ECLJump(ecl, 0x68f4, 0x6a4c, 90);
                break;
            case 3:
                ecl << pair{0xe38, 4000};
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

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th18_everlasting_bgm, 0x477a50, 1, {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        // 4th stack item = i32 call_addr

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && (thPracParam.mode == 1) && thPracParam.section;
        is_practice = (*((int32_t*)0x4cccc8) & 0x1);
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
            THAdvOptWnd::singleton().RestartFix();
            thPracParam._playLock = true;
        });

        if (thPracParam.mode != 1)
            return;

#define R(name)                                                                                                                 \
    card->_recharge_timer.current = static_cast<int32_t>(card->recharge_time * (static_cast<float>(thPracParam.name) / 10000)); \
    card->_recharge_timer.current_f = card->recharge_time * (thPracParam.name / 10000.0f)

        *(int32_t*)(0x4cccfc) = (int32_t)(thPracParam.score / 10);
        *(int32_t*)(0x4ccd48) = thPracParam.life;
        *(int32_t*)(0x4ccd4c) = thPracParam.life_fragment;
        *(int32_t*)(0x4ccd58) = thPracParam.bomb;
        *(int32_t*)(0x4ccd5c) = thPracParam.bomb_fragment;
        *(int32_t*)(0x4ccd38) = thPracParam.power;
        *(int32_t*)(0x4ccd30) = *(int32_t*)(0x4ccd34) = thPracParam.funds;

        auto* ability_manager = *(AbilityManager**)ABILTIY_MANAGER_PTR;

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
                static_cast<CardLily*>(card)->count = thPracParam.lily_count;
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
    }, .data = PatchHookImpl(1) },
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
    EHOOK_DY(th18_add_card, 0x411460, 1, {
        uint32_t* list = nullptr;
        uint32_t sub_count = 0;
        uint32_t cardAddId = *(uint32_t*)(pCtx->Esp + 4);
        uint32_t cardAddType = 0;
        uint8_t cardIdArray[64] = {};

        for (uint32_t* i = (uint32_t*)GetMemContent(ABILTIY_MANAGER_PTR, 0x1c); i; i = (uint32_t*)i[1]) {
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
        asm_call<0x408c30, Fastcall>(*(uint32_t*)ABILTIY_MANAGER_PTR);
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
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen() || THGuiSP::singleton().IsOpen();
        GameGuiEnd(drawCursor);
    })
    EHOOK_DY(th18_render, 0x401510, 1, {
        GameGuiRender(IMPL_WIN32_DX9);
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext()) {
            return;
        }
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4ccdf8, 0x568c30,
            Gui::INGAGME_INPUT_GEN2, 0x4ca21c, 0x4ca218, 0,
            -2, *(float*)0x56aca0, 0.0f);

        SetDpadHook(0x4016EF, 3);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiPrac::singleton();
        THGuiPrac::singleton();
        THGuiPrac::singleton();

        // Hooks
        EnableAllHooks(THMainHook);

        // Reset thPracParam
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
}
}
