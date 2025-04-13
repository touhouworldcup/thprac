#include "thprac_games.h"
#include "thprac_utils.h"
#include <queue>


namespace THPrac {
namespace TH185 {
    const char* card_names_en[] = {
        "None",
        "Blank Card",
        "Money Comes and Goes on its Own",
        "Money Comes and Goes on its Own",
        "Life Card",
        "Ringo Brand Dumplings",
        "Phoenix's Tail",
        "Yin-Yang Orb",
        "Yin-Yang Orb (Needle)",
        "Mini-Hakkero",
        "Mini-Hakkero (Missile)",
        "Maid Knife",
        "Maid Knife (Ricochet)",
        "Safe Return Amulet",
        "Shed Snakeskin Amulet",
        "Half of a Half-Ghost",
        "Shanghai Doll",
        "Ice Fairy",
        "Back Door",
        "Annoying UFO",
        "Miser's Advice",
        "Offerings to the Mountain God",
        "Life Explosion Elixir",
        "Fraudster Rabbit's Foot",
        "Law of the Survival of the Fittest",
        "Sutra of Dharmatic Power",
        "Pebble Hat",
        "Bursting Red Frog",
        "Gale Geta",
        "Idol Defense Corps",
        "Princess Kaguya's Secret Stash",
        "Reliable Tanuki Apprentice",
        "Danmaku Ghost",
        "Kiketsu Matriarch's Threat",
        "Money is the Best Lawyer in Hell",
        "Physical Enhancement Jizo",
        "Magician's Basic Study",
        "Manekineko with Good Business Skills",
        "Yamawaro Shopping Technique",
        "Dragon Pipe",
        "Screen Border",
        "Miracle Mallet",
        "Keystone of Endurance",
        "Lunatic Moon",
        "Esteemed Authority",
        "Vampire Fang",
        "Subterranean Sun",
        "Item Season",
        "Heavy Bass Drum",
        "Psychokinesis",
        "Ancient Magatama",
        "Spirit Power Sample Bottle",
        "Great Tengu's Barley Rice",
        "Gluttonous Centipede",
        "Offensive Jeweled Pagoda",
        "Peaceful Radiance",
        "Maddening Stillness",
        "Lively Powerlessness",
        "Aesthetics of Destruction",
        "The Dragon's Path",
        "Ferocious Komainu",
        "Irresistible Feathered Fan",
        "Soot-covered Uchiwa",
        "No Problem for a Creator!",
        "The Rabbit Has Landed",
        "Sheep You Want to Count",
        "Pristine Self-Confidence",
        "Capitalist's Dilemma",
        "Hundredth Black Market",
        "Bumper Crop of Life",
        "Stimulating Scales",
        "Yamanba's Kitchen Knife",
        "Smelt's Scales",
        "Freewheeling Severed Head",
        "Drenched Giant Snake",
        "Too-Honest Signpost",
        "Hasty Detour of the Three Crossings",
        "Laid-Back Invertebrate",
        "Half-Ghost's Spare Part",
        "Nimble Fabric",
        "Keystone Missile",
        "Life-Burning Torch",
        "Indominable Meathead",
        "Drunkenly Whimsical Ibuki Gourd",
        "Teacup Reimu",
        "Teacup Marisa",
    };
    const char* card_names_cn[] = {
        "无",
        "空白卡牌",
        "财富流至门前来",
        "财富流至门前来",
        "生命卡牌",
        "铃瑚团子",
        "不死鸟之尾",
        "阴阳玉",
        "阴阳玉（针）",
        "迷你八卦炉",
        "迷你八卦炉（导弹）",
        "Maid Knife(女仆飞刀)",
        "Maid Knife（跳弹）",
        "平安归来蛙护身符",
        "放入蛇蜕的护身符",
        "半灵的一半",
        "上海人偶",
        "Ice Fairy(冰之妖精)",
        "背后之门",
        "恼人的UFO",
        "守财奴的教训",
        "向神山的贡品",
        "生命爆炸之药",
        "欺诈兔脚",
        "弱肉强食之理",
        "法力经典",
        "小石子帽子",
        "爆裂赤蛙",
        "疾风木屐",
        "偶像防卫队",
        "辉夜姬的秘密宝箱",
        "可靠的弟子狸",
        "弹幕的亡灵",
        "鬼杰组长的恐吓",
        "有钱能使鬼推磨",
        "肉体强化地藏",
        "魔法使的基础魔法",
        "擅长做生意的招财猫",
        "山童式购物术",
        "Dragon Khsier(龙之烟管)",
        "画面的界线",
        "万宝槌",
        "忍耐的要石",
        "狂气之月",
        "崇高的威光",
        "Vampire Fang(吸血鬼之牙)",
        "地底的太阳",
        "道具的季节",
        "重低音大太鼓",
        "Psychokinesis(超能念力)",
        "太古的勾玉",
        "灵力的标本瓶",
        "大天狗的麦饭",
        "暴食的蜈蚣",
        "攻击性宝塔",
        "静谧的闪耀",
        "疯狂的寂静",
        "熙攘的无力感",
        "破坏的美学",
        "龙脉",
        "关门放狛犬",
        "难以抗拒的朱莉安娜羽扇",
        "烟熏的团扇",
        "对造形神来说很简单的事",
        "着陆之兔",
        "想要去数的羊",
        "纯粹的自我肯定感",
        "资本主义的困境",
        "第一百次黑市",
        "生命的丰穰",
        "刺激性的鱗粉",
        "山姥的菜刀",
        "若鹭的鱼鳞",
        "自由自在的飞头",
        "湿润的巨蛇",
        "太过正直的路标",
        "急渡三途路远甚",
        "任性的无脊椎动物",
        "备用半灵",
        "闪避布",
        "要石导弹",
        "点燃生命的火把",
        "不屈的简单头脑",
        "耍酒疯的伊吹瓢",
        "茶杯灵梦",
        "茶杯魔理沙",
    };
    const char* card_names_jp[] = {
        "なし",
        "空白のカード",
        "勝手に天下の回り物",
        "勝手に天下の回り物",
        "命のカード",
        "鈴瑚印の団子",
        "不死鳥の尾",
        "陰陽玉",
        "陰陽玉（針）",
        "ミニ八卦炉",
        "ミニ八卦炉（ミサイル）",
        "メイドナイフ",
        "メイドナイフ（跳弾）",
        "無事かえるお守り",
        "蛇の抜け殻入りお守り",
        "半霊の半分",
        "上海人形",
        "アイスフェアリー",
        "背中の扉",
        "鬱陶しいＵＦＯ",
        "守銭奴の教訓",
        "神山への供物",
        "生命爆発の薬",
        "詐欺うさぎの足",
        "弱肉強食の理",
        "法力経典",
        "小石ころ帽子",
        "はじける赤蛙",
        "疾風の下駄",
        "偶像防衛隊",
        "かぐや姫の隠し箱",
        "頼りになる弟子狸",
        "弾幕の亡霊",
        "鬼傑組長の脅嚇",
        "地獄の沙汰も金次第",
        "肉体強化地蔵",
        "魔法使いの基礎勉強",
        "商売上手な招き猫",
        "山童的買い物術",
        "ドラゴンキセル",
        "画面の境界",
        "打ち出の小槌",
        "忍耐の要石",
        "狂気の月",
        "やんごとなき威光",
        "ヴァンパイアファング",
        "地底の太陽",
        "アイテムの季節",
        "重低音バスドラム",
        "サイコキネシス",
        "太古の勾玉",
        "霊力の標本瓶",
        "大天狗の麦飯",
        "暴食のムカデ",
        "攻撃的な宝塔",
        "静かなる輝き",
        "狂おしい静寂",
        "賑やかな無力",
        "破壊の美学",
        "龍の通り道",
        "けしかける狛犬",
        "抗いがたきジュリ扇",
        "煤けた団扇",
        "クリエイターなら容易な事",
        "舞い降りた兎",
        "数えたくなる羊",
        "純粋な自己肯定感",
        "資本主義のジレンマ",
        "百回目のブラックマーケット",
        "生命の豊穣",
        "刺激的な鱗粉",
        "山姥の包丁",
        "わかさぎの鱗",
        "自由気ままな生首",
        "濡れた大蛇",
        "正直すぎる道しるべ",
        "急がば三途の回り道",
        "気ままな無脊椎動物",
        "半霊のスペア",
        "ひらり布",
        "要石ミサイル",
        "命を焚く松明",
        "不屈の脳筋",
        "酔狂の伊吹瓢",
        "ゆのみ霊夢",
        "ゆのみ魔理沙",
    };

    constexpr const size_t card_name_count = elementsof(card_names_en);

    static_assert(elementsof(card_names_cn) == card_name_count);
    static_assert(elementsof(card_names_jp) == card_name_count);

    enum addrs {
        BLACK_MARKET_PTR = 0x4d7ac4,
        CARD_DESC_LIST = 0x4ca370,
        ENEMY_MANAGER = 0x4d7af4,
        GAME_THREAD_PTR = 0x4d7b0c
    };

    bool isItemEnabled = false;
    bool wave_forced = false;

    __declspec(noinline) void AddCard(uint32_t cardId)
    {
        if (cardId < 85) {
            asm_call<0x414F20, Thiscall>(*(uint32_t*)0x4d7ab8, cardId, 2);
        }
    }

    struct THPracParam {
        int32_t mode;

        int32_t bulletMoney;
        int32_t funds;
        int32_t life;
        int32_t difficulty;

        std::vector<unsigned int> warp;
        std::vector<unsigned int> additional_cards;
    };
    THPracParam thPracParam {};
    stage_warps_t warps = {};

    bool StageWarpsLoad(size_t stage, stage_warps_t& out) noexcept {
#define FORCE_BOSS(boss_index, start_index)                                                                                                                                                                  \
    0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 0), 0x03 * (boss_index == 0), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index), 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,         \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 1), 0x03 * (boss_index == 1), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 1, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 2), 0x03 * (boss_index == 2), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 2, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x01, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x5E, 0xD9, 0xFF, 0xFF,                                                                              \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                                      \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 3), 0x03 * (boss_index == 3), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 3, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00

#define FORCE_BOSS_C(boss_index, start_index)                                                                                                                                                                \
    0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 0), 0x03 * (boss_index == 0), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index), 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,         \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 1), 0x03 * (boss_index == 1), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 1, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 2), 0x03 * (boss_index == 2), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 2, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 3), 0x03 * (boss_index == 3), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 3, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00

#define BOSS_SPELL_CARD_ASYNC(boss_id, spell_id) 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x24, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x42, 0x6F, 0x73, 0x73, boss_id / 10 + 0x30, boss_id % 10 + 0x30, 0x42, 0x6F, 0x73, 0x73, 0x43, 0x61, 0x72, 0x64, 0x30 + spell_id, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x10, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00

        out = {
            .label = S(TH_PROGRESS),
            .type = stage_warps_t::TYPE_SLIDER,
        };
        switch (stage) {
        case 0:
            out.section_param = {
                {
                    .label = S(TH_TUTORIAL)
                },
                {
                    .label = S(TH185_WAVE_1),
                    .jumps = {
                        { "main", {
                            { .off = 0x188, .dest = 0x230 }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_2),
                    .jumps = {
                        { "main", {
                            { .off = 0x188, .dest = 0x2fc }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x184, .bytes = { 2 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_3),
                    .jumps = {
                        { "main", {
                            { .off = 0x188, .dest = 0x3b8 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x184, .bytes = { 3 } }
                        } }
                    }
                },
                {
                    .label = S(TH_BOSS),
                    .jumps = {
                        { "main", {
                            { .off = 0x188, .dest = 0x488 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x184, .bytes = { 4 } }
                        } }
                    }
                }
            };
            out.section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_DLG)
                    },
                    {
                        .label = S(TH_NONSPELL),
                        .writes {
                            { "Boss01tBoss", {
                                { .off = 0x220, .bytes = { 0x00, 0x00, 0x24, 0x00 } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_0_1),
                        .writes {
                            { "Boss01tBoss", {
                                { .off = 0x220, .bytes = { 0x00, 0x00, 0x24, 0x00 } },
                                { .off = 0x1b0, .bytes = { 0x84, 0x03, 0x00, 0x00 } }
                            } }
                        }
                    }
                }
            };
            break;
        case 1:
            out.section_param = {
                {
                    .label = S(TH185_WAVE_1)
                },
                {
                    .label = S(TH185_WAVE_2),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x380 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_3),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x44c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x03, 0x00, 0x00, 0x00 }}
                        } }
                    }
                },
                {
                    .label = S(TH_BOSS),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x530 }
                        } }
                    }
                }
            };
            out.section_param[3].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM)
                    },
                    {
                        .label = S(TH185_GOUTOKUZI_MIKE),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(0, 2) } }
                            } }
                        }
                    },
                    { 
                        .label = S(TH185_AKI_MINORIKO),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(1, 2) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_ETERNITY_LARVA),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(2, 2) } }
                            } }
                        }
                    },
                    { 
                        .label = S(TH185_SAKATA_NEMUNO),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(3, 2) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[3].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_1_1),
                        .writes = {
                            { "Boss01Boss", {
                                { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss01Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(1, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[3].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_2_1),
                        .writes = {
                            { "Boss02Boss", {
                                { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss02Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(2, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[3].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_3_1),
                        .writes = {
                            { "Boss03Boss", {
                                { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss03Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(3, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[3].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_4_1),
                        .writes = {
                            { "Boss04Boss", {
                                { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss04Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(4, 1) } }
                            } }
                        }
                    }
                }
            };
            break;
        case 2:
            out.section_param = {
                {
                    .label = S(TH185_WAVE_1),
                },
                {
                    .label = S(TH185_WAVE_2),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x380 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_3),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x44c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x03, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_4),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x51c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x04, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                { .label = S(TH_BOSS),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x5fc }
                        } }
                    }
                }
            };
            out.section_param[4].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM)
                    },
                    { 
                        .label = S(TH185_CIRNO),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(0, 6) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_WAKASAGIHIME),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(1, 6) } }
                            } }
                        }
                    },
                    { 
                        .label = S(TH185_SEKIBANKI),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(2, 6) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_USHIZAKI_URUMI),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(3, 6) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[4].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_ATTACK)
                    },
                    {
                        .label = S(TH185_SPELL_5_1),
                        .writes = {
                            { "Boss05Boss", {
                                { .off = 0x1b0, .bytes = { 0xDC, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss05Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(5, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[4].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_6_1),
                        .writes = {
                            { "Boss06Boss", {
                                { .off = 0x1b0, .bytes = { 0xDC, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss06Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(6, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[4].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_7_1),
                        .writes = {
                            { "Boss07Boss", {
                                { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss07Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(7, 1)} }
                            } }
                        }
                    }
                }
            };
            out.section_param[4].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_8_1),
                        .writes = {
                            { "Boss08Boss", {
                                { .off = 0x1b0, .bytes = { 0xDC, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss08Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(8, 1) } }
                            } }
                        }
                    }
                }
            };
            break;
        case 3:
            out.section_param = {
                {
                    .label = S(TH185_WAVE_1)
                },
                {
                    .label = S(TH185_WAVE_2),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x380 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_3),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x44c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x03, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_4),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x51c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x04, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_5),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x5e8 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x05, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH_BOSS),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x6cc }
                        } }
                    },
                }
            };
            out.section_param[5].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM)
                    },
                    {
                        .label = S(TH185_EBISU_EIKA),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(0, 10) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_NIWATARI_KUTAKA),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(1, 10) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_YATADERA_NARUMI),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(2, 10) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_ONOZUKA_KOMACHI),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(3, 10) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_9_1),
                        .writes = {
                            { "Boss09Boss", {
                                { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                            } },
                            { "Boss09Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(9, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_10_1),
                        .writes = {
                            { "Boss10Boss", {
                                { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                            } },
                            { "Boss10Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(10, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_11_1),
                        .writes = {
                            { "Boss11Boss", {
                                { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                            } },
                            { "Boss11Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(11, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_12_1),
                        .writes = {
                            { "Boss12Boss", {
                                { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                            } },
                            { "Boss12Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(12, 1) } }
                            } }
                        }
                    }
                }
            };
            break;
        case 4:
            out.section_param = {
                {
                    .label = S(TH185_WAVE_1),
                },
                {
                    .label = S(TH185_WAVE_2),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x380 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_3),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x44c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x03, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_4),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x518 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x04, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_5),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x5e4 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x05, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH_BOSS),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x6c4 }
                        } }
                    },
                }
            };
            out.section_param[5].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM)
                    },
                    {
                        .label = S(TH185_KOCHIYA_SANAE),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(0, 14) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_IZAYOI_SAKUYA),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(1, 14) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_KONPAKU_YOUMU),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(2, 14) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_HAKUREI_REIMU),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(3, 14) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_KAWASIRO_NITORI),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 15, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 16, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x01, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x5E, 0xD9, 0xFF, 0xFF,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0xF3, 0x03, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 27, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_13_1),
                        .writes = {
                            { "Boss13Boss", {
                                { .off = 0x1b0, .bytes = { 0x98, 0x08, 0x00, 0x00 } }
                            } },
                            { "Boss13Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(13, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_14_1),
                        .writes = {
                            { "Boss14Boss", {
                                { .off = 0x1b0, .bytes = { 0x98, 0x08, 0x00, 0x00 } }
                            } },
                            { "Boss14Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(14, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_15_1),
                        .writes = {
                            { "Boss15Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss15Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(15, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_16_1),
                        .writes = {
                            { "Boss16Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss16Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(16, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[5].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_DLG)
                    },
                    {
                        .label = S(TH_NONSPELL),
                        .writes = {
                            { "Boss26Boss", {
                                { .off = 0x208, .bytes = {  0x00, 0x00, 0x24, 0x00 } }
                            } },
                        }
                    },
                    {
                        .label = S(TH185_SPELL_26_1),
                        .writes = {
                            { "Boss26Boss", {
                                { .off = 0x1b0, .bytes = { 0xa0, 0x0f, 0x00, 0x00 } },
                                { .off = 0x208, .bytes = { 0x00, 0x00, 0x24, 0x00 } }
                            } },
                            { "Boss26Boss1", {
                                { .off = 0x10, .bytes = {
                                    0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x44, 0x80, 0xA0, 0xFF, 0xFF,
                                    BOSS_SPELL_CARD_ASYNC(26, 1)
                                }
                            } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_26_2),
                        .writes = {
                            { "Boss26Boss", {
                                { .off = 0x1b0, .bytes = { 0xd0, 0x07, 0x00, 0x00 } },
                                { .off = 0x208, .bytes = { 0x00, 0x00, 0x24, 0x00 } }
                            } },
                            { "Boss26Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(26, 2) } }
                            } }
                        }
                    }
                }
            };
            break;
        case 5:
            out.section_param = {
                {
                    .label = S(TH185_WAVE_1)
                },
                {
                    .label = S(TH185_WAVE_2),
                    .jumps = {
                        { "main", {
                            { .off = 0x28c, .dest = 0x3b4 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x28a, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_3),
                    .jumps = {
                        { "main", {
                            { .off = 0x28c, .dest = 0x480 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x28a, .bytes = { 0x03, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_4),
                    .jumps = {
                        { "main", {
                            { .off = 0x28c, .dest = 0x54c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x28a, .bytes = { 0x04, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_5),
                    .jumps = {
                        { "main", {
                            { .off = 0x28c, .dest = 0x618 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x28a, .bytes = { 0x05, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_6),
                    .jumps = {
                        { "main", {
                            { .off = 0x28c, .dest = 0x6e4 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x28a, .bytes = { 0x06, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_7),
                    .jumps = {
                        { "main", {
                            { .off = 0x28c, .dest = 0x7b0 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x28a, .bytes = { 0x07, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH_BOSS),
                    .jumps = {
                        { "main", {
                            { .off = 0x28c, .dest = 0x890 }
                        } }
                    },
                }
            };
            out.section_param[7].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM)
                    },
                    {
                        .label = S(TH185_KUDAMAKI_TSUKASA),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(0, 18) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_IIZUNAMARU_MEGUMU),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(1, 18) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_CLOWNPIECE),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(2, 18) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_HINANAWI_TENSHI),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(3, 18) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_17_1),
                        .writes = {
                            { "Boss17Boss", {
                                { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                            } },
                            { "Boss17Boss1", {
                                { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(17, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_17_2),
                        .writes = {
                            { "Boss17Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss17Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(17, 2) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_18_1),
                        .writes = {
                            { "Boss18Boss", {
                                { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                            } },
                            { "Boss18Boss1", {
                                { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(18, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_18_2),
                        .writes = {
                            { "Boss18Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss18Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(18, 2) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_19_1),
                        .writes = {
                            { "Boss19Boss", {
                                { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                            } },
                            { "Boss19Boss1", {
                            { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(19, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_19_2),
                        .writes = {
                            { "Boss19Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss19Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(19, 2) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_20_1),
                        .writes = {
                            { "Boss20Boss", {
                                { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                            } },
                            { "Boss20Boss1", {
                            { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(20, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_20_2),
                        .writes = {
                            { "Boss20Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss20Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(20, 2) } }
                            } }
                        }
                    }
                }
            };
            break;
        case 6:
            out.section_param = {
                {
                    .label = S(TH185_WAVE_1)
                },
                {
                    .label = S(TH185_WAVE_2),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x380 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_3),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x44c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x03, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_4),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x518 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x04, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_5),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x5e4 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x05, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_6),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x6b0 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x06, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_7),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x77c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x254, .bytes = { 0x07, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH_BOSS),
                    .jumps = {
                        { "main", {
                            { .off = 0x258, .dest = 0x84c }
                        } }
                    },
                }
            };
            out.section_param[7].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM)
                    },
                    {
                        .label = S(TH185_IBUKI_SUIKA),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(0, 22) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_FUTATUIWA_MAMIZOU),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(1, 22) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_KUROKOMA_SAKI),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(2, 22) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_HIMEMUSHI_MOMOYO),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { FORCE_BOSS(3, 22) } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_YAMASHIRO_TAKANE),
                        .writes = {
                            { "WorldWaveB00", {
                                { .off = 0x34, .bytes = { {
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x01, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x5E, 0xD9, 0xFF, 0xFF,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0xF3, 0x03, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00,   28, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 } }
                            } }
                        } }
                    }
                }
            };
            out.section_param[7].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_21_1),
                        .writes = {
                            { "Boss21Boss", {
                                { .off = 0x1b0, .bytes = { 0xa0, 0x0f, 0x00, 0x00 } }
                            } },
                            { "Boss21Boss1", {
                            { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x44, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(21, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_21_2),
                        .writes = {
                            { "Boss21Boss", {
                                { .off = 0x1b0, .bytes = { 0xb0, 0x04, 0x00, 0x00 } }
                            } },
                            { "Boss21Boss1", {
                            { .off = 0x10, .bytes = {
                                BOSS_SPELL_CARD_ASYNC(21, 2)
                            } } } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_22_1),
                        .writes = {
                            { "Boss22Boss", {
                                { .off = 0x1b0, .bytes = { 0xd4, 0x17, 0x00, 0x00 } }
                            } },
                            { "Boss22Boss1", {
                            { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0x4E, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(22, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_22_2),
                        .writes = {
                            { "Boss22Boss", {
                                { .off = 0x1b0, .bytes = { 0xe4, 0x0c, 0x00, 0x00 } }
                            } },
                            { "Boss22Boss1", {
                            { .off = 0x10, .bytes = {
                                BOSS_SPELL_CARD_ASYNC(22, 2)
                            } } } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_23_1),
                        .writes = {
                            { "Boss23Boss", {
                                { .off = 0x1b0, .bytes = { 0xd4, 0x17, 0x00, 0x00 } }
                            } },
                            { "Boss23Boss1", {
                            { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0x4E, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(23, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_23_2),
                        .writes = {
                            { "Boss23Boss", {
                                { .off = 0x1b0, .bytes = { 0xe4, 0x0c, 0x00, 0x00 } }
                            } },
                            { "Boss23Boss1", {
                            { .off = 0x10, .bytes = {
                                BOSS_SPELL_CARD_ASYNC(23, 2)
                            } } } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_24_1),
                        .writes = {
                            { "Boss24Boss", {
                                { .off = 0x1b0, .bytes = { 0xd4, 0x17, 0x00, 0x00 } }
                            } },
                            { "Boss24Boss1", {
                            { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0x4E, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(24, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_24_2),
                        .writes = {
                            { "Boss24Boss", {
                                { .off = 0x1b0, .bytes = { 0xe4, 0x0c, 0x00, 0x00 } }
                            } },
                            { "Boss24Boss1", {
                            { .off = 0x10, .bytes = {
                                BOSS_SPELL_CARD_ASYNC(24, 2)
                            } } } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[5].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_DLG)
                    },
                    {
                        .label = S(TH_NONSPELL),
                        .writes = {
                            { "Boss27Boss", {
                                { .off = 0x208, .bytes = { 0, 0 } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_27_1),
                        .writes = {
                            { "Boss27Boss", {
                                { .off = 0x208, .bytes = { 0, 0 } },
                                { .off = 0x1b0, .bytes = { 0x88, 0x2c, 0x00, 0x00 } }
                            } },
                            { "Boss27Boss1", {
                                { .off = 0x10, .bytes = {
                                    0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF3, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                    0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                    BOSS_SPELL_CARD_ASYNC(27, 1)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_27_2),
                        .writes = {
                            { "Boss27Boss", {
                                { .off = 0x208, .bytes = { 0, 0 } },
                                { .off = 0x1b0, .bytes = { 0x78, 0x1e, 0x00, 0x00 } }
                            } },
                            { "Boss27Boss1", {
                                { .off = 0x10, .bytes = {
                                    0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                    BOSS_SPELL_CARD_ASYNC(27, 2)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_27_3),
                        .writes = {
                            { "Boss27Boss", {
                                { .off = 0x208, .bytes = { 0, 0 } },
                                { .off = 0x1b0, .bytes = { 0xa0, 0x0f, 0x00, 0x00 } }
                            } },
                            { "Boss27Boss1", {
                                { .off = 0x10, .bytes = {
                                    BOSS_SPELL_CARD_ASYNC(27, 3)
                                } }
                            } }
                        }
                    }
                }
            };
            break;
        case 7:
            out.section_param = {
                {
                    .label = S(TH185_WAVE_1)
                },
                {
                    .label = S(TH185_WAVE_2),
                    .jumps = {
                        { "main", {
                            { .off = 0x29c, .dest = 0x3d4 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x288, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_3),
                    .jumps = {
                        { "main", {
                            { .off = 0x29c, .dest = 0x4b0 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x288, .bytes = { 0x03, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_4),
                    .jumps = {
                        { "main", {
                            { .off = 0x29c, .dest = 0x58c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x288, .bytes = { 0x04, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_TENKYU_CHIMATA),
                    .jumps = {
                        { "main", {
                            { .off = 0x29c, .dest = 0x67c }
                        } }
                    },
                }
            };
            out.section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_DLG)
                    },
                    {
                        .label = S(TH_NONSPELL),
                        .writes = {
                            { "Boss25Boss", {
                                { .off = 0x208, .bytes = { 0, 0 } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_25_1),
                        .writes = {
                            { "Boss25Boss", {
                                { .off = 0x208, .bytes = { 0, 0 } },
                                { .off = 0x1b0, .bytes = { 0x58, 0x1b, 0x00, 0x00 } }
                            } },
                            { "Boss25Boss1", {
                                { .off = 0x10, .bytes = {
                                    0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                    BOSS_SPELL_CARD_ASYNC(25, 1)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_25_2),
                        .writes = {
                            { "Boss25Boss", {
                                { .off = 0x208, .bytes = { 0, 0 } },
                                { .off = 0x1b0, .bytes = { 0xa0, 0x0f, 0x00, 0x00 } }
                            } },
                            { "Boss25Boss1", {
                                { .off = 0x10, .bytes = {
                                    BOSS_SPELL_CARD_ASYNC(25, 2)
                                } }
                            } }
                        }
                    }
                }
            };
            break;
        case 8:
            out.section_param = {
                {
                    .label = S(TH185_WAVE_1),
                },
                {
                    .label = S(TH185_WAVE_2),
                    .jumps = {
                        { "main", {
                            { .off = 0x434, .dest = 0x52c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x3f0, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_3),
                    .jumps = {
                        { "main", {
                            { .off = 0x434, .dest = 0x628 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x3f0, .bytes = { 0x03, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_4),
                    .jumps = {
                        { "main", {
                            { .off = 0x434, .dest = 0x6d8 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x3f0, .bytes = { 0x04, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_5),
                    .jumps = {
                        { "main", {
                            { .off = 0x434, .dest = 0x7d4 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x3f0, .bytes = { 0x05, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_6),
                    .jumps = {
                        { "main", {
                            { .off = 0x434, .dest = 0x884 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x3f0, .bytes = { 0x06, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_7),
                    .jumps = {
                        { "main", {
                            { .off = 0x434, .dest = 0x980 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x3f0, .bytes = { 0x07, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_8),
                    .jumps = {
                        { "main", {
                            { .off = 0x434, .dest = 0xa30 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x3f0, .bytes = { 0x08, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_9),
                    .jumps = {
                        { "main", {
                            { .off = 0x434, .dest = 0xb2c }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x3f0, .bytes = { 0x09, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_10),
                    .jumps = {
                        { "main", {
                            { .off = 0x434, .dest = 0xbdc }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x3f0, .bytes = { 0x0a, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_11),
                    .jumps = {
                        { "main", {
                            { .off = 0x434, .dest = 0xcd8 }
                        } }
                    },
                    .writes = {
                        { "main", {
                            { .off = 0x3f0, .bytes = { 0x0b, 0x00, 0x00, 0x00 } }
                        } }
                    }
                },
                {
                    .label = S(TH185_WAVE_12),
                    .jumps = {
                        { "main", {
                            { .off = 0x434, .dest = 0xd88 }
                        } }
                    }
                },
            };
            out.section_param[1].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM),
                    },
                    {
                        .label = S(TH185_GOUTOKUZI_MIKE),
                        .writes = {
                            { "WorldWaveB01", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(0, 2)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_AKI_MINORIKO),
                        .writes = {
                            { "WorldWaveB01", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(1, 2)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_ETERNITY_LARVA),
                        .writes = {
                            { "WorldWaveB01", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(2, 2)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_SAKATA_NEMUNO),
                        .writes = {
                            { "WorldWaveB01", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(3, 2)
                                } }
                            } }
                        }
                    }
                }
            };
            out.section_param[3].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM)
                    },
                    {
                        .label = S(TH185_CIRNO),
                        .writes = {
                            { "WorldWaveB02", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(0, 6)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_WAKASAGIHIME),
                        .writes = {
                            { "WorldWaveB02", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(1, 6)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_SEKIBANKI),
                        .writes = {
                            { "WorldWaveB02", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(2, 6)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_USHIZAKI_URUMI),
                        .writes = {
                            { "WorldWaveB02", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(3, 6)
                                } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM)
                    },
                    {
                        .label = S(TH185_EBISU_EIKA),
                        .writes = {
                            { "WorldWaveB03", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(0, 10)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_NIWATARI_KUTAKA),
                        .writes = {
                            { "WorldWaveB03", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(1, 10)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_YATADERA_NARUMI),
                        .writes = {
                            { "WorldWaveB03", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(2, 10)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_ONOZUKA_KOMACHI),
                        .writes = {
                            { "WorldWaveB03", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(3, 10)
                                } }
                            } }
                        }
                    }
                }
            };
            out.section_param[7].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM)
                    },
                    {
                        .label = S(TH185_KOCHIYA_SANAE),
                        .writes = {
                            { "WorldWaveB04", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(0, 14)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_IZAYOI_SAKUYA),
                        .writes = {
                            { "WorldWaveB04", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(1, 14)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_KONPAKU_YOUMU),
                        .writes = {
                            { "WorldWaveB04", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(2, 14)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_HAKUREI_REIMU),
                        .writes = {
                            { "WorldWaveB04", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(3, 14)
                                } }
                            } }
                        }
                    }
                }
            };
            out.section_param[9].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM)
                    },
                    {
                        .label = S(TH185_KUDAMAKI_TSUKASA),
                        .writes = {
                            { "WorldWaveB05", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(0, 18)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_IIZUNAMARU_MEGUMU),
                        .writes = {
                            { "WorldWaveB05", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(1, 18)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_CLOWNPIECE),
                        .writes = {
                            { "WorldWaveB05", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(2, 18)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_HINANAWI_TENSHI),
                        .writes = {
                            { "WorldWaveB05", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(3, 18)
                                } }
                            } }
                        }
                    }
                }
            };
            out.section_param[11].phases = {
                .label = S(TH_BOSS),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH185_NONE_RANDOM)
                    },
                    {
                        .label = S(TH185_IBUKI_SUIKA),
                        .writes = {
                            { "WorldWaveB06", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(0, 22)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_FUTATUIWA_MAMIZOU),
                        .writes = {
                            { "WorldWaveB06", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(1, 22)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_KUROKOMA_SAKI),
                        .writes = {
                            { "WorldWaveB06", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(2, 22)
                                } }
                            } }
                        }
                    },
                    {
                        .label = S(TH185_HIMEMUSHI_MOMOYO),
                        .writes = {
                            { "WorldWaveB06", {
                                { .off = 0x34, .bytes = {
                                    FORCE_BOSS_C(3, 22)
                                } }
                            } }
                        }
                    }
                }
            };
            out.section_param[1].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_1_1),
                        .writes = {
                            { "Boss01Boss", {
                                { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss01Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(1, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[1].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_2_1),
                        .writes = {
                            { "Boss02Boss", {
                                { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss02Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(2, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[1].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_3_1),
                        .writes = {
                            { "Boss03Boss", {
                                { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss03Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(3, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[1].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_4_1),
                        .writes = {
                            { "Boss04Boss", {
                                { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss04Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(4, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[3].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_5_1),
                        .writes = {
                            { "Boss05Boss", {
                                { .off = 0x1b0, .bytes = { 0xDC, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss05Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(5, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[3].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_6_1),
                        .writes = {
                            { "Boss06Boss", {
                                { .off = 0x1b0, .bytes = { 0xDC, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss06Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(6, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[3].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_7_1),
                        .writes = {
                            { "Boss07Boss", {
                                { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss07Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(7, 1)} }
                            } }
                        }
                    }
                }
            };
            out.section_param[3].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_8_1),
                        .writes = {
                            { "Boss08Boss", {
                                { .off = 0x1b0, .bytes = { 0xDC, 0x05, 0x00, 0x00 } }
                            } },
                            { "Boss08Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(8, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_9_1),
                        .writes = {
                            { "Boss09Boss", {
                                { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                            } },
                            { "Boss09Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(9, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_10_1),
                        .writes = {
                            { "Boss10Boss", {
                                { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                            } },
                            { "Boss10Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(10, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_11_1),
                        .writes = {
                            { "Boss11Boss", {
                                { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                            } },
                            { "Boss11Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(11, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[5].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_12_1),
                        .writes = {
                            { "Boss12Boss", {
                                { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                            } },
                            { "Boss12Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(12, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_13_1),
                        .writes = {
                            { "Boss13Boss", {
                                { .off = 0x1b0, .bytes = { 0x98, 0x08, 0x00, 0x00 } }
                            } },
                            { "Boss13Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(13, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_14_1),
                        .writes = {
                            { "Boss14Boss", {
                                { .off = 0x1b0, .bytes = { 0x98, 0x08, 0x00, 0x00 } }
                            } },
                            { "Boss14Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(14, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_15_1),
                        .writes = {
                            { "Boss15Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss15Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(15, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[7].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_16_1),
                        .writes = {
                            { "Boss16Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss16Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(16, 1) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[9].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_17_1),
                        .writes = {
                            { "Boss17Boss", {
                                { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                            } },
                            { "Boss17Boss1", {
                                { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(17, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_17_2),
                        .writes = {
                            { "Boss17Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss17Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(17, 2) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[9].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_18_1),
                        .writes = {
                            { "Boss18Boss", {
                                { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                            } },
                            { "Boss18Boss1", {
                                { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(18, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_18_2),
                        .writes = {
                            { "Boss18Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss18Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(18, 2) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[9].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_19_1),
                        .writes = {
                            { "Boss19Boss", {
                                { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                            } },
                            { "Boss19Boss1", {
                                { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(19, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_19_2),
                        .writes = {
                            { "Boss19Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss19Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(19, 2) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[9].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_20_1),
                        .writes = {
                            { "Boss20Boss", {
                                { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                            } },
                            { "Boss20Boss1", {
                                { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(20, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_20_2),
                        .writes = {
                            { "Boss20Boss", {
                                { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                            } },
                            { "Boss20Boss1", {
                                { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(20, 2) } }
                            } }
                        }
                    }
                }
            };
            out.section_param[11].phases->section_param[1].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_21_1),
                        .writes = {
                            { "Boss21Boss", {
                                { .off = 0x1b0, .bytes = { 0xa0, 0x0f, 0x00, 0x00 } }
                            } },
                            { "Boss21Boss1", {
                            { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x44, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(21, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_21_2),
                        .writes = {
                            { "Boss21Boss", {
                                { .off = 0x1b0, .bytes = { 0xb0, 0x04, 0x00, 0x00 } }
                            } },
                            { "Boss21Boss1", {
                            { .off = 0x10, .bytes = {
                                BOSS_SPELL_CARD_ASYNC(21, 2)
                            } } } }
                        }
                    }
                }
            };
            out.section_param[11].phases->section_param[2].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_22_1),
                        .writes = {
                            { "Boss22Boss", {
                                { .off = 0x1b0, .bytes = { 0xd4, 0x17, 0x00, 0x00 } }
                            } },
                            { "Boss22Boss1", {
                            { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0x4E, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(22, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_22_2),
                        .writes = {
                            { "Boss22Boss", {
                                { .off = 0x1b0, .bytes = { 0x4a, 0x01, 0x00, 0x00 } }
                            } },
                            { "Boss22Boss1", {
                            { .off = 0x10, .bytes = {
                                BOSS_SPELL_CARD_ASYNC(22, 2)
                            } } } }
                        }
                    }
                }
            };
            out.section_param[11].phases->section_param[3].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_23_1),
                        .writes = {
                            { "Boss23Boss", {
                                { .off = 0x1b0, .bytes = { 0xd4, 0x17, 0x00, 0x00 } }
                            } },
                            { "Boss23Boss1", {
                            { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0x4E, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(23, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_23_2),
                        .writes = {
                            { "Boss23Boss", {
                                { .off = 0x1b0, .bytes = { 0xe4, 0x0c, 0x00, 0x00 } }
                            } },
                            { "Boss23Boss1", {
                            { .off = 0x10, .bytes = {
                                BOSS_SPELL_CARD_ASYNC(23, 2)
                            } } } }
                        }
                    }
                }
            };
            out.section_param[11].phases->section_param[4].phases = {
                .label = S(TH_ATTACK),
                .type = stage_warps_t::TYPE_COMBO,
                .section_param = {
                    {
                        .label = S(TH_NONSPELL)
                    },
                    {
                        .label = S(TH185_SPELL_24_1),
                        .writes = {
                            { "Boss24Boss", {
                                { .off = 0x1b0, .bytes = { 0xd4, 0x17, 0x00, 0x00 } }
                            } },
                            { "Boss24Boss1", {
                            { .off = 0x10, .bytes = {
                                0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x44, 0x80, 0xA0, 0xFF, 0xFF,
                                BOSS_SPELL_CARD_ASYNC(24, 1)
                            } } } }
                        }
                    },
                    {
                        .label = S(TH185_SPELL_24_2),
                        .writes = {
                            { "Boss24Boss", {
                                { .off = 0x1b0, .bytes = { 0x4a, 0x01, 0x00, 0x00 } }
                            } },
                            { "Boss24Boss1", {
                            { .off = 0x10, .bytes = {
                                BOSS_SPELL_CARD_ASYNC(24, 2)
                            } } } }
                        }
                    }
                }
            };
            break;
        default:
            return false;
        };
        return true;

#undef FORCE_BOSS
#undef FORCE_BOSS_C
#undef BOSS_SPELL_CARD_ASYNC
    }

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
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mInfLives();
            mInfBMoney();
            mTimeLock();
            mZeroCD();
            mWholesale();
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

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };
        Gui::GuiHotKey mMuteki { TH_MUTEKI, "F1", VK_F1, {
            new HookCtx(0x4635a5, "\x01", 1) } };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2, {
            new HookCtx(0x40aec3, "\x66\x0f\x1f\x44\x00\x00", 6),
            new HookCtx(0x463281, "\x00", 1) } };
        Gui::GuiHotKey mInfBMoney { TH185_INF_BMONEY, "F3", VK_F3, {
            new HookCtx(0x40ed5f, "\x66\x2e\x0f\x1f\x84\x00\x00\x00\x00\x00\x66\x2e\x0f\x1f\x84\x00\x00\x00\x00\x00", 20),
            new HookCtx(0x41ee2d, "\x66\x0f\x1f\x44\x00\x00", 6) } };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F4", VK_F4, {
            new HookCtx(0x434c85, "\x66\x0f\x1f\x44\x00\x00", 6),
            new HookCtx(0x436E38, "\x0f\x1f\x84\x00\x00\x00\x00\x00\x0f\x1f\x84\x00\x00\x00\x00\x00\x0f\x1f\x84\x00\x00\x00\x00\x00", 24) } };
        Gui::GuiHotKey mZeroCD { TH18_ZERO_CD, "F5", VK_F5, {
            new HookCtx(0x462146, [](PCONTEXT pCtx) -> bool {
                struct Timer {
                    int32_t prev;
                    int32_t cur;
                    float cur_f;
                    void* unused;
                    uint32_t control;
                };
                Timer* timer = (Timer*)(pCtx->Ecx + 0x34);
                *timer = { -1, 0, 0, 0, 0 };
                return true;
            }) } };
        Gui::GuiHotKey mWholesale { TH185_WHOLESALE, "F6", VK_F6, {
            new HookCtx(0x41da02, [](PCONTEXT pCtx) -> bool {
                uint8_t wholesale[] = { 0x53, 0x00, 0x00, 0x00, 0xF8, 0xA3, 0x4C, 0x00, 0x3C, 0xA4, 0x4C, 0x00, 0x80, 0xA4, 0x4C, 0x00, 0xC4, 0xA4, 0x4C, 0x00, 0x08, 0xA5, 0x4C, 0x00, 0x4C, 0xA5, 0x4C, 0x00, 0x90, 0xA5, 0x4C, 0x00, 0xD4, 0xA5, 0x4C, 0x00, 0x18, 0xA6, 0x4C, 0x00, 0x5C, 0xA6, 0x4C, 0x00, 0xA0, 0xA6, 0x4C, 0x00, 0xE4, 0xA6, 0x4C, 0x00, 0x28, 0xA7, 0x4C, 0x00, 0x6C, 0xA7, 0x4C, 0x00, 0xB0, 0xA7, 0x4C, 0x00, 0xF4, 0xA7, 0x4C, 0x00, 0x38, 0xA8, 0x4C, 0x00, 0x7C, 0xA8, 0x4C, 0x00, 0xC0, 0xA8, 0x4C, 0x00, 0x04, 0xA9, 0x4C, 0x00, 0x48, 0xA9, 0x4C, 0x00, 0x8C, 0xA9, 0x4C, 0x00, 0xD0, 0xA9, 0x4C, 0x00, 0x14, 0xAA, 0x4C, 0x00, 0x58, 0xAA, 0x4C, 0x00, 0x9C, 0xAA, 0x4C, 0x00, 0xE0, 0xAA, 0x4C, 0x00, 0x24, 0xAB, 0x4C, 0x00, 0x68, 0xAB, 0x4C, 0x00, 0xAC, 0xAB, 0x4C, 0x00, 0xF0, 0xAB, 0x4C, 0x00, 0x34, 0xAC, 0x4C, 0x00, 0x78, 0xAC, 0x4C, 0x00, 0xBC, 0xAC, 0x4C, 0x00, 0x00, 0xAD, 0x4C, 0x00, 0x44, 0xAD, 0x4C, 0x00, 0x88, 0xAD, 0x4C, 0x00, 0xCC, 0xAD, 0x4C, 0x00, 0x10, 0xAE, 0x4C, 0x00, 0x54, 0xAE, 0x4C, 0x00, 0x98, 0xAE, 0x4C, 0x00, 0xDC, 0xAE, 0x4C, 0x00, 0x20, 0xAF, 0x4C, 0x00, 0x64, 0xAF, 0x4C, 0x00, 0xA8, 0xAF, 0x4C, 0x00, 0xEC, 0xAF, 0x4C, 0x00, 0x30, 0xB0, 0x4C, 0x00, 0x74, 0xB0, 0x4C, 0x00, 0xB8, 0xB0, 0x4C, 0x00, 0xFC, 0xB0, 0x4C, 0x00, 0x40, 0xB1, 0x4C, 0x00, 0x84, 0xB1, 0x4C, 0x00, 0xC8, 0xB1, 0x4C, 0x00, 0x0C, 0xB2, 0x4C, 0x00, 0x50, 0xB2, 0x4C, 0x00, 0x94, 0xB2, 0x4C, 0x00, 0xD8, 0xB2, 0x4C, 0x00, 0x1C, 0xB3, 0x4C, 0x00, 0x60, 0xB3, 0x4C, 0x00, 0xA4, 0xB3, 0x4C, 0x00, 0xE8, 0xB3, 0x4C, 0x00, 0x2C, 0xB4, 0x4C, 0x00, 0x70, 0xB4, 0x4C, 0x00, 0xB4, 0xB4, 0x4C, 0x00, 0xF8, 0xB4, 0x4C, 0x00, 0x3C, 0xB5, 0x4C, 0x00, 0x80, 0xB5, 0x4C, 0x00, 0xC4, 0xB5, 0x4C, 0x00, 0x08, 0xB6, 0x4C, 0x00, 0x4C, 0xB6, 0x4C, 0x00, 0x90, 0xB6, 0x4C, 0x00, 0xD4, 0xB6, 0x4C, 0x00, 0x18, 0xB7, 0x4C, 0x00, 0x5C, 0xB7, 0x4C, 0x00, 0xA0, 0xB7, 0x4C, 0x00, 0xE4, 0xB7, 0x4C, 0x00, 0x28, 0xB8, 0x4C, 0x00, 0x6C, 0xB8, 0x4C, 0x00, 0xB0, 0xB8, 0x4C, 0x00, 0xF4, 0xB8, 0x4C, 0x00, 0x38, 0xB9, 0x4C, 0x00, 0x7C, 0xB9, 0x4C, 0x00, 0xC0, 0xB9, 0x4C, 0x00 };
                memcpy((void*)(pCtx->Esi + 0xA40), wholesale, sizeof(wholesale));
                return true;
            }) } };
    };

    stage_warps_t stages[9] = {};

    struct THWaveSelect : public Gui::GameGuiWnd {
        SINGLETON(THWaveSelect);

        THWaveSelect() noexcept
        {
            SetWndFlag(
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);

            SetFade(0.5f, 0.5f);
            SetSizeRel(0.3f, 0.6f);
            SetPosRel(0.35f, 0.2f);
            SetTitle("THWaveSelect");

            th185_replaymanager_on_tick_cutoff.Setup();
        }

        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(S(TH185_FORCE_WAVE));
            ImGui::Separator();

            const ImVec4 green = { 0, 1, 0, 1 };

            if (selectedWave == -1) {
                ImGui::TextColored(green, "[%s]", S(TH185_NONE_RANDOM));
            } else {
                ImGui::TextUnformatted(S(TH185_NONE_RANDOM));
            }

            for (size_t i = 0; i < waveOpts.size(); i++) {
                auto [id, name] = waveOpts[i];
                if (selectedWave == i) {
                    ImGui::TextColored(green, "[%s]", name);
                } else {
                    ImGui::TextUnformatted(name);
                }
            }

            if (Gui::InGameInputGet(VK_DOWN)) {
                if (++selectedWave >= waveOpts.size()) {
                    selectedWave = SIZE_MAX;
                }
            }
            if (Gui::InGameInputGet(VK_UP)) {
                if (selectedWave == SIZE_MAX) {
                    selectedWave = waveOpts.size() - 1;
                } else {
                    --selectedWave;
                }
            }
            if (Gui::InGameInputGet('Z') || Gui::KeyboardInputGetRaw(VK_RETURN)) {
                th185_replaymanager_on_tick_cutoff.Disable();
            }
        }

        public:
        EHOOK_ST(th185_replaymanager_on_tick_cutoff, 0x468764)
        {
            pCtx->Eax = 3;
            return true;
        }

        size_t selectedWave = SIZE_MAX;
        std::vector<std::pair<size_t, const char*>> waveOpts;
    };

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mMode = 1;
            *mLife = 9;

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
                mStage = GetMemContent(0x4d7c68, 0xfc);
                StageWarpsLoad(mStage, warps);
                SetFade(0.8f, 0.1f);
                Open();
                thPracParam = {};
                break;
            case 1:
                SetFade(0.8f, 0.1f);
                Close();

                // Fill Param
                thPracParam.mode = *mMode;
                thPracParam.warp = mWarp;
                thPracParam.life = *mLife;
                thPracParam.funds = *mFunds;
                thPracParam.difficulty = *mDifficulty;
                thPracParam.bulletMoney = *mBulletMoney;
                thPracParam.additional_cards = {};

                for (auto c : mAdditionalCards) {
                    if (c) thPracParam.additional_cards.push_back(c - 1);
                    else   break;
                }

                break;
            case 2:
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
                SetSizeRel(0.5f, 0.81f);
                SetPosRel(0.4f, 0.14f);
                SetItemWidthRel(-0.100f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(0.6f, 0.75f);
                SetPosRel(0.35f, 0.165f);
                SetItemWidthRel(-0.100f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(0.56f, 0.81f);
                SetPosRel(0.37f, 0.14f);
                SetItemWidthRel(-0.105f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }

            StageWarpsLoad(mStage, warps);
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(S(TH_MENU));
            ImGui::Separator();

            PracticeMenu();
        }

        const char* difficulties[8] = {
            "Very Easy",
            "Easy",
            "Normal",
            "Little Hard",
            "Hard",
            "Very Hard",
            "Lunatic",
            "Over Drive"
        };

        void PracticeMenu()
        {
            mMode();
            if (*mMode == 1) {
                StageWarpsRender(warps, mWarp, 0);

                mLife();
                mFunds();
                mBulletMoney();
                mDifficulty(difficulties[*mDifficulty]);

                ImGui::Separator();
                ImGui::TextUnformatted(S(TH185_ADDITIONAL_CARDS));
                switch (Gui::LocaleGet()) {
                case Gui::LOCALE_ZH_CN:
                    return Gui::MultiComboSelect(mAdditionalCards, card_names_cn, card_name_count, S(TH18_CARD_FORMAT));
                case Gui::LOCALE_JA_JP:
                    return Gui::MultiComboSelect(mAdditionalCards, card_names_jp, card_name_count, S(TH18_CARD_FORMAT));
                case Gui::LOCALE_EN_US:
                    return Gui::MultiComboSelect(mAdditionalCards, card_names_en, card_name_count, S(TH18_CARD_FORMAT));
                }
            }
        }

        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        size_t mStage;
        Gui::GuiDrag<int32_t, ImGuiDataType_S32> mBulletMoney { TH185_BULLET_MONEY, 0, INT_MAX };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 9 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mFunds { TH18_FUNDS, 0, 999990, 1, 100000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mDifficulty { TH_DIFFICULTY, 0, 7 };
        std::vector<unsigned int> mWarp;
        std::vector<unsigned int> mAdditionalCards = { 0 };

        // TODO: Setup chapters
        int mChapterSetup[7][2] {
            { 2, 4 },
            { 3, 3 },
            { 3, 3 },
            { 3, 3 },
            { 4, 6 },
            { 4, 0 },
            { 5, 4 },
        };
    };

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            if (*(uint8_t*)0x4d54e1 == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x475306, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x475306 = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x475306, 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
        }
        void GameplaySet()
        {
        }

    public:
        THAdvOptWnd() noexcept
        {
            SetWndFlag(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
            SetFade(0.8f, 0.8f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();

            FpsInit();
            GameplayInit();
        }

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_ADV_OPT));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(S(TH_ADV_OPT));
            ImGui::Separator();
            ImGui::BeginChild("Adv. Options", ImVec2(0.0f, 0.0f));

            if (BeginOptGroup<TH_GAME_SPEED>()) {
                if (GameFPSOpt(mOptCtx, false))
                    FpsSet();
                EndOptGroup();
            }

            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;
    };
    bool UpdateAdvOptWindow()
    {
        static THAdvOptWnd* advOptWnd = nullptr;
        if (!advOptWnd)
            advOptWnd = new THAdvOptWnd();
        if (Gui::KeyboardInputUpdate(VK_F12) == 1) {
            if (advOptWnd->IsOpen())
                advOptWnd->Close();
            else
                advOptWnd->Open();
        }
        advOptWnd->Update();

        return advOptWnd->IsOpen();
    }

    PATCH_ST(th185_prac_disable_arrows, 0x46d39f, "\xe9\xcd\x00\x00\x00", 5);
    EHOOK_G1(th185_prac_leave, 0x46d481)
    {
        if (isItemEnabled) {
            pCtx->Eip = 0x46d9c0;
            return true;
        }
        th185_prac_leave::GetHook().Disable();
        THGuiPrac::singleton().State(2);
        th185_prac_disable_arrows.Disable();
        pCtx->Eip = 0x46d9c0;
        return true;
    }
    PATCH_ST(th185_unhardcode_bosses, 0x43d0f6, "\xeb", 1);

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th185_wave_fire, 0x43cf47)
    {
        if (!thPracParam.mode)
            return true;

        auto getWaveName = [](const char* subName) -> const char* {
            const char* retStr = subName;

            auto subList = GetMemContent(0x004d7af4, 0x4f34, 0x10c);
            uint8_t* sub = ThModern_ECLGetSub(subName, subList);

            struct th10_instr_t {
                uint32_t time;
                uint16_t id;
                uint16_t size;
                uint16_t param_mask;
                uint8_t rank_mask;
                uint8_t param_count;
                uint32_t zero;
            };

            uint8_t* p = sub + ((uint32_t*)sub)[1];
            th10_instr_t* instr = (th10_instr_t*)p;

            while (*(uint32_t*)p != 0x484C4345) {
                if (instr->id == 1006) {
                    retStr = (char*)(p + sizeof(th10_instr_t) + 4);
                    break;
                }
                p += instr->size;
                instr = (th10_instr_t*)p;
            }

            return retStr;
        };

        auto& wave_select = THWaveSelect::singleton();

        if (wave_select.IsClosed()) {
            auto*  weights = (uint32_t*)GetMemAddr(ENEMY_MANAGER, 0x324);
            auto** sub_names = (const char**)0x4B5D50;

            wave_select.waveOpts = {};
            wave_select.selectedWave = SIZE_MAX;

            for (size_t i = 0; i < 110; i++) {
                if (weights[i]) {
                    wave_select.waveOpts.push_back({ i, getWaveName(sub_names[i]) });
                }
            }

            if (wave_select.waveOpts.size() <= 1)
                return true;

            pCtx->Eax = ULONG_MAX; // Safe. DWORD == unsigned long
            pCtx->Eip = 0x43d57a;
            wave_select.th185_replaymanager_on_tick_cutoff.Enable();
            wave_select.Open();
        } else {
            wave_select.Close();
        }

        if (wave_forced) {
            wave_forced = false;
            pCtx->Eax = ULONG_MAX; // Safe. DWORD == unsigned long
            pCtx->Eip = 0x43d57a;
        }
        return true;
    }

    PATCH_DY(th185_unblock_all, 0x46d532, "\xeb", 1);
    EHOOK_DY(th185_gui_update, 0x4013dd)
    {
        GameGuiBegin(IMPL_WIN32_DX9);

        // Gui components update
        THOverlay::singleton().Update();
        THGuiPrac::singleton().Update();

        if (uintptr_t game_thread = GetMemContent(GAME_THREAD_PTR);
            game_thread && !(GetMemContent(game_thread + 0xb0) & 0x10))
            THWaveSelect::singleton().Update();

        GameGuiEnd(UpdateAdvOptWindow() || isItemEnabled || THGuiPrac::singleton().IsOpen());
        return true;
    }
    EHOOK_DY(th185_gui_render, 0x4014fa)
    {
        GameGuiRender(IMPL_WIN32_DX9);
        return true;
    }

    EHOOK_DY(th185_patch_main, 0x448fb2)
    {
        // 0x4d1024 = phase

        if (thPracParam.mode) {
            *(int32_t*)(0x4d1070) = thPracParam.bulletMoney;
            *(int32_t*)(0x4d1074) = thPracParam.bulletMoney;
            *(int32_t*)(0x4d106c) = thPracParam.funds;
            *(int32_t*)(0x4d10bc) = thPracParam.life;
            *(int32_t*)(0x4d1038) = thPracParam.difficulty;

            th185_unhardcode_bosses.Enable();

            // I want additional cards to always appear in the same wave slot
            // but ZUN never initializes this variable outside the ecl script
            // therefore, I have to set it myself.
            *(int32_t*)(0x4d1024) = 1;
            StageWarpsApply(warps, thPracParam.warp, ThModern_ECLGetSub, GetMemContent(0x004d7af4, 0x4f34, 0x10c), 0);
            for (auto& c : thPracParam.additional_cards)
                AddCard(c);

        } else {
            th185_unhardcode_bosses.Disable();
        }
        return true;
    }
    EHOOK_DY(th185_restart, 0x45f86d)
    {
        auto s1 = pCtx->Esp + 0xc;
        auto s2 = pCtx->Edi + 0x1e4;
        auto s3 = *(DWORD*)(pCtx->Edi + 0x1e8);

        asm_call<0x479220, Stdcall>(0x7, pCtx->Ecx);

        uint32_t* ret = asm_call<0x48AAA0, Thiscall, uint32_t*>(s2, s1, 125, pCtx->Ecx);

        asm_call<0x48a540, Stdcall>(*ret, 0x6);

        // Restart New 1
        asm_call<0x48a540, Stdcall>(s3, 0x1);

        // Set restart flag, same under replay save status
        asm_call<0x41C7e0, Thiscall>(pCtx->Esi, 0x5);

        // Switch menu state to close
        asm_call<0x45e050, Thiscall>(pCtx->Edi, 19);

        pCtx->Edx = *(DWORD*)0x4ce400;
        pCtx->Eip = 0x45f918;
        return true;
    }
    EHOOK_DY(th185_exit, 0x45f918)
    {
        if (Gui::KeyboardInputGetRaw('Q'))
            pCtx->Eip = 0x45f92e;

        return true;
    }
    EHOOK_DY(th185_change_gamemode, 0x45b77a)
    {
        auto& wave_select = THWaveSelect::singleton();
        wave_select.Close();
        wave_select.th185_replaymanager_on_tick_cutoff.Disable();
        return true;
    }
    EHOOK_DY(th185_force_wave, 0x43d156)
    {
        auto& wave_select = THWaveSelect::singleton();
        if (auto i = wave_select.selectedWave; i != -1 && thPracParam.mode) {
            auto& [id, name] = wave_select.waveOpts[i];
            pCtx->Esi = id;
            wave_forced = true;
        }
        return true;
    }
    EHOOK_DY(th185_prac_confirm, 0x46d523)
    {
        if (isItemEnabled) {
            pCtx->Eip = 0x46d9c0;
            return true;
        }
        auto& p = THGuiPrac::singleton();
        if (p.IsOpen()) {
            p.State(1);
            th185_prac_disable_arrows.Disable();
            th185_prac_leave::GetHook().Disable();
        } else {
            p.State(0);
            th185_prac_disable_arrows.Enable();
            th185_prac_leave::GetHook().Enable();
            pCtx->Eip = 0x46d9c0;
        }
        return true;
    }
    PATCH_DY(th185_disable_topmost, 0x4747ac, "\x00", 1);
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4d52c8, 0x571d50,
            Gui::INGAGME_INPUT_GEN2, 0x4ce400, 0x4ce3f8, 0,
            -2, *(float*)0x573dc0, 0.0f);

        // Gui components creation
        THOverlay::singleton();
        THGuiPrac::singleton();
        THWaveSelect::singleton();

        th185_prac_disable_arrows.Setup();
        th185_unhardcode_bosses.Setup();

        // Hooks
        THMainHook::singleton().EnableAllHooks();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th185_gui_init_1.Disable();
        s.th185_gui_init_2.Disable();
    }

    EHOOK_DY(th185_gui_init_1, 0x46ce39)
    {
        THGuiCreate();
        THInitHookDisable();
        return true;
    }
    EHOOK_DY(th185_gui_init_2, 0x476580)
    {
        THGuiCreate();
        THInitHookDisable();
        return true;
    }
    HOOKSET_ENDDEF()
}

void TH185Init()
{
    TH185::THInitHook::singleton().EnableAllHooks();
}

}
