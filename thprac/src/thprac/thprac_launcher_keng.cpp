#include "thprac_launcher_others.h"
#include "thprac_launcher_utils.h"
#include "thprac_launcher_games.h"
#include "thprac_launcher_main.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_games_def.h"
#include "thprac_main.h"
#include "thprac_gui_locale.h"
#include "thprac_utils.h"
#include "utils/utils.h"
#include <functional>
#include <string>
#include <vector>
#include <numbers>
#include <fstream>
#include <iostream>
#include <format>
#include <chrono>
#include <map>
#include "..\3rdParties\rapidcsv\rapidcsv.h"
#include "thprac_compatible.h"

namespace THPrac {
    #define DIFF_NAME_SIZE 256
    #define PLAY_NAME_SIZE 256
    #define KENG_NAME_SIZE 256
    #define PLAY_CMT_SIZE 2048
    #define KENG_DESC_SIZE 2048

    std::vector<std::pair<std::string, std::vector<std::string>>>* GetPreset(Gui::locale_t language)
    {
    static std::vector<std::pair<std::string, std::vector<std::string>>> Preset_CN {
    { "无",{} },
{ "红魔乡L(梦A,简化)", 
    { "一面", "二道中", "琪露诺一非", "雹符「冰雹暴风」", "琪露诺二非", "冻符「完美冻结」", "雪符「钻石风暴」",
        "三前半", "三道中非", "华符「卷柏9」", "美铃一非", "虹符「彩虹的风铃」", "美铃二非", "幻符「华想梦葛」", "美铃三非", "彩符「彩光乱舞」", "彩符「极彩台风」",
        "四前半", "魔法书", "小恶魔", "四后半", 
        "帕秋莉一非", "火符「火神之光 上级」", "帕秋莉二非", "土符「三石塔的震动」", "火&土符「环状熔岩带」", "金&水符「水银之毒」", "木&火符「森林大火」",
        "五前半", "五道中非", "奇术「幻惑误导」", "五后半", "咲夜一非", "幻幽「迷幻杰克」", "咲夜二非", "幻世「世界」", "咲夜三非", "女仆秘技「杀人玩偶」",
        "六前半", "六道中非", "奇术「永恒的温柔」", "蕾米一非", "神罚「幼小的恶魔领主」", "蕾米二非", "狱符「千根针的针山」", "蕾米三非","神术「吸血鬼幻想」", "蕾米四非","红符「绯红之主」", "「红色的幻想乡」"
    }
},
{ "红魔乡L(梦B,简化)", 
    { "一面", "二道中", "琪露诺一非", "雹符「冰雹暴风」", "琪露诺二非", "冻符「完美冻结」", "雪符「钻石风暴」",
        "三前半", "三道中非", "华符「卷柏9」", "美铃一非", "虹符「彩虹的风铃」", "美铃二非", "幻符「华想梦葛」", "美铃三非", "彩符「彩光乱舞」", "彩符「极彩台风」",
        "四前半", "魔法书", "小恶魔", "四后半", 
        "帕秋莉一非", "水符「湖葬」", "帕秋莉二非", "木符「翠绿风暴」", "水&木符「水之精灵」", "金&水符「水银之毒」", "土&金符「翡翠巨石」",
        "五前半", "五道中非", "奇术「幻惑误导」", "五后半", "咲夜一非", "幻幽「迷幻杰克」", "咲夜二非", "幻世「世界」", "咲夜三非", "女仆秘技「杀人玩偶」",
        "六前半", "六道中非", "奇术「永恒的温柔」", "蕾米一非", "神罚「幼小的恶魔领主」", "蕾米二非", "狱符「千根针的针山」", "蕾米三非","神术「吸血鬼幻想」", "蕾米四非","红符「绯红之主」", "「红色的幻想乡」"
    }
},
{ "红魔乡L(魔A,简化)", 
    { "一面", "二道中", "琪露诺一非", "雹符「冰雹暴风」", "琪露诺二非", "冻符「完美冻结」", "雪符「钻石风暴」",
        "三前半", "三道中非", "华符「卷柏9」", "美铃一非", "虹符「彩虹的风铃」", "美铃二非", "幻符「华想梦葛」", "美铃三非", "彩符「彩光乱舞」", "彩符「极彩台风」",
        "四前半", "魔法书", "小恶魔", "四后半", 
        "帕秋莉一非", "木符「风灵角笛 上级」", "帕秋莉二非", "火符「火神之光辉」", "木&火符「森林大火」", "土&金符「翡翠巨石」", "火&土符「环状熔岩带」",
        "五前半", "五道中非", "奇术「幻惑误导」", "五后半", "咲夜一非", "幻幽「迷幻杰克」", "咲夜二非", "幻世「世界」", "咲夜三非", "女仆秘技「杀人玩偶」",
        "六前半", "六道中非", "奇术「永恒的温柔」", "蕾米一非", "神罚「幼小的恶魔领主」", "蕾米二非", "狱符「千根针的针山」", "蕾米三非","神术「吸血鬼幻想」", "蕾米四非","红符「绯红之主」", "「红色的幻想乡」"
    }
},
{ "红魔乡L(魔B,简化)", 
    { "一面", "二道中", "琪露诺一非", "雹符「冰雹暴风」", "琪露诺二非", "冻符「完美冻结」", "雪符「钻石风暴」",
        "三前半", "三道中非", "华符「卷柏9」", "美铃一非", "虹符「彩虹的风铃」", "美铃二非", "幻符「华想梦葛」", "美铃三非", "彩符「彩光乱舞」", "彩符「极彩台风」",
        "四前半", "魔法书", "小恶魔", "四后半", 
        "帕秋莉一非", "土符「慵懒三石塔 上级」", "帕秋莉二非", "金符「银龙」", "土&金符「翡翠巨石」", "水&木符「水之精灵」", "金&水符「水银之毒」",
        "五前半", "五道中非", "奇术「幻惑误导」", "五后半", "咲夜一非", "幻幽「迷幻杰克」", "咲夜二非", "幻世「世界」", "咲夜三非", "女仆秘技「杀人玩偶」",
        "六前半", "六道中非", "奇术「永恒的温柔」", "蕾米一非", "神罚「幼小的恶魔领主」", "蕾米二非", "狱符「千根针的针山」", "蕾米三非","神术「吸血鬼幻想」", "蕾米四非","红符「绯红之主」", "「红色的幻想乡」"
    }
},
    
{ "妖妖梦L(简化)", 
    { "一面", "二前半", "橙道中非", "仙符「凤凰展翅」", "二后半", "橙一非","阴阳「晴明大纹」", "橙二非", "童符「护法天童乱舞」","方符「奇门遁甲」",
        "三前半1", "爱丽丝一道中非", "三前半2", "爱丽丝二道中非", "操符「少女文乐」", "三后半", "爱丽丝一非", "苍符「博爱的奥尔良人偶」", "爱丽丝二非", "白符「白垩的俄罗斯人偶」", "爱丽丝三非", "雅符「春之京都人偶」", "诅咒「上吊的蓬莱人偶」",
        "四前半", "莉莉白", "四增援", "四后半", "四终幕", "四面一非", "四面一符", "四面二非", "骚符「活着的骚灵」", "四面三非", "四面三符", "骚葬「幽冥河畔」", "大合葬「灵车大协奏曲怪」",
        "五前半", "五道中非", "饿王剑「饿鬼十王的报应」", "五后半", "妖梦一非", "狱神剑「业风神闪斩」", "妖梦二非", "修罗剑「现世妄执」","人神剑「俗谛常住」","天神剑「三魂七魄」",
        "六开幕死路一条", "六道剑「一念无量劫」", "幽幽子一非", "亡乡「亡我乡 -自尽-」", "幽幽子二非", "亡舞「生者必灭之理 -魔境-」", "幽幽子三非", "华灵「蝶妄想」","幽幽子四非","幽曲「埋骨于弘川 -神灵-」","樱符「完全墨染的樱花 -开花-」","「反魂蝶 -八分咲-」"
    }
},
{ "永夜抄L(4A6A,简化)", 
    { "一面", "二道中", "二道中非", "声符「木菟的咆哮」","小碎骨一非","猛毒「毒蛾的黑暗演舞」", "小碎骨二非", "鹰符「祸延疾冲」","夜盲「夜雀之歌」","夜雀「午夜中的合唱指挥」",
        "三前半", "三道中非","产灵「最初的金字塔」", "三后半", "慧音一非", "始符「短命的137」", "野符「GHQ的危机」", "慧音二非", "国体「三种神器　乡」", "虚史「幻想乡传说」", "未来「高天原」",
        "四前半", "灵梦一非", "梦境「二重大结界」", "灵梦二非", "散灵「梦想封印　寂」", "四后半", "灵梦三非", "神技「八方龙杀阵」", "灵梦四非", "回灵「梦想封印　侘」", "大结界「博丽弹幕结界」", "神灵「梦想封印　瞬」", 
        "五前半", "黑兔一非", "黑兔二非", "五后半", "铃仙一非", "幻波「赤眼催眠」", "铃仙二非", "狂视「狂视调律」", "铃仙三非", "懒惰「生神停止", "散符「真实之月」","月眼「月兔远隔催眠术」",
        "六前半", "六道中非", "天丸「壶中的天地」", "永琳一非","神符「天人的族谱」", "永琳二非", "苏生「Rising Game」", "永琳三非", "神脑「思兼的头脑」","永琳四非","天咒「阿波罗13」","秘术「天文密葬法」","禁药「蓬莱之药」"
    }
},
{ "永夜抄L(4A6B,简化)", 
    { "一面", "二道中", "二道中非", "声符「木菟的咆哮」","小碎骨一非","猛毒「毒蛾的黑暗演舞」", "小碎骨二非", "鹰符「祸延疾冲」","夜盲「夜雀之歌」","夜雀「午夜中的合唱指挥」",
        "三前半", "三道中非","产灵「最初的金字塔」", "三后半", "慧音一非", "始符「短命的137」", "野符「GHQ的危机」", "慧音二非", "国体「三种神器　乡」", "虚史「幻想乡传说」", "未来「高天原」",
        "四前半", "灵梦一非", "梦境「二重大结界」", "灵梦二非", "散灵「梦想封印　寂」", "四后半", "灵梦三非", "神技「八方龙杀阵」", "灵梦四非", "回灵「梦想封印　侘」", "大结界「博丽弹幕结界」", "神灵「梦想封印　瞬」", 
        "五前半", "黑兔一非", "黑兔二非", "五后半", "铃仙一非", "幻波「赤眼催眠」", "铃仙二非", "狂视「狂视调律」", "铃仙三非", "懒惰「生神停止", "散符「真实之月」","月眼「月兔远隔催眠术」",
        "六前半", "六道中非", "药符「壶中的大银河」", "辉夜一非","神宝「耀眼的龙玉」", "辉夜二非", "神宝「佛体的金刚石」", "辉夜三非", "神宝「火蜥蜴之盾」","辉夜四非","神宝「无限的生命之泉」","神宝「蓬莱的玉枝　-梦色之乡-」",
        "「永夜归返　-待宵-」","「永夜归返　-子时四刻-」","「永夜归返　-丑时四刻-」","「永夜归返　-寅时四刻-」","「永夜归返　-世间开明-」"
    }
},
{ "永夜抄L(4B6A,简化)", 
    { "一面", "二道中", "二道中非", "声符「木菟的咆哮」","小碎骨一非","猛毒「毒蛾的黑暗演舞」", "小碎骨二非", "鹰符「祸延疾冲」","夜盲「夜雀之歌」","夜雀「午夜中的合唱指挥」",
        "三前半", "三道中非","产灵「最初的金字塔」", "三后半", "慧音一非", "始符「短命的137」", "野符「GHQ的危机」", "慧音二非", "国体「三种神器　乡」", "虚史「幻想乡传说」", "未来「高天原」",
        "四前半", "魔理沙一非", "魔空「小行星带」", "魔理沙二非", "黑魔「黑洞边缘」", "四后半", "魔理沙三非", "恋风「星光台风」", "魔理沙四非", "恋心「二重火花」", "光击「射月」", "魔炮「超究极火花」", 
        "五前半", "黑兔一非", "黑兔二非", "五后半", "铃仙一非", "幻波「赤眼催眠」", "铃仙二非", "狂视「狂视调律」", "铃仙三非", "懒惰「生神停止", "散符「真实之月」","月眼「月兔远隔催眠术」",
        "六前半", "六道中非", "天丸「壶中的天地」", "永琳一非","神符「天人的族谱」", "永琳二非", "苏生「Rising Game」", "永琳三非", "神脑「思兼的头脑」","永琳四非","天咒「阿波罗13」","秘术「天文密葬法」","禁药「蓬莱之药」"
    }
},
{ "永夜抄L(4B6B,简化)", 
    { "一面", "二道中", "二道中非", "声符「木菟的咆哮」","小碎骨一非","猛毒「毒蛾的黑暗演舞」", "小碎骨二非", "鹰符「祸延疾冲」","夜盲「夜雀之歌」","夜雀「午夜中的合唱指挥」",
        "三前半", "三道中非","产灵「最初的金字塔」", "三后半", "慧音一非", "始符「短命的137」", "野符「GHQ的危机」", "慧音二非", "国体「三种神器　乡」", "虚史「幻想乡传说」", "未来「高天原」",
        "四前半", "魔理沙一非", "魔空「小行星带」", "魔理沙二非", "黑魔「黑洞边缘」", "四后半", "魔理沙三非", "恋风「星光台风」", "魔理沙四非", "恋心「二重火花」", "光击「射月」", "魔炮「超究极火花」", 
        "五前半", "黑兔一非", "黑兔二非", "五后半", "铃仙一非", "幻波「赤眼催眠」", "铃仙二非", "狂视「狂视调律」", "铃仙三非", "懒惰「生神停止", "散符「真实之月」","月眼「月兔远隔催眠术」",
        "六前半", "六道中非", "药符「壶中的大银河」", "辉夜一非","神宝「耀眼的龙玉」", "辉夜二非", "神宝「佛体的金刚石」", "辉夜三非", "神宝「火蜥蜴之盾」","辉夜四非","神宝「无限的生命之泉」","神宝「蓬莱的玉枝　-梦色之乡-」",
        "「永夜归返　-待宵-」","「永夜归返　-子时四刻-」","「永夜归返　-丑时四刻-」","「永夜归返　-寅时四刻-」","「永夜归返　-世间开明-」"
   }
},
{ "风神录L(简化)", 
    { "一面", "二前半", "转转道中非", "厄符「厄神大人的生理节律」", "二后半", "转转一非", "疵痕「损坏的护符」", "转转二非", "悲运「大钟婆之火」", "创符「流放人偶」",
        "三前半", "光学「水迷彩」", "三后半1","四吴克", "河童一非", "漂溺「粼粼水底之心伤」", "河童二非","水符「河童之幻想大瀑布」", "河童三非", "河童「回转顶板」",
        "四前半", "狗椛", "四增援", "四后半", "三吴克","黄豆阵", "文文一非", "岐符「猿田彦神之岔路」", "文文二非", "风神「二百十日」", "文文三非", "「无双风神」", "塞符「天上天下的照国」", 
        "五前半", "五道中非", "秘术「单脉相传之弹幕」", "五后半", "早苗一非", "奇迹「客星辉煌之夜」", "早苗二非", "开海「摩西之奇迹」", "早苗三非", "准备「召请建御名方神」", "大奇迹「八坂之神风」",
        "六前半1", "六前半吴克", "六后半", "蓝猫","神妈一非", "奇祭「目处梃子乱舞」", "神妈二非", "神谷「神灵之谷」", "神妈三非","神秘「大和环面」","神妈四非","天龙「雨之源泉」","「风神之神德」",
   }
},
{ "地灵殿L(梦A,简化)", 
    { "一面", "二前半", "帕露西道中非", "嫉妒「看不见的绿眼怪兽」", "二后半", "帕露西一非", "开花爷爷「小白的灰烬」", "帕露西二非", "剪舌麻雀「大葛笼与小葛笼」", "恨符「丑时参拜第七日」",
        "三前半", "勇仪道中一非", "勇仪道中二非","鬼符「怪力乱神」", "三后半", "勇仪一非", "枷符「罪人不释之枷」", "勇仪二非","力业「大江山颪」", "勇仪三非","四天王奥义「三步必杀」",
        "四道中猫一非", "四前半", "四道中猫二非", "四后半", "小五一非", "回忆「恐怖催眠术」", "小五二非", "回忆「二重黑死蝶」", "回忆「飞行虫之巢」", "回忆「波与粒的境界」", 
        "五前半", "五道中猫一非", "五道中猫二非", "猫符「怨灵猫乱步」","五增援", "五后半", "猫一非", "咒精「怨灵凭依妖精」", "猫二非", "尸灵「食人怨灵」", "猫三非", "赎罪「古时之针与痛楚的怨灵」", "「小恶灵复活」",
        "六前半", "妖怪「火焰的车轮」","阿空一非", "核热「核反应失控」", "阿空二非", "爆符「千兆耀斑」", "阿空三非","焰星「十凶星」","阿空四非","「地狱的托卡马克装置」","「地底太阳」",
   }
},
{ "地灵殿L(梦A,简化)", 
    { "一面", "二前半", "帕露西道中非", "嫉妒「看不见的绿眼怪兽」", "二后半", "帕露西一非", "开花爷爷「小白的灰烬」", "帕露西二非", "剪舌麻雀「大葛笼与小葛笼」", "恨符「丑时参拜第七日」",
        "三前半", "勇仪道中一非", "勇仪道中二非","鬼符「怪力乱神」", "三后半", "勇仪一非", "枷符「罪人不释之枷」", "勇仪二非","力业「大江山颪」", "勇仪三非","四天王奥义「三步必杀」",
        "四道中猫一非", "四前半", "四道中猫二非", "四后半", "小五一非", "回忆「恐怖催眠术」", "小五二非", "回忆「二重黑死蝶」", "回忆「飞行虫之巢」", "回忆「波与粒的境界」", 
        "五前半", "五道中猫一非", "五道中猫二非", "猫符「怨灵猫乱步」","五增援", "五后半", "猫一非", "咒精「怨灵凭依妖精」", "猫二非", "尸灵「食人怨灵」", "猫三非", "赎罪「古时之针与痛楚的怨灵」", "「小恶灵复活」",
        "六前半", "妖怪「火焰的车轮」","阿空一非", "核热「核反应失控」", "阿空二非", "爆符「千兆耀斑」", "阿空三非","焰星「十凶星」","阿空四非","「地狱的托卡马克装置」","「地底太阳」",
   }
},
{ "地灵殿L(梦A,简化)", 
    { "一面", "二前半", "帕露西道中非", "嫉妒「看不见的绿眼怪兽」", "二后半", "帕露西一非", "开花爷爷「小白的灰烬」", "帕露西二非", "剪舌麻雀「大葛笼与小葛笼」", "恨符「丑时参拜第七日」",
        "三前半", "勇仪道中一非", "勇仪道中二非","鬼符「怪力乱神」", "三后半", "勇仪一非", "枷符「罪人不释之枷」", "勇仪二非","力业「大江山颪」", "勇仪三非","四天王奥义「三步必杀」",
        "四道中猫一非", "四前半", "四道中猫二非", "四后半", "小五一非", "回忆「恐怖催眠术」", "小五二非", "回忆「二重黑死蝶」", "回忆「飞行虫之巢」", "回忆「波与粒的境界」", 
        "五前半", "五道中猫一非", "五道中猫二非", "猫符「怨灵猫乱步」","五增援", "五后半", "猫一非", "咒精「怨灵凭依妖精」", "猫二非", "尸灵「食人怨灵」", "猫三非", "赎罪「古时之针与痛楚的怨灵」", "「小恶灵复活」",
        "六前半", "妖怪「火焰的车轮」","阿空一非", "核热「核反应失控」", "阿空二非", "爆符「千兆耀斑」", "阿空三非","焰星「十凶星」","阿空四非","「地狱的托卡马克装置」","「地底太阳」",
   }
},
{ "地灵殿L(梦B,简化)", 
    { "一面", "二前半", "帕露西道中非", "嫉妒「看不见的绿眼怪兽」", "二后半", "帕露西一非", "开花爷爷「小白的灰烬」", "帕露西二非", "剪舌麻雀「大葛笼与小葛笼」", "恨符「丑时参拜第七日」",
        "三前半", "勇仪道中一非", "勇仪道中二非","鬼符「怪力乱神」", "三后半", "勇仪一非", "枷符「罪人不释之枷」", "勇仪二非","力业「大江山颪」", "勇仪三非","四天王奥义「三步必杀」",
        "四道中猫一非", "四前半", "四道中猫二非", "四后半", "小五一非", "回忆「恐怖催眠术」", "小五二非", "回忆「户隐山之投」", "回忆「百万鬼夜行」", "回忆「蒙蒙迷雾」", 
        "五前半", "五道中猫一非", "五道中猫二非", "猫符「怨灵猫乱步」","五增援", "五后半", "猫一非", "咒精「怨灵凭依妖精」", "猫二非", "尸灵「食人怨灵」", "猫三非", "赎罪「古时之针与痛楚的怨灵」", "「小恶灵复活」",
        "六前半", "妖怪「火焰的车轮」","阿空一非", "核热「核反应失控」", "阿空二非", "爆符「千兆耀斑」", "阿空三非","焰星「十凶星」","阿空四非","「地狱的托卡马克装置」","「地底太阳」",
   }
},
{ "地灵殿L(梦C,简化)", 
    { "一面", "二前半", "帕露西道中非", "嫉妒「看不见的绿眼怪兽」", "二后半", "帕露西一非", "开花爷爷「小白的灰烬」", "帕露西二非", "剪舌麻雀「大葛笼与小葛笼」", "恨符「丑时参拜第七日」",
        "三前半", "勇仪道中一非", "勇仪道中二非","鬼符「怪力乱神」", "三后半", "勇仪一非", "枷符「罪人不释之枷」", "勇仪二非","力业「大江山颪」", "勇仪三非","四天王奥义「三步必杀」",
        "四道中猫一非", "四前半", "四道中猫二非", "四后半", "小五一非", "回忆「恐怖催眠术」", "小五二非", "回忆「风神木叶隐身术」", "回忆「天狗巨暴流」", "回忆「鸟居旋风」", 
        "五前半", "五道中猫一非", "五道中猫二非", "猫符「怨灵猫乱步」","五增援", "五后半", "猫一非", "咒精「怨灵凭依妖精」", "猫二非", "尸灵「食人怨灵」", "猫三非", "赎罪「古时之针与痛楚的怨灵」", "「小恶灵复活」",
        "六前半", "妖怪「火焰的车轮」","阿空一非", "核热「核反应失控」", "阿空二非", "爆符「千兆耀斑」", "阿空三非","焰星「十凶星」","阿空四非","「地狱的托卡马克装置」","「地底太阳」",
   }
},
{ "地灵殿L(魔A,简化)", 
    { "一面", "二前半", "帕露西道中非", "嫉妒「看不见的绿眼怪兽」", "二后半", "帕露西一非", "开花爷爷「小白的灰烬」", "帕露西二非", "剪舌麻雀「大葛笼与小葛笼」", "恨符「丑时参拜第七日」",
        "三前半", "勇仪道中一非", "勇仪道中二非","鬼符「怪力乱神」", "三后半", "勇仪一非", "枷符「罪人不释之枷」", "勇仪二非","力业「大江山颪」", "勇仪三非","四天王奥义「三步必杀」",
        "四道中猫一非", "四前半", "四道中猫二非", "四后半", "小五一非", "回忆「恐怖催眠术」", "小五二非", "回忆「春之京都人偶」", "回忆「稻草人偶敢死队」", "回忆「回归虚无」", 
        "五前半", "五道中猫一非", "五道中猫二非", "猫符「怨灵猫乱步」","五增援", "五后半", "猫一非", "咒精「怨灵凭依妖精」", "猫二非", "尸灵「食人怨灵」", "猫三非", "赎罪「古时之针与痛楚的怨灵」", "「小恶灵复活」",
        "六前半", "妖怪「火焰的车轮」","阿空一非", "核热「核反应失控」", "阿空二非", "爆符「千兆耀斑」", "阿空三非","焰星「十凶星」","阿空四非","「地狱的托卡马克装置」","「地底太阳」",
   }
},
{ "地灵殿L(魔B,简化)", 
    { "一面", "二前半", "帕露西道中非", "嫉妒「看不见的绿眼怪兽」", "二后半", "帕露西一非", "开花爷爷「小白的灰烬」", "帕露西二非", "剪舌麻雀「大葛笼与小葛笼」", "恨符「丑时参拜第七日」",
        "三前半", "勇仪道中一非", "勇仪道中二非","鬼符「怪力乱神」", "三后半", "勇仪一非", "枷符「罪人不释之枷」", "勇仪二非","力业「大江山颪」", "勇仪三非","四天王奥义「三步必杀」",
        "四道中猫一非", "四前半", "四道中猫二非", "四后半", "小五一非", "回忆「恐怖催眠术」", "小五二非", "回忆「水银之毒」", "回忆「水精公主」", "回忆「贤者之石」", 
        "五前半", "五道中猫一非", "五道中猫二非", "猫符「怨灵猫乱步」","五增援", "五后半", "猫一非", "咒精「怨灵凭依妖精」", "猫二非", "尸灵「食人怨灵」", "猫三非", "赎罪「古时之针与痛楚的怨灵」", "「小恶灵复活」",
        "六前半", "妖怪「火焰的车轮」","阿空一非", "核热「核反应失控」", "阿空二非", "爆符「千兆耀斑」", "阿空三非","焰星「十凶星」","阿空四非","「地狱的托卡马克装置」","「地底太阳」",
   }
},
{ "地灵殿L(魔C,简化)", 
    { "一面", "二前半", "帕露西道中非", "嫉妒「看不见的绿眼怪兽」", "二后半", "帕露西一非", "开花爷爷「小白的灰烬」", "帕露西二非", "剪舌麻雀「大葛笼与小葛笼」", "恨符「丑时参拜第七日」",
        "三前半", "勇仪道中一非", "勇仪道中二非","鬼符「怪力乱神」", "三后半", "勇仪一非", "枷符「罪人不释之枷」", "勇仪二非","力业「大江山颪」", "勇仪三非","四天王奥义「三步必杀」",
        "四道中猫一非", "四前半", "四道中猫二非", "四后半", "小五一非", "回忆「恐怖催眠术」", "小五二非", "回忆「延展手臂」", "回忆「河童之河口浪潮」", "回忆「粼粼水底之心伤」", 
        "五前半", "五道中猫一非", "五道中猫二非", "猫符「怨灵猫乱步」","五增援", "五后半", "猫一非", "咒精「怨灵凭依妖精」", "猫二非", "尸灵「食人怨灵」", "猫三非", "赎罪「古时之针与痛楚的怨灵」", "「小恶灵复活」",
        "六前半", "妖怪「火焰的车轮」","阿空一非", "核热「核反应失控」", "阿空二非", "爆符「千兆耀斑」", "阿空三非","焰星「十凶星」","阿空四非","「地狱的托卡马克装置」","「地底太阳」",
   }
},
{ "星莲船L(简化)", 
    { "一面", "二前半", "二道中非", "大轮「Hello,Forgotten World」", "二后半", "小伞一非", "伞符「雨伞的星之追忆」", "小伞二非", "雨伞「超防水干爽伞妖」", "化铁「备用伞特急夜晚狂欢号」",
        "三前半","三前半铁球", "三道中非", "神拳「天海地狱冲」","三后半蝴蝶","三后半铁球", "三后半绕圈", "一轮一非", "连打「帝王海妖来袭」", "一轮二非","溃灭「天上天下连续勾拳」", "一轮三非","忿怒「空前绝后巨眼焚身」",
        "四前半", "四面恶俗骰子", "四后半", "四后半乱弹","四后半蝴蝶", "船长一非", "倾覆「击沉之锚」", "船长二非", "溺符「沉底漩涡」", "船长三非", "港符「幽灵船永久停泊」", "幽灵「悄然袭来的长勺」", 
        "五前半", "五前半铁球", "五道中一非", "宝塔「最优良的宝物」", "五增援","五后半", "五闭幕铁球", "虎一非", "宝塔「光辉宝枪」", "虎二非", "光符「正义之威光」", "虎三非", "法灯「无瑕佛法之独钴杵」", "「完全净化」",
        "六道中", "六面恶俗骰子","莲妈一非", "吉兆「极乐的紫色云路」", "莲妈二非", "魔法「魔法之蝶」", "莲妈三非","光魔「魔法银河系」","莲妈四非","大魔法「魔神复诵」","超人「圣白莲」","飞钵「传说的飞空圆盘」"
   }
},
{ "神灵庙L(简化)", 
    { "一面", "二前半", "二道中非", "响符「混乱的山谷回声」", "二后半", "响子一非", "响符「强力共振」", "响子二非", "山彦「扩大回声」", "大声「激动Yahoo」",
        "三前半", "三道中非", "虹符「雨伞风暴」","三后半", "芳香一非", "回复「借由欲望的恢复」", "芳香二非","毒爪「剧毒杀害」", "芳香三非","欲灵「贪分欲吞噬者」",
        "四前半", "四道中非","邪符「孤魂野鬼」", "四后半", "小风神", "娘娘一非", "入魔「走火入魔」", "娘娘二非", "通灵「通灵芳香」", "娘娘三非", "道符「道胎动」", 
        "五前半", "五道中非","雷矢「元兴寺的龙卷」", "五后半","七吴克", "布都一非", "天符「天之磐舟哟，向天飞升吧」", "布都二非", "投皿「物部氏的八十平瓮」", "布都三非","炎符「火烧樱井寺」", "圣童女「大物忌正餐」",
        "六道中", "神子一非", "名誉「十二阶之冠位」", "神子二非", "仙符「日出之处的天子」", "神子三非","召唤「豪族乱舞」","神子四非","秘宝「圣德太子的欧帕兹」","光符「救世之光」","神光「无忤为宗」","「新生的神灵」"
   }
},    
{ "辉针城L(A,简化)", 
    { "一面", "二道中", "赤蛮奇一非", "首符「辘轳首飞来」", "赤蛮奇二非", "飞头「第九个头」", "飞头「杜拉罕之夜」",
        "三前半", "三道中非", "牙符「月下的犬齿」","三后半", "影狼一非", "变身「星形齿」", "影狼二非","咆哮「満月的远吠」", "影狼三非","天狼「高速猛扑」",
        "四前半", "四道中一非","四道中二非", "四后半", "弁弁一非", "平曲「祗园精舍的钟声」", "弁弁二非", "怨灵「平家的大怨灵」", "弁弁三非", "乐符「双重乐谱」", 
        "五前半", "五道中非","欺符「逆针击」", "五后半", "正邪一非", "逆符「镜中的邪恶」", "正邪二非", "逆符「天下翻覆」", "正邪三非","逆弓「天壤梦弓的诏敕」", "逆转「变革空勇士」",
        "六道中", "挽歌一非", "小弹「小人的荆棘路」", "挽歌二非", "小槌「变得更大吧」", "挽歌三非","妖剑「辉针剑」","挽歌四非","小槌「你给我变大吧」","「一寸之壁」","「七个一寸法师」"
   }
},    
{ "辉针城L(B,简化)", 
    { "一面", "二道中", "赤蛮奇一非", "首符「辘轳首飞来」", "赤蛮奇二非", "飞头「第九个头」", "飞头「杜拉罕之夜」",
        "三前半", "三道中非", "牙符「月下的犬齿」","三后半", "影狼一非", "变身「星形齿」", "影狼二非","咆哮「満月的远吠」", "影狼三非","天狼「高速猛扑」",
        "四前半", "四道中一非","四道中二非", "四后半", "八桥一非", "琴符「诸行无常的琴声」", "八桥二非", "响符「回音之庭」", "八桥三非", "筝曲「下克上安魂曲」", 
        "五前半", "五道中非","欺符「逆针击」", "五后半", "正邪一非", "逆符「镜中的邪恶」", "正邪二非", "逆符「天下翻覆」", "正邪三非","逆弓「天壤梦弓的诏敕」", "逆转「变革空勇士」",
        "六道中", "挽歌一非", "小弹「小人的荆棘路」", "挽歌二非", "小槌「变得更大吧」", "挽歌三非","妖剑「辉针剑」","挽歌四非","小槌「你给我变大吧」","「一寸之壁」","「七个一寸法师」"
   }
},   
{ "绀珠传L(简化)", 
    { "一道中1", "一道中一非", "一道中2", "一道中二非", "凶弹「高速撞击」", "一道中3", "蓝兔一非","弹符「鹰已击中」", "蓝兔二非","铳符「月狂之枪」",
      "二前半", "二道中非","二后半","黄兔一非", "兔符「浆果浆果团子」", "黄兔二非", "兔符「团子影响力」","黄兔三非", "月见酒「月狂的九月」",
      "三前半", "三道中非", "梦符「绯红色的压迫噩梦」","三后半", "123一非", "梦符「愁永远之梦」", "123二非","梦符「刈安色的错综迷梦」", "123三非","梦符「梦我梦中」","月符「绀色的狂梦」",
      "四前半1","四前半激光阵","四前半阴阳玉阵", "四道中非", "四后半", "探女一非", "玉符「乌合的二重咒」", "探女二非", "玉符「秽身探知型水雷　改」", "探女三非", "玉符「众神的光辉弹冠」","「孤翼的白鹭」", 
      "五前半", "狙激光阵", "三吴克", "黄激光", "绿激光", "太田飞行阵", "黄绿蓝", "皮丝一非", "狱符「地狱之蚀」", "皮丝二非", "狱符「星与条纹」", "皮丝三非", "狱炎「擦弹的狱意」", "地狱「条纹状的深渊」","「阿波罗捏造说」",
      "六道中", "流星雨","纯狐一非", "「掌上的纯光」", "纯狐二非", "「杀意的百合」", "纯狐三非","「现代的神灵界」","纯狐四非","「战栗的寒冷之星」","「纯粹的狂气」","「地上秽的纯化」","纯符「纯粹的弹幕地狱」"
   }
},  
{ "天空璋L(简化)", 
    { "一面", "二道中","山姥一非", "雨符「被诅咒的暴雨」", "山姥二非", "刃符「山姥的鬼菜刀研磨」", "尽符「血腥的深山谋杀」",
      "三前半", "三道中莉莉白","三后半", "阿吽一非", "狗符「山狗的散步」", "阿吽二非","独乐「蜷缩死去」", "阿吽三非","狛符「单人式阿吽的呼吸」",
      "四前半", "四道中非", "四后半","四后半飞行阵", "成美一非", "魔符「即席菩提」", "成美二非", "魔符「作宠物的巨大弹生命体」", "成美三非", "地藏「业火救济」",
      "五前半", "五道中非", "五后半", "尬舞一非", "尬舞一符", "尬舞二非", "尬舞二符", "尬舞三非", "舞符「背后之祭」", "狂舞「狂乱天狗怖吓」",
      "六道中阴阳玉阵", "六道中蝴蝶","摩多罗一非", "后符「绝对秘神的后光」", "夏非", "里夏「异常酷暑之焦土」", "秋非","里秋「异常枯死之饿鬼」","冬非","里冬「异常降雪之雪人」","春非","里春「异常落花之魔术使」","终符"
   }
},  
{ "鬼形兽L(简化) ", 
    { "一面", "二前半", "润美道中非","二后半", "润美一非", "石符「沉重的石之婴儿」", "润美二非", "溺符「三途的沦溺」", "鬼符「饿鬼围城」",
      "三前半", "三道中非","三后半", "鸡一非", "水符「分水的顶级试炼」", "鸡二非","光符「瞭望的顶级试炼」", "鸡三非","鬼符「鬼渡的狱级试炼」",
      "四前半","鬼火阵", "四道中非", "四后半", "吉吊一非", "龟符「龟甲地狱」", "吉吊二非", "鬼符「搦手的鬼畜生」", "吉吊三非", "龙符「龙纹弹」",
      "五前半", "五道中非", "五后半", "磨弓一非", "埴轮「熟练弓兵埴轮」", "磨弓二非", "埴轮「熟练剑士埴轮」", "磨弓三非", "埴轮「熟练骑马兵埴轮」", "埴轮「不败的无尽兵团」",
      "六前半", "六道中非","六后半","袿姬一非", "方形「方形造物」", "袿姬二非", "圆形「圆形造物」", "袿姬三非","线形「线形造物」","袿姬四非","埴轮「偶像造物」","「鬼形造形术」","「几何造物」","「偶像恶魔」"
   }
},  
{ "虹龙洞L(简化) ", 
    { "一面", "二前半", "二道中非","二后半", "山城一非", "森符「真·木隐的技术」", "山城二非", "森符「真·最深的森域」", "叶技「绿色龙卷」",
      "三前半", "三道中非","三后半","绿豆阵", "山如一非", "山符「惊天的云间草」", "山如二非","山怪「妖魔喧嚣的薄雪草」", "山如三非","山花「杀戮的山之女王」",
      "四开","四前半","黄风神", "大阴阳玉", "蓝风神","四后半", "玉造一非", "玉符「阴阳神玉」", "玉造二非", "女王珠「彩虹门的另一侧」", "玉造三非", "「阴阳窒息」",
      "五前半", "五道中非","五增援", "五后半","五面飞行阵", "龙一非", "祸星「星火燎原乱舞」", "龙二非", "星风「虹彩陆离乱舞」", "龙三非", "光马「天马行空乱舞」", "虹光「光风霁月」",
      "六前半", "六道中非","六后半","千亦一非", "「向无主的贡品」", "千亦二非", "「弹幕收集狂的妄执」", "千亦三非","「弹幕自由市场」","千亦四非","「虹人环」","「无道的弹幕领土」","「弹幕的庇护所」"
   }
},  

    };

    static std::vector<std::pair<std::string, std::vector<std::string>>> Preset_JP = { { "None", {} } };
    static std::vector<std::pair<std::string, std::vector<std::string>>> Preset_EN = { { "None", {} } };
    switch (language)
    {
    case Gui::LOCALE_ZH_CN:
        return &Preset_CN;
    case Gui::LOCALE_JA_JP:
        return &Preset_JP;
    case Gui::LOCALE_EN_US:
        return &Preset_EN;
    }
    return &Preset_EN;
    }

    void KengSave();
    void GuiDateSelector(const char* id, std::chrono::year_month_day* time)
    {
        ImGui::PushID(id);
        
        int y = (int32_t)time->year();
        int m = (unsigned)time->month();
        int d = (unsigned)time->day();
        
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputInt("##Y", &y);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(180.0f);
        ImGui::InputInt("##M", &m);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(180.0f);
        ImGui::InputInt("##D", &d);
        
        std::chrono::year_month_day time_new = std::chrono::year_month_day(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
        if (time_new.ok())
            *time = time_new;

        ImGui::PopID();
    }

    typedef int DiffIndex;

    struct KengDifficulty {
        DiffIndex id;
        char name[DIFF_NAME_SIZE];

        KengDifficulty(int iid = 0, const char* nname = "")
            : id(iid)
        {
            strcpy_s(name, nname);
        }
        KengDifficulty(std::istream& is, [[maybe_unused]] int ver)
        {
            is.read((char*)&id, sizeof(id));
            is.read((char*)name, sizeof(name));
        }
        std::ostream& Write(std::ostream& os)
        {
            os.write((char*)&id, sizeof(id));
            os.write((char*)name, sizeof(name));
            return os;
        }
    };

    
    void static GuiSwapDiff(int idx_u, int idx_d, std::vector<KengDifficulty>& diffs, std::vector<char>& selects)
    {
        if (idx_u != -1 && idx_u >= 1) {
            std::swap(selects[idx_u], selects[idx_u - 1]);
            std::swap(diffs[idx_u], diffs[idx_u - 1]);
        }
        if (idx_d != -1 && std::ssize(diffs) >= 2 && idx_d <= std::ssize(diffs) - 2) {
            std::swap(selects[idx_d], selects[idx_d + 1]);
            std::swap(diffs[idx_d], diffs[idx_d + 1]);
        }
    }

    bool static GuiInsertDiff(int& idx_add, std::vector<KengDifficulty>& diffs, std::vector<char>& selects,int& id_tot)
    {
        static bool isopen = false;
        bool focus = (isopen == false);
        bool is_changed = false;
        if (idx_add != -1) {
            isopen = true;
            ImGui::OpenPopup(S(THPRAC_KENG_ADD_DIFF_POPUP));
        }
        if (GuiModal(S(THPRAC_KENG_ADD_DIFF_POPUP), { LauncherWndGetSize().x * 0.5f, LauncherWndGetSize().y * 0.3f }, &isopen)) {
            ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
            {
                static char diff_name[DIFF_NAME_SIZE] = { 0 };
                ImGui::Columns(2, 0, false);
                ImGui::SetColumnWidth(0, 300.0f);
                ImGui::Text(S(THPRAC_KENG_DIFF_NAME));
                ImGui::NextColumn();
                if(focus)
                    ImGui::SetKeyboardFocusHere();
                ImGui::InputText("##diff name", diff_name, sizeof(diff_name), ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Columns(1);
                ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                auto retnValue = GuiCornerButton(S(THPRAC_APPLY), S(THPRAC_CANCEL), ImVec2(1.0f, 0.0f), true);
                if (retnValue == 1 || ImGui::IsKeyDown(0xD)) {//enter
                    std::string name_input = std::string(diff_name);
                    bool find=false;
                    for (int i = 0; i < std::ssize(diffs); i++)
                        if (!strcmp(diffs[i].name,diff_name)){
                            find = true;
                            break;
                        }
                    if (!find)
                    {
                        if (diffs.size() == 0) {
                            diffs.insert(diffs.begin() + idx_add, KengDifficulty { id_tot, diff_name });
                            id_tot++;
                            selects.insert(selects.begin() + idx_add, true);
                        } else {
                            diffs.insert(diffs.begin() + idx_add + 1, KengDifficulty { id_tot, diff_name });
                            id_tot++;
                            selects.insert(selects.begin() + idx_add + 1, true);
                        }
                        is_changed = true;
                    }
                    idx_add = -1;
                    KengSave();
                    isopen = false;
                } else if (retnValue == 2) {
                    idx_add = -1;
                    isopen = false;
                }
                if (!isopen)
                {
                    memset(diff_name, 0, sizeof(diff_name));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::PopTextWrapPos();
                ImGui::EndPopup();
            }
        }
        return is_changed;
    }

    class SingleGamePlay
    {
        char mPlayName[PLAY_NAME_SIZE];
        char mPlayComment[PLAY_CMT_SIZE];
        std::chrono::year_month_day mTimeCreate;
        std::vector<DiffIndex> mDiffsDied;
    public:
        SingleGamePlay(const char* name, const char* cmt, std::vector<DiffIndex> diffs, std::chrono::year_month_day timeCreate):
            mDiffsDied(diffs)
            , mTimeCreate(timeCreate)
        {
            strcpy_s(mPlayName, name);
            strcpy_s(mPlayComment, cmt);
        }
        SingleGamePlay(std::istream& is, int ver)
        {
            switch (ver)
            {
            case 1:
            default:
            {
                int nsz;
                is.read((char*)mPlayName, sizeof(mPlayName));
                is.read((char*)mPlayComment, sizeof(mPlayComment));
        
                int y, m, d;
                is.read((char*)&y, sizeof(y));
                is.read((char*)&m, sizeof(m));
                is.read((char*)&d, sizeof(d));
                mTimeCreate = std::chrono::year_month_day(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
        
                is.read((char*)&nsz, sizeof(nsz));
                mDiffsDied.resize(nsz, 0);
                for (int i = 0; i < nsz; i++) {
                    is.read((char*)&(mDiffsDied[i]), sizeof(mDiffsDied[i]));
                }
            }
            }
        }

        char* GetTimeDesc(){
            static char chs[256] = { 0 };
            sprintf_s(chs, S(THPRAC_OTHER_TODAY), static_cast<int>(mTimeCreate.year()), static_cast<unsigned int>(mTimeCreate.month()), static_cast<unsigned int>(mTimeCreate.day()));
            return chs;
        }

        
        const char* GetDescription_Line()
        {
            static char mCmtLine[64];
            memcpy_s(mCmtLine, sizeof(mCmtLine), mPlayComment, sizeof(mCmtLine));
            bool add_dots = false;
            for (int i = 0; i < 25; i++) {
                if (mCmtLine[i] == '\r' || mCmtLine[i] == '\n') {
                    mCmtLine[i] = '.';
                    mCmtLine[i + 1] = '.';
                    mCmtLine[i + 2] = '.';
                    mCmtLine[i + 3] = '\0';
                    add_dots = true;
                    break;
                } else if (mCmtLine[i] == '\0') {
                    add_dots = true;
                    break;
                }
            }
            if (!add_dots) {
                mCmtLine[24] = '.';
                mCmtLine[25] = '.';
                mCmtLine[26] = '.';
                mCmtLine[27] = '\0';
            }
            return mCmtLine;
        }

        void DrawPlay(bool* is_del, bool* is_open, std::vector<KengDifficulty>& diffs, int& diffs_id_tot, std::function<void(int)> remove_diff)
        {
            // keng draw
            ImGui::Columns(3, 0, false);
            ImGui::SetColumnWidth(0, LauncherWndGetSize().x * 0.05f);
            ImGui::SetColumnWidth(1, 300.0f);

            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_KENG_PLAY_NAME));
            ImGui::NextColumn();
            ImGui::InputText("##playname", mPlayName,sizeof(mPlayName));
            ImGui::NextColumn();

            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_KENG_PLAY_DATE));
            ImGui::NextColumn();
            GuiDateSelector("##date", &mTimeCreate);
            ImGui::NextColumn();

            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_KENG_PLAY_CMT));
            ImGui::NextColumn();
            ImGui::InputTextMultiline("##playcmt", mPlayComment,sizeof(mPlayComment));
            ImGui::Columns(1);

            ImGui::NewLine();
            ImGui::Separator();

            // diffs draw
            static std::vector<char> diffs_select;
            diffs_select.resize(diffs.size(),false);
            for (auto& i : diffs_select)
                i = false;
            for (auto& id : mDiffsDied){
                for (int j = 0; j < std::ssize(diffs); j++){
                    if (diffs[j].id == id){
                        diffs_select[j] = true;
                        break;
                    }
                }
            }
            
            ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
            static int idx_add = -1;
            static int idx_changename = -1;
            int idx_u = -1, idx_d = -1, idx_rem = -1;
            bool is_changed = false;
            if (ImGui::BeginTable("##diffTable", 2, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable, ImVec2(LauncherWndGetSize().x * 0.9f, 0.0f)))
            {
                ImGui::TableSetupColumn(S(THPRAC_KENG_DIFF_TABLE_CK), ImGuiTableColumnFlags_WidthStretch, ImGui::GetTextLineHeight() * 10);
                ImGui::TableSetupColumn(S(THPRAC_KENG_DIFF_TABLE_NAME), ImGuiTableColumnFlags_WidthStretch, LauncherWndGetSize().x * 0.9f - ImGui::GetTextLineHeight() * 10);
                ImGui::TableHeadersRow();
                if (diffs.size() == 0) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button(S(THPRAC_KENG_DIFF_TABLE_ADD)))
                        idx_add = 0;
                }
                for (int idx = 0; idx < std::ssize(diffs); idx++) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button(std::format("{}##diff_add_{}", S(THPRAC_KENG_DIFF_TABLE_ADD), idx).c_str()))
                        idx_add = idx;
                    ImGui::SameLine();
                    if (ImGui::Button(std::format("{}##diff_rem_{}", S(THPRAC_KENG_DIFF_TABLE_REM), idx).c_str()))
                        idx_rem = idx;
                    ImGui::SameLine();
                    // u
                    if (ImGui::Button(std::format("{}##diff_u_{}", S(THPRAC_KENG_DIFF_TABLE_U), idx).c_str()))
                        idx_u = idx;
                    ImGui::SameLine();
                    // d
                    if (ImGui::Button(std::format("{}##diff_d_{}", S(THPRAC_KENG_DIFF_TABLE_D), idx).c_str()))
                        idx_d = idx;
                    ImGui::SameLine();
                    is_changed |= ImGui::Checkbox(std::format("##diff{}", idx).c_str(), (bool*)(&diffs_select[idx]));
                    ImGui::TableNextColumn();
                    if(ImGui::Selectable(std::format("{}##sel{}", diffs[idx].name, idx).c_str()))
                        idx_changename=idx;
                }
                ImGui::EndTable();
            }else{
                idx_add = -1;
            }
            {
                static bool isopen_changename = false;
                static char diffname[DIFF_NAME_SIZE];
                bool focus = (isopen_changename == false);
                if (idx_changename != -1) {
                    if (!isopen_changename) {
                        strcpy_s(diffname, diffs[idx_changename].name);
                    }
                    isopen_changename = true;
                    ImGui::OpenPopup(std::format("{}##diffname", S(THPRAC_KENG_DIFF_NAME)).c_str());
                }
                if (GuiModal(std::format("{}##diffname", S(THPRAC_KENG_DIFF_NAME)).c_str(), { LauncherWndGetSize().x * 0.5f, LauncherWndGetSize().y * 0.3f }, &isopen_changename)) {
                    ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                    {
                        ImGui::Columns(2, 0, false);
                        ImGui::SetColumnWidth(0, 300.0f);
                        ImGui::Text(S(THPRAC_KENG_DIFF_NAME));
                        ImGui::NextColumn();
                        if (focus)
                            ImGui::SetKeyboardFocusHere();
                        ImGui::InputText("##diffnameip", diffname, sizeof(diffname), ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsNoBlank);

                        ImGui::Columns(1);
                        ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                        if (ImGui::Button(S(THPRAC_APPLY)) || ImGui::IsKeyDown(0xD)) { // enter
                            bool find = false;
                            for (int i = 0; i < std::ssize(diffs); i++)
                                if (!strcmp(diffs[i].name, diffname)) {
                                    find = true;
                                    break;
                                }
                            if (!find) {
                                strcpy_s(diffs[idx_changename].name, diffname);
                            }
                            KengSave();
                            isopen_changename = false;
                        }
                        if (!isopen_changename) {
                            ImGui::CloseCurrentPopup();
                            idx_changename = -1;
                        }
                        ImGui::PopTextWrapPos();
                        ImGui::EndPopup();
                    }
                }
                if (!isopen_changename)
                    idx_changename = -1;
            }
            GuiSwapDiff(idx_u, idx_d, diffs, diffs_select);
            is_changed |= GuiInsertDiff(idx_add, diffs, diffs_select, diffs_id_tot);
            if (idx_rem != -1){
                remove_diff(idx_rem);
            }
            if (is_changed){
                std::vector<int> ids;
                for (int i = 0; i < std::ssize(diffs); i++) {
                    if (diffs_select[i])
                        ids.push_back(diffs[i].id);
                }
                this->mDiffsDied = ids;
            }

            ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
            if (ImGui::Button(S(THPRAC_KENG_RETURN))) {
                *is_open = false;
            }
            ImGui::SameLine();
            if (ImGui::Button(S(THPRAC_KENG_DEL))) {
                ImGui::OpenPopup(S(THPRAC_KENG_DEL_POPUP));
            }
            if (GuiModal(S(THPRAC_KENG_DEL_POPUP), { LauncherWndGetSize().x * 0.3f, LauncherWndGetSize().y * 0.3f })) {
                ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                {
                    ImGui::Text(S(THPRAC_KENG_DEL_YES_OR_NO));
                    ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                    auto retnValue = GuiCornerButton(S(THPRAC_APPLY), S(THPRAC_CANCEL), ImVec2(1.0f, 0.0f), true);
                    if (retnValue == 1) {
                        *is_del = true;
                        *is_open = false;
                        ImGui::CloseCurrentPopup();
                    } else if (retnValue == 2) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::PopTextWrapPos();
                    ImGui::EndPopup();
                }
            }
        }
        std::ostream& Write(std::ostream& os)
        {
            os.write((char*)mPlayName, sizeof(mPlayName));
            os.write((char*)mPlayComment, sizeof(mPlayComment));

            int y = (int32_t)mTimeCreate.year();
            int m = (unsigned)mTimeCreate.month();
            int d = (unsigned)mTimeCreate.day();
            os.write((char*)&y, sizeof(y));
            os.write((char*)&m, sizeof(m));
            os.write((char*)&d, sizeof(d));

            int nsz = mDiffsDied.size();
            os.write((char*)&nsz, sizeof(nsz));
            for (auto& id : mDiffsDied)
                os.write((char*)&id, sizeof(id));
            return os;
        }
        friend class Keng;
    };

    struct DiffsDetailedTableItem {
        int index;
        char name[DIFF_NAME_SIZE];
        int count;
        double pass_rate;
        static const ImGuiTableSortSpecs* s_current_sort_specs;

#ifndef IMGUI_CDECL
#ifdef _MSC_VER
#define IMGUI_CDECL __cdecl
#else
#define IMGUI_CDECL
#endif
#endif
        static int IMGUI_CDECL CompareWithSortSpecs(const void* lhs, const void* rhs)
        {
            const DiffsDetailedTableItem* a = (const DiffsDetailedTableItem*)lhs;
            const DiffsDetailedTableItem* b = (const DiffsDetailedTableItem*)rhs;
            for (int n = 0; n < s_current_sort_specs->SpecsCount; n++) {
                // Here we identify columns using the ColumnUserID value that we ourselves passed to TableSetupColumn()
                // We could also choose to identify columns based on their index (sort_spec->ColumnIndex), which is simpler!
                const ImGuiTableColumnSortSpecs* sort_spec = &s_current_sort_specs->Specs[n];
                double delta = 0;
                switch (sort_spec->ColumnUserID) {
                default:
                case 0:
                    delta = (a->index - b->index);
                    break;
                case 1:
                    delta = (strcmp(a->name, b->name));
                    break;
                case 2:
                    delta = (a->count - b->count);
                    break;
                case 3:
                    delta = (a->pass_rate - b->pass_rate);
                    break;
                }
                if (delta > 0)
                    return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
                if (delta < 0)
                    return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
            }
            return (a->index - b->index);
        };
    };
    const ImGuiTableSortSpecs* DiffsDetailedTableItem::s_current_sort_specs = NULL;

    class Keng
    {
        char mKengName[KENG_NAME_SIZE];
        char mKengDescription[KENG_DESC_SIZE];
        std::chrono::year_month_day mTimeCreate;

        std::vector<KengDifficulty> mKengDifficulties;
        int mDifficulties_idtot=0;
        std::vector<SingleGamePlay> mPlays;
    public:
        char* GetTimeDesc()
        {
            static char chs[256] = { 0 };
            sprintf_s(chs, S(THPRAC_OTHER_TODAY), static_cast<int>(mTimeCreate.year()), static_cast<unsigned int>(mTimeCreate.month()), static_cast<unsigned int>(mTimeCreate.day()));
            return chs;
        }

        void GuiGetDetails()
        {
            static std::map<int, int> diffs_die_count;
            static std::map<int, double> probs_pass_map;
            static std::vector<double> probs_pass_vec;
            static std::vector<double> pass_rate;
            static bool isopen = false;
            static bool use_EM = false;
            static int n_plays = 0;
            static std::chrono::year_month_day after_this=std::chrono::year_month_day(std::chrono::year(1900),std::chrono::month(1),std::chrono::day(1));

            static ImVector<DiffsDetailedTableItem> table_itmes;
            static bool calculated = false;

            if (ImGui::Button(S(THPRAC_KENG_DETAILS))) {
                calculated = false;
                isopen = true;

                if (mPlays.size()==0)
                    after_this = std::chrono::year_month_day(std::chrono::year(1900), std::chrono::month(1), std::chrono::day(1));
                else 
                    after_this = mPlays[0].mTimeCreate;
                for (auto& play : mPlays) {
                    if (play.mTimeCreate < after_this)
                        after_this = play.mTimeCreate;
                }
                ImGui::OpenPopup(S(THPRAC_KENG_DETAILS_POPUP));
            }
            if (!isopen)
                return;
            if (!calculated) {
                calculated = true;
                diffs_die_count = {};
                probs_pass_vec = {};
                pass_rate = {};
                probs_pass_map = {};
                table_itmes.clear();
                n_plays = 0;
                for (auto& play : mPlays) {
                    if (play.mTimeCreate >= after_this) {
                        for (auto& diff : play.mDiffsDied) {
                            if (diffs_die_count.contains(diff))
                                diffs_die_count[diff]++;
                            else
                                diffs_die_count[diff] = 1;
                        }
                        n_plays++;
                    }
                }
                probs_pass_vec.resize(mKengDifficulties.size(), 0.0);
                for (int i = 0; i < std::ssize(mKengDifficulties); i++) {
                    if (diffs_die_count.contains(mKengDifficulties[i].id)) {
                        if (use_EM)
                            probs_pass_map[mKengDifficulties[i].id] = probs_pass_vec[i] = 1.0 - ((double)diffs_die_count[mKengDifficulties[i].id] + 1.0) / std::max(1.0, (double)n_plays + 2.0);
                        else
                            probs_pass_map[mKengDifficulties[i].id] = probs_pass_vec[i] = 1.0 - (double)diffs_die_count[mKengDifficulties[i].id] / std::max(1.0, (double)n_plays);
                    }else{
                        probs_pass_map[mKengDifficulties[i].id] = probs_pass_vec[i] = 1.0;
                    }
                }
                auto CalProbForN = [](std::vector<double>& probs_single) -> std::vector<double> {
                    // p[n][m]:  for first n singles, catchs m
                    // p[n][0]
                    // p[n][m] = p[n-1][m-1]*prob_single[n] + p[n-1][m]
                    // n: 1..N, m: 0..n
                    std::vector<std::vector<double>> p;
                    p.push_back({ 1.0 }); // init  p[0][n]
                    for (int i = 1; i <= std::ssize(probs_single); i++) {
                        p.push_back(std::vector<double>());
                        p[i].resize(static_cast<size_t>(i) + 1, -1); // init p[i][m]
                    }
                    for (int i = 1; i <= std::ssize(probs_single); i++) {
                        // cal p[n][0]
                        p[i][0] = 1.0;
                        for (int j = 1; j <= i; j++) {
                            p[i][0] *= (1 - probs_single[j - 1]);
                        }
                    }
                    p[0][0] = 1.0; // init p[0][0]
                    for (int i = 1; i <= std::ssize(probs_single); i++) {
                        for (int j = 1; j <= i; j++) {
                            p[i][j] = p[i - 1][j - 1] * probs_single[i - 1] + (j <= i - 1 ? p[i - 1][j] * (1 - probs_single[i - 1]) : 0.0);
                        }
                    }
                    return p[p.size() - 1];
                };
                auto probs = CalProbForN(probs_pass_vec);
                std::reverse(probs.begin(), probs.end());
                pass_rate.resize(probs.size());
                int miss = 0;
                double tot = 0.0;
                for (double u : probs) {
                    tot = tot + u;
                    pass_rate[miss] = tot;
                    miss++;
                }

                // fill the table
                int n = 1;
                for (auto& diff : mKengDifficulties) {
                    DiffsDetailedTableItem item;
                    item.index = n;
                    memcpy(item.name,diff.name,sizeof(diff.name));
                    item.count = diffs_die_count[diff.id];
                    item.pass_rate = probs_pass_map[diff.id];
                    table_itmes.push_back(item);
                    n++;
                }
            }
            
            if (GuiModal(S(THPRAC_KENG_DETAILS_POPUP), { LauncherWndGetSize().x * 0.95f, LauncherWndGetSize().y * 0.8f }, &isopen)) {
                ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.05f);
                if (ImGui::Button(S(THPRAC_KENG_RETURN)))
                    isopen = false;

                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.05f);
                ImGui::Text(S(THPRAC_KENG_DETAILS_AFTER));
                ImGui::SameLine();
                GuiDateSelector("##ymd_after", &after_this);
                ImGui::SameLine();
                ImGui::Text("(%d)", n_plays);
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.05f);
                ImGui::Checkbox(S(THPRAC_KENG_DETAILS_CAL_EM), &use_EM);
                ImGui::SameLine();
                GuiHelpMarker(S(THPRAC_KENG_DETAILS_CAL_EM_DESC));
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.05f);
                if (ImGui::Button(S(THPRAC_KENG_DETAILS_CAL))) {
                    calculated = false;
                }

                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.05f);

                if (ImGui::BeginTable(S(THPRAC_KENG_DETAILS_DIFF_TABLE), 4, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable | ImGuiTableFlags_::ImGuiTableFlags_Sortable | ImGuiTableFlags_::ImGuiTableFlags_SortMulti, ImVec2(ImGui::GetWindowWidth() * 0.9f, 0.0f))) {
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_DIFF_INDEX), ImGuiTableColumnFlags_WidthStretch, ImGui::GetWindowWidth() * 0.15f, 0);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_DIFF_NAME), ImGuiTableColumnFlags_WidthStretch, ImGui::GetWindowWidth() * 0.4f, 1);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_DIFF_CNT), ImGuiTableColumnFlags_WidthStretch, ImGui::GetWindowWidth() * 0.2f, 2);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_DIFF_PASSRATE), ImGuiTableColumnFlags_WidthStretch, ImGui::GetWindowWidth() * 0.25f, 3);
                    ImGui::TableHeadersRow();

                    if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
                        if (sorts_specs->SpecsDirty) {
                            DiffsDetailedTableItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
                            if (table_itmes.Size > 1)
                                qsort(&table_itmes[0], (size_t)table_itmes.Size, sizeof(table_itmes[0]), DiffsDetailedTableItem::CompareWithSortSpecs);
                            DiffsDetailedTableItem::s_current_sort_specs = NULL;
                            sorts_specs->SpecsDirty = false;
                        }

                    for (auto& item : table_itmes) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text("%d", item.index);
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", item.name);
                        ImGui::TableNextColumn();
                        ImGui::Text("%6d", item.count);
                        ImGui::TableNextColumn();
                        float r, g, b;
                        double passrate = item.pass_rate;
                        passrate = std::clamp(passrate, 0.0, 1.0);
                        ImGui::ColorConvertHSVtoRGB((float)(passrate * passrate * passrate * passrate)* 0.33f, 0.75f, 1.0f, r, g, b);
                        ImGui::TextColored({r,g,b,1.0f}, "%6.2lf%%", passrate * 100.0);
                    }
                    ImGui::EndTable();

                    ImGui::NewLine();
                    ImGui::Separator();
                    ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
                    if (ImGui::BeginTable(S(THPRAC_KENG_DETAILS_PASS_TABLE), 3, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable, ImVec2(ImGui::GetWindowWidth() * 0.9f, 0.0f)))
                    {
                        ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_PASS_MISS), ImGuiTableColumnFlags_WidthStretch, ImGui::GetWindowWidth() * 0.15f);
                        ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_PASS_PROB), ImGuiTableColumnFlags_WidthStretch, ImGui::GetWindowWidth() * 0.42f);
                        ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_PASS_EXCEPTION), ImGuiTableColumnFlags_WidthStretch, ImGui::GetWindowWidth() * 0.43f);
                        ImGui::TableHeadersRow();

                        for (int miss = 0; miss < std::ssize(pass_rate); miss++) {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("%6d", miss);
                            ImGui::TableNextColumn();
                            ImGui::Text("%10.6lf%%", pass_rate[miss] * 100.0);
                            ImGui::TableNextColumn();
                            if (pass_rate[miss]<1e-9)
                                ImGui::Text(S(THPRAC_KENG_DETAILS_PASS_EXCEPTION_INF));
                            else
                                ImGui::Text("%10.1lf%", std::max(1.0, 1.0 / pass_rate[miss]));
                        }
                        ImGui::EndTable();
                    }
                }
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
                if (ImGui::Button(std::format("{}##2",S(THPRAC_KENG_RETURN)).c_str()))
                    isopen = false;
                if (!isopen)
                    ImGui::CloseCurrentPopup();
                ImGui::PopTextWrapPos();
                ImGui::EndPopup();
            }
            if(!isopen)
                calculated=false;
        }

        const char* GetDescription_Line()
        {
            static char mKengDescription_line[64];
            memcpy_s(mKengDescription_line, sizeof(mKengDescription_line), mKengDescription, sizeof(mKengDescription_line));
            bool add_dots=false;
            for (int i = 0; i < 25; i++)
            {
                if (mKengDescription_line[i] == '\r' || mKengDescription_line[i] == '\n'){
                    mKengDescription_line[i] = '.';
                    mKengDescription_line[i+1] = '.';
                    mKengDescription_line[i+2] = '.';
                    mKengDescription_line[i+3] = '\0';
                    add_dots = true;
                    break;
                } else if (mKengDescription_line[i] == '\0') {
                    add_dots = true;
                    break;
                }
            }
            if (!add_dots)
            {
                mKengDescription_line[24] = '.';
                mKengDescription_line[25] = '.';
                mKengDescription_line[26] = '.';
                mKengDescription_line[27] = '\0';
            }
            return mKengDescription_line;
        }

        void WriteToCsv(std::wstring csv_filename)
        {
            mTimeCreate = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
            rapidcsv::Document doc;
            doc.SetColumnName(0, "name");
            doc.SetColumnName(1, "date");
            doc.SetColumnName(2, "comment");
            //std::vector<std::string> header;
            //header.push_back("name");
            //header.push_back("date");
            //header.push_back("comment");
            for (int i = 0; i < std::ssize(mKengDifficulties); i++)
                doc.SetColumnName(i + 3, mKengDifficulties[i].name);
                //header.push_back(mKengDifficulties[i].name);
            //doc.SetRow(0, header);
            for (int j = 0; j < std::ssize(mPlays); j++) {
                auto &play = mPlays[j];
                std::vector<std::string> row;
                row.push_back(play.mPlayName);
                row.push_back(std::format("{}/{}/{}", (int32_t)play.mTimeCreate.year(), (unsigned)(play.mTimeCreate.month()), (unsigned)(play.mTimeCreate.day())));
                row.push_back(play.mPlayComment);
                for (int df = 0; df < std::ssize(mKengDifficulties); df++) {
                    bool is_died = false;
                    for (int k = 0; k < std::ssize(play.mDiffsDied); k++) {
                        if (mKengDifficulties[df].id == play.mDiffsDied[k]){
                            row.push_back("1");
                            is_died = true;
                            break;
                        }
                    }
                    if (!is_died)
                        row.push_back("0");
                }
                doc.SetRow(j, row);
            }
            doc.Save(utf16_to_mb(csv_filename.c_str(), CP_ACP));
        }

        Keng(std::wstring csv_filename)
        {
            std::string name = utf16_to_mb(GetNameFromFullPath(csv_filename).c_str(), CP_UTF8);
            strcpy_s(mKengName, name.c_str());
            mTimeCreate = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
            memset(mKengDescription, 0, sizeof(mKengDescription));

            rapidcsv::Document doc(utf16_to_mb(csv_filename.c_str(), CP_ACP), rapidcsv::LabelParams(-1, -1));
            int idx_row = 0;
            std::vector<std::string> header = doc.GetRow<std::string>(idx_row);
            
            int diff_count = header.size() - 3;
            for (int i = 0; i < diff_count; i++){
                mKengDifficulties.emplace_back(i, header[i+3].c_str());
            }
            mDifficulties_idtot = diff_count;
            for (int i = 1; i < (int)doc.GetRowCount();i++){
                std::vector<std::string> rw = doc.GetRow<std::string>(i);
                std::string plname = rw[0];
                std::string date = rw[1];
                std::string cmt = rw[2];
                std::vector<int> ids;
                for (int j = 0; j < std::min((int)(rw.size()) - 3, diff_count); j++){
                    if (rw[j + 3] == "1")
                        ids.push_back(j);
                }
                int y, m, d;
                std::chrono::year_month_day time_play;
                if (sscanf_s(date.c_str(), "%d/%d/%d", &y, &m, &d) == 3){
                    time_play = std::chrono::year_month_day(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
                }else{
                    time_play = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
                }
                mPlays.emplace_back(plname.c_str(), cmt.c_str(), ids, time_play);
            }
        }
        Keng(const char* name, const char* desc, std::chrono::year_month_day t, std::vector<std::string> diffs) : mTimeCreate(t)
        {
            strcpy_s(mKengName, name);
            strcpy_s(mKengDescription, desc);
            mDifficulties_idtot = diffs.size();
            for (int i = 0; i < std::ssize(diffs); i++) {
                mKengDifficulties.emplace_back(i,diffs[i].c_str());
            }
        }
        Keng(std::istream& is, int ver)
        {
            switch (ver)
            {
            case 1:
            default:
            {
                is.read((char*)mKengName, sizeof(mKengName));
                is.read((char*)mKengDescription, sizeof(mKengDescription));
        
                int y,m,d;
                is.read((char*)&y, sizeof(y));
                is.read((char*)&m, sizeof(m));
                is.read((char*)&d, sizeof(d));
                mTimeCreate = std::chrono::year_month_day(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
        
                int nsz = mKengDifficulties.size();
                is.read((char*)&nsz, sizeof(nsz));
                for (int i = 0; i < nsz;i++) {
                   mKengDifficulties.emplace_back(is, ver);
                }
                is.read((char*)&mDifficulties_idtot, sizeof(mDifficulties_idtot));
        
                is.read((char*)&nsz, sizeof(nsz));
                for (int i = 0; i < nsz; i++) {
                    mPlays.emplace_back(is,ver);
                }
            }
            }
        }
        std::ostream& Write(std::ostream& os)
        {
            os.write((char*)mKengName, sizeof(mKengName));
            os.write((char*)mKengDescription, sizeof(mKengDescription));

            int y = (int32_t)mTimeCreate.year();
            int m = (unsigned)mTimeCreate.month();
            int d = (unsigned)mTimeCreate.day();
            os.write((char*)&y, sizeof(y));
            os.write((char*)&m, sizeof(m));
            os.write((char*)&d, sizeof(d));

            int nsz = mKengDifficulties.size();
            os.write((char*)&nsz, sizeof(nsz));
            for (auto& i : mKengDifficulties){
                i.Write(os);
            }
            os.write((char*)&mDifficulties_idtot, sizeof(mDifficulties_idtot));

            nsz = mPlays.size();
            os.write((char*)&nsz, sizeof(nsz));
            for (auto& i : mPlays) {
                i.Write(os);
            }
            return os;
        }

        void RemoveDiffs(int idx)
        {
            for (auto& i : mPlays)
            {
                for (auto& diff_id : i.mDiffsDied)
                {
                    if (diff_id == mKengDifficulties[idx].id){
                        return;//can not remove
                    }
                }
            }
            mKengDifficulties.erase(mKengDifficulties.begin() + idx);
            return;
        }

        void DrawKeng(bool* is_del,bool* is_open)
        {
            //keng draw
            static int play_index_draw = -1;
            if (play_index_draw != -1)
            {
                bool is_del2 = false, is_open2 = true;
                mPlays[play_index_draw].DrawPlay(&is_del2, &is_open2, mKengDifficulties, mDifficulties_idtot, [=](int idx) { RemoveDiffs(idx); });
                if (is_del2) {
                    mPlays.erase(mPlays.begin() + play_index_draw);
                    is_open2 = false;
                    play_index_draw = -1;
                }
                if (is_open2 == false)
                {
                    play_index_draw = -1;
                }
            }else {
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.05f);
                if (ImGui::Button(S(THPRAC_KENG_RETURN)))
                    *is_open = false;
                ImGui::Columns(3, 0, false);
                ImGui::SetColumnWidth(0, LauncherWndGetSize().x * 0.05f);
                ImGui::SetColumnWidth(1, 300.0f);

                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_KENG_NAME));
                ImGui::NextColumn();
                ImGui::InputText("##keng name", mKengName, sizeof(mKengName));
                ImGui::NextColumn();

                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_KENG_DATE));
                ImGui::NextColumn();
                GuiDateSelector("##keng date", &mTimeCreate);
                ImGui::NextColumn();

                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_KENG_DESC));
                ImGui::NextColumn();
                ImGui::InputTextMultiline("##keng desc", mKengDescription, sizeof(mKengDescription));
                //ImGui::TextWrapped(mKengDescription.c_str());
                ImGui::Columns(1);

                // add play
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);

                static char playName[PLAY_NAME_SIZE] = { 0 };
                static char playCmt[PLAY_CMT_SIZE] = { 0 };
                static std::chrono::year_month_day time;
                static std::vector<char> diffs_select;
                static bool isopen = false;
                if (ImGui::Button(S(THPRAC_KENG_PLAY_ADD))) {
                    isopen = true;
                    diffs_select.resize(mKengDifficulties.size());
                    for (auto& i : diffs_select)
                        i = 0;
                    time = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
                    ImGui::OpenPopup(S(THPRAC_KENG_PLAY_ADD_POPUP));
                }

                if (GuiModal(S(THPRAC_KENG_PLAY_ADD_POPUP), { LauncherWndGetSize().x * 0.95f, LauncherWndGetSize().y * 0.8f }, &isopen)) {
                    ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                    {
                        ImGui::Columns(2, 0, false);
                        ImGui::SetColumnWidth(0, 300.0f);
                        ImGui::Text(S(THPRAC_KENG_PLAY_NAME));
                        ImGui::NextColumn();
                        ImGui::InputText("##play name", playName, sizeof(playName));
                        ImGui::NextColumn();

                        ImGui::Text(S(THPRAC_KENG_PLAY_DATE));
                        ImGui::NextColumn();
                        GuiDateSelector("##play date", &time);
                        ImGui::NextColumn();

                        ImGui::Text(S(THPRAC_KENG_PLAY_CMT));
                        ImGui::NextColumn();
                        ImGui::InputTextMultiline("##play cmt", playCmt, sizeof(playCmt));

                        ImGui::Columns(1);

                        static int idx_add = -1;
                        int idx_u = -1, idx_d = -1, idx_rem = -1;
                        if (ImGui::BeginTable("##diffaddTable", 2, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable))
                        {
                            ImGui::TableSetupColumn(S(THPRAC_KENG_DIFF_TABLE_CK), ImGuiTableColumnFlags_WidthStretch, ImGui::GetTextLineHeight() * 10);
                            ImGui::TableSetupColumn(S(THPRAC_KENG_DIFF_TABLE_NAME), ImGuiTableColumnFlags_WidthStretch, ImGui::GetWindowWidth() - ImGui::GetTextLineHeight() * 10);

                            ImGui::TableHeadersRow();
                            
                            if (mKengDifficulties.size() == 0) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                if (ImGui::Button(S(THPRAC_KENG_DIFF_TABLE_ADD)))
                                    idx_add = 0;
                            }
                            
                            for (int idx = 0; idx < std::ssize(mKengDifficulties); idx++) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                if (ImGui::Button(std::format("{}##diff_add_{}", S(THPRAC_KENG_DIFF_TABLE_ADD), idx).c_str()))
                                    idx_add = idx;
                                ImGui::SameLine();
                                if (ImGui::Button(std::format("{}##diff_rem_{}", S(THPRAC_KENG_DIFF_TABLE_REM), idx).c_str()))
                                    idx_rem = idx;
                                ImGui::SameLine();
                                // u
                                if (ImGui::Button(std::format("{}##diff_u_{}", S(THPRAC_KENG_DIFF_TABLE_U), idx).c_str()))
                                    idx_u = idx;
                                ImGui::SameLine();
                                // d
                                if (ImGui::Button(std::format("{}##diff_d_{}", S(THPRAC_KENG_DIFF_TABLE_D), idx).c_str()))
                                    idx_d = idx;
                                ImGui::SameLine();
                                ImGui::Checkbox(std::format("##diff{}", idx).c_str(), (bool*)(&diffs_select[idx]));
                                ImGui::TableNextColumn();
                                ImGui::Text(mKengDifficulties[idx].name);
                            }
                            ImGui::EndTable();
                        }else {
                            idx_add = -1;
                        }
                        GuiSwapDiff(idx_u, idx_d, mKengDifficulties, diffs_select);
                        GuiInsertDiff(idx_add, mKengDifficulties, diffs_select, mDifficulties_idtot);
                        if(idx_rem!=-1)
                            RemoveDiffs(idx_rem);

                        ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                        auto retnValue = GuiCornerButton(S(THPRAC_APPLY), S(THPRAC_CANCEL), ImVec2(1.5f, 0.0f), true);
                        if (retnValue == 1) {
                            std::vector<int> ids;
                            for (int i = 0; i < std::ssize(mKengDifficulties); i++) {
                                if (diffs_select[i])
                                    ids.push_back(mKengDifficulties[i].id);
                            }
                            mPlays.emplace_back(playName, playCmt, ids, time);
                            KengSave();
                            isopen = false;
                        } else if (retnValue == 2) {
                            isopen = false;
                        }
                        if (!isopen)
                            ImGui::CloseCurrentPopup();
                        ImGui::PopTextWrapPos();
                        ImGui::EndPopup();
                    }
                }

                ImGui::SameLine();
                GuiGetDetails();
                ImGui::SameLine();
                if (ImGui::Button(S(THPRAC_KENG_SAVE_CSV)))
                {
                    OPENFILENAMEW ofn;
                    wchar_t szFile[MAX_PATH] = L".csv";
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = nullptr;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrFilter = L"csv File\0*.csv\0";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = nullptr;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = L"";
                    ofn.lpstrDefExt = L".csv";
                    ofn.Flags = OFN_OVERWRITEPROMPT;
                    if (GetSaveFileNameW(&ofn)) {
                        try {
                            WriteToCsv(szFile);
                        } catch (std::exception& e) {
                            MessageBoxA(NULL, e.what(), "Error", MB_OK);
                        }
                    }
                }
                
                
                ImGui::Separator();
                // play draw
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
                if (ImGui::CollapsingHeader(std::format("{}##header", S(THPRAC_KENG_PLAY_TABLE)).c_str(),ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
                    if (ImGui::BeginTable(S(THPRAC_KENG_PLAY_TABLE), 5, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable, ImVec2(LauncherWndGetSize().x * 0.9f, 0.0f))) {
                        ImGui::TableSetupColumn(S(THPRAC_KENG_PLAY_INDEX), ImGuiTableColumnFlags_WidthStretch, LauncherWndGetSize().x * 0.9f * 0.1f);
                        ImGui::TableSetupColumn(S(THPRAC_KENG_PLAY_NAME), ImGuiTableColumnFlags_WidthStretch, LauncherWndGetSize().x * 0.9f * 0.15f);
                        ImGui::TableSetupColumn(S(THPRAC_KENG_PLAY_DATE), ImGuiTableColumnFlags_WidthStretch, LauncherWndGetSize().x * 0.9f * 0.2f);
                        ImGui::TableSetupColumn(S(THPRAC_KENG_PLAY_CMT), ImGuiTableColumnFlags_WidthStretch, LauncherWndGetSize().x * 0.9f * 0.4f);
                        ImGui::TableSetupColumn(S(THPRAC_KENG_PLAY_CNT), ImGuiTableColumnFlags_WidthStretch, LauncherWndGetSize().x * 0.9f * 0.15f);
                        ImGui::TableHeadersRow();
                        int idx = std::ssize(mPlays) - 1;
                        for (auto it = mPlays.rbegin(); it != mPlays.rend(); it++) {

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("%2d", idx + 1);
                            ImGui::TableNextColumn();
                            if (ImGui::Selectable(std::format("{}##{}", it->mPlayName, idx).c_str(), false, ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns)) {
                                play_index_draw = idx;
                            }
                            ImGui::TableNextColumn();
                            ImGui::Text(it->GetTimeDesc());
                            ImGui::TableNextColumn();
                            ImGui::Text(it->GetDescription_Line());
                            ImGui::TableNextColumn();
                            ImGui::Text("%6d", it->mDiffsDied.size());
                            idx--;
                        }
                        ImGui::EndTable();
                    }
                }
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
                if (ImGui::Button(std::format("{}##2", S(THPRAC_KENG_RETURN)).c_str()))
                    *is_open = false;
                ImGui::SameLine();
                if (ImGui::Button(S(THPRAC_KENG_DEL))) {
                    ImGui::OpenPopup(S(THPRAC_KENG_DEL_POPUP));
                }
                if (GuiModal(S(THPRAC_KENG_DEL_POPUP), { LauncherWndGetSize().x * 0.3f, LauncherWndGetSize().y * 0.3f })) {
                    ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                    {
                        ImGui::Text(S(THPRAC_KENG_DEL_YES_OR_NO));
                        ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                        auto retnValue = GuiCornerButton(S(THPRAC_APPLY), S(THPRAC_CANCEL), ImVec2(1.0f, 0.0f), true);
                        if (retnValue == 1) {
                            *is_del = true;
                            *is_open = false;
                            ImGui::CloseCurrentPopup();
                        } else if (retnValue == 2) {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::PopTextWrapPos();
                        ImGui::EndPopup();
                    }
                }

            }
        }
        friend class KengRecorder;
    };


    class KengRecorder
    {
        std::vector<Keng> mKengs;
        int cur_draw_keng_idx=-1;
    public:
        void ReadRecords(std::istream& is)
        {
            int ver=1,keng_cnt=0;
            is.read((char*)&ver, sizeof(ver));
            is.read((char*)&cur_draw_keng_idx, sizeof(cur_draw_keng_idx));
            is.read((char*)&keng_cnt, sizeof(keng_cnt));
            for (int i = 0; i < keng_cnt; i++) {
                mKengs.emplace_back(is, ver);
            }
        }
        void WriteRecords(std::ostream& os)
        {
            int ver = 1, keng_cnt = mKengs.size();
            os.write((char*)&ver, sizeof(ver));
            os.write((char*)&cur_draw_keng_idx, sizeof(cur_draw_keng_idx));
            os.write((char*)&keng_cnt, sizeof(keng_cnt));
            for (int i = 0; i < keng_cnt; i++) {
                mKengs[i].Write(os);
            }
        }
        void DrawKengs()
        {
            if (cur_draw_keng_idx >= std::ssize(mKengs) || cur_draw_keng_idx < 0)
                cur_draw_keng_idx = -1;
            if (cur_draw_keng_idx == -1)
            {
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
                if (ImGui::BeginTable("##kengsTable", 3, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable, ImVec2(LauncherWndGetSize().x * 0.9f, 0.0f)))
                {
                    ImGui::TableSetupColumn(S(THPRAC_KENG_NAME), ImGuiTableColumnFlags_WidthStretch, LauncherWndGetSize().x * 0.9f * 0.25f);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DATE), ImGuiTableColumnFlags_WidthStretch, LauncherWndGetSize().x * 0.9f * 0.25f);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DESC), ImGuiTableColumnFlags_WidthStretch, LauncherWndGetSize().x * 0.9f * 0.5f);
                    ImGui::TableHeadersRow();

                    int idx = 0;
                    for (auto it = mKengs.begin(); it != mKengs.end(); it++) {

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(std::format("{}##{}", it->mKengName, idx).c_str(), false, ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns)) {
                            cur_draw_keng_idx = idx;
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(it->GetTimeDesc());
                        ImGui::TableNextColumn();
                        ImGui::Text(it->GetDescription_Line());
                        idx++;
                    }
                    ImGui::EndTable();
                }

                // adder
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);

                static std::chrono::year_month_day time;
                static bool isopen = false;
                if (ImGui::Button(S(THPRAC_KENG_ADD_KENG))) {
                    isopen = true;
                    time = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
                    ImGui::OpenPopup(S(THPRAC_KENG_ADD_KENG_POPUP));
                }

                if (GuiModal(S(THPRAC_KENG_ADD_KENG_POPUP), { LauncherWndGetSize().x * 0.95f, LauncherWndGetSize().y * 0.8f }, &isopen)) {
                    ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                    {
                        static char kengName[KENG_NAME_SIZE];
                        static char kengDesc[KENG_DESC_SIZE];
                        static int keng_preset_id = 0;

                        ImGui::Columns(2, 0, false);
                        ImGui::SetColumnWidth(0, 300.0f);
                        ImGui::Text(S(THPRAC_KENG_NAME));
                        ImGui::NextColumn();
                        ImGui::InputText("##keng name", kengName, sizeof(kengName));
                        ImGui::NextColumn();

                        ImGui::Text(S(THPRAC_KENG_DATE));
                        ImGui::NextColumn();
                        GuiDateSelector("##keng date", &time);
                        ImGui::NextColumn();
                        
                        ImGui::Text(S(THPRAC_KENG_PRESET));
                        ImGui::NextColumn();
                        static std::vector<std::pair<std::string, std::vector<std::string>>>* cur_preset;
                        cur_preset = GetPreset(Gui::LocaleGet());
                        ImGui::Combo("##keng preset", &keng_preset_id, []([[maybe_unused]]void* data, int idx, const char** out) -> bool {
                                *out = (*cur_preset)[idx].first.c_str();
                                return 1;
                            },nullptr,cur_preset->size());

                        ImGui::NextColumn();


                        ImGui::Text(S(THPRAC_KENG_DESC));
                        ImGui::NextColumn();
                        ImGui::InputTextMultiline("##keng desc", kengDesc, sizeof(kengDesc));

                        ImGui::Columns(1);

                        ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                        auto retnValue = GuiCornerButton(S(THPRAC_APPLY), S(THPRAC_CANCEL), ImVec2(1.0f, 0.0f), true);
                        if (retnValue == 1) {
                            mKengs.emplace_back(kengName, kengDesc, time, (*cur_preset)[keng_preset_id].second);
                            KengSave();
                            isopen = false;
                        } else if (retnValue == 2) {
                            isopen = false;
                        }
                        if(!isopen)
                            ImGui::CloseCurrentPopup();
                        ImGui::PopTextWrapPos();
                        ImGui::EndPopup();
                    }
                }

                ImGui::SameLine();
                if (ImGui::Button(S(THPRAC_KENG_READ_CSV)))
                {
                    std::wstring file = LauncherWndFileSelect(nullptr, L"csv(*.csv)\0*.csv\0*.*\0\0");
                    try {
                        mKengs.emplace_back(file);
                        KengSave();
                    } catch (std::exception& e) {
                        MessageBoxA(NULL, e.what(), "Error", MB_OK);
                    }
                }
            }else{
                bool is_del = false,is_open=true;
                mKengs[cur_draw_keng_idx].DrawKeng(&is_del, &is_open);
                if (is_del) {
                    mKengs.erase(mKengs.begin() + cur_draw_keng_idx);
                    cur_draw_keng_idx = -1;
                }
                if (!is_open){
                    cur_draw_keng_idx = -1;
                }
            }
        }
    };


class THKengRecorder {
public:
    KengRecorder mRecorder;

    THKengRecorder() { }
    SINGLETON(THKengRecorder)

private:
    
private:
public: 
    void Gui(){
        mRecorder.DrawKengs();
    }
};

class THKengsGui {
private:
    THKengsGui()
    {

    }
    SINGLETON(THKengsGui)

public:
    void GuiUpdate(){
        GuiMain();
    }

private:
    void GuiMain(){
        THKengRecorder::singleton().Gui();
    }
};

bool LauncherKengGuiUpd()
{
    THKengsGui::singleton().GuiUpdate();
    return true;
}

void LauncherKengInit()
{
    PushCurrentDirectory(L"%appdata%\\thprac");
    auto fs = ::std::fstream("keng.dat", ::std::ios::in | ::std::ios::binary);
    if (fs.is_open()) {
        THKengRecorder::singleton().mRecorder.ReadRecords(fs);
    }
    PopCurrentDirectory();
}

void LauncherKengUpdate()
{
}

void KengSave()
{
    PushCurrentDirectory(L"%appdata%\\thprac");
    auto fs = ::std::fstream("keng.dat", ::std::ios::out | ::std::ios::binary);
    if (fs.is_open()) {
        THKengRecorder::singleton().mRecorder.WriteRecords(fs);
    }
    PopCurrentDirectory();
}

void LauncherKengDestroy()
{
    KengSave();
}

}