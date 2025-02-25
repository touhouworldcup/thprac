## This is a branch of 2.2.1.8 which added more features(which might be buggy though), changes are:
- re-add unlock monitor refresh rate to thprac
- added a spellcard capture rate monitor for EoSD (copyed from others' project) (use backspace and F8/F9 or F12 to show)
- game time counter for EoSD
- added a window resize option when open game (in settings - Game adjustments)
- fix bugs for using local data directory
- add a reaction test in Tools tabbar
- added retry time monitor for LoLK
- add SOCD option for keyboard (in settings - Game adjustments), since some keyboard already has this function, I think it's ok to use when playing TH games
- add ingame info for many games(6,7,8,10,11,12,13,14,15,16,17,18,128), which includes miss count, bomb count and other counts(like UFO count), use key F8(key 1 in th18) to enable
- fix a bug for imgui of d3d8
- add a X/shift/Z-key disable in f12 menu for lnb neta
- add a keybind change function
- add complete spell capture rate for th10-18(not showing master when capture trial>99)(use f12 menu)
- add one key die for th7,8,10 to save rep quickly(f12 menu)
- add buggy force boss DOWN(F12)
- add a option in settings to disable F10 for th11-13 (in case toggle fullscreen/window)
- add esc + r/ctrl+shift+down to quick restart in EoSD
- add a option for EoSD to pause BGM when using esc pause menu
- add a phase selection for IN st6A LSC;  add FORCE wave 2 for 123 normal 1; add inf time for momoji; add P1-P4 rage for th11 final spell
- add hint editor for MoF
- add font bug fix(mainly chinese) and customized font
- add total game time and game time too long hint
- add clock-in in others menu(only in chinese)
- add keng tools
- add some advanced options like show rank in EoSD, show bonus count in DDC,...
- fix bug for not showing window when open game
- add a ab test in th15, 123's normal
- add roll from csv
- add LW practice for IN
- make IN and PCB ignore .exe checksum to avoid unexpected changes in .exe file(like using SpecialK and game crashes)
- add win key disable
- add key monitor
- disable joystick to avoid lags
- inf time for EoSD magic books / MoF soya beans formation / SA sperm bath / UFO st4 random bullet formation / UFO st5 formation / LoLK yellow laser/ LoLK green laser / LoLK yellow-green-blue / LoLK shining shooting stars / HSiFS st4 formation / UM mung bean formation / UM yellow wind god / UM st5 reinforcements / UM st5 formation
- make Remilia not be blue
- EoSD fix rand seed(just like play a replay before playing, for high score which needs fixed items drop)

## 给 thprac 2.2.1.8 加了个实用(?) branch, 其加入的功能如下
- 重新把解锁刷新率加回来了
- 红魔乡收率显示 (进入 backspace 界面后按 F8/F9 或开 F12)
- 红魔乡游戏时间计时器
- 可以在游戏开始前修改窗口大小
- 修复使用本地目录的一个八哥
- 加了个反应力测试功能(在工具界面)
- 给绀珠传加入了无欠模式的Re数显示
- 内置键盘SOCD功能, 考虑到部分高端键盘已经有这个功能了所以我觉得加入了应该也无所谓(吧大概
- 内置游戏内部分数据显示
- 修复一个 imgui-d3d8 的八哥
- 可以在F12菜单里扣X/shift/Z键了
- 改键功能
- 收率查看(10-18)
- th7,8,10的一键疮痍(F12)(用于光速存rep)
- 八哥多多的boss下压器(F12)
- 设置th11-13靳用F10(以防窗口切换)
- 红魔乡ESC+R/ctrl+shift+下 快速重开
- 红魔乡3036后自动暂停bgm(设置界面)
- 给蓬莱之药加了阶段选择; 给123一非加了阶段选择; 给狗椛加了无限时间; 地底太阳P1-4狂暴
- 给风加了个hint编辑器
- 字体修复(thwiki.cc/官方游戏/错误解决办法   字体不正常一节)与自定义字体
- 总游戏时间记录, 游戏时间过长提示功能
- 打卡功能
- 填坑器
- 一些高级IGI功能
- 修复开游戏后没有窗口的bug
- 在绀珠传123非符后面加了个底力测试(进入不同难度和修改初始P数以改变难度, 开局射几枪以改变P5弹幕为中玉二非光玉)
- 使用CSV文件随机roll东西
- 永夜抄LW练习
- 妖/永禁用checksum, 从而如汉化版等修改版的rep和score.dat不会导致游戏崩溃/游戏存档清零了(但是rep可能仍然会炸)
- 禁用win键
- 按键显示
- 禁用手柄
- 魔法书/风黄豆阵/殿精子浴/船四后半/船五后半/绀黄激光/绀绿激光/绀黄绿蓝/绀流星雨/璋四飞行阵/洞绿豆阵/洞黄风神/洞五后半增援/洞五后半飞行阵 无尽模式
- 不准蕾咪蓝狗
- 红固定随机数(等价于播放一个高级rep, 用于打分固定掉落)

## 25.2.25
- 红妖永锁p修复

## 25.2.23
- 红, 永练习模式/rep中判定显示

## 25.2.21
- 永夜抄4面后半道中对话

## 25.2.18
- 红魔乡BGM暂停修复(当使用永续BGM时正常游戏仍然暂停BGM)
- 红魔乡自动输入机签
- 红魔乡下压器

## 25.2.15
- 绀珠传st3-3, st4-2 跳过前半部分

## 25.2.13
- 虹龙洞强制卡牌
- 红魔乡F9强制显示鼠标

## 25.2.8
- 绀珠传绿激光

## 25.2.5
- 禁用更新
- 加了一普朗克描述文本

## 25.1.31
- 神灵界(H/L)增强模式
- 常夜樱发狂

## 25.1.26
- 百合狂暴波
- 一键重置当前飞行时间

## 25.1.14
- 给th18加入了必定反走/正走

## 25.1.12
- 光速猫步
- 红魔乡判定点在咲夜时停的时候不会停
- th06 返回 按钮对齐
- 锁残->防疮
- 小五飞行虫选择左开还是右开
- 猫步开局就打第二波的练习器
- 123一非选择相位
- 魔神P选择