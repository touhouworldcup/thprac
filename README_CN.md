## [English](/README.md)
# thprac
一个东方Project弹幕射击游戏练习器（既是练习器，又是游戏启动器！）
## [下载](https://github.com/touhouworldcup/thprac/releases/latest)

# 如何使用
Either: 把可执行文件放在和游戏相同的文件夹中，然后运行。游戏必须更新到最新版本并且保留其原本的文件名 (大多游戏为 thXX.exe，东方红魔乡为 東方紅魔郷.exe，黄昏酒场为 alcostg.exe)。Thprac 会自动检测 vpatch。 Steam 版本不适用此方法.

Or: 以任意方式运行游戏 (通过 thcrap，附加 vpatch，通过 Steam，等等)，然后运行 thprac exe。Thprac 会自动检测运行中的游戏并附加。您可能需要返回主菜单才能使 thprac 生效。

Or: 使用自带游戏启动器。这个视频交代了如何使用（需要科学上网）
[![A quick and dirty overview of thprac 2.0](https://i.ytimg.com/vi/sRV4PDQceYo/maxresdefault.jpg)](https://www.youtube.com/watch?v=sRV4PDQceYo)

# 功能
## 增强练习模式 (支持所有主要游戏，包括妖精大战争和黄昏酒场。不支持花映塚)
![东方虹龙洞](https://user-images.githubusercontent.com/23106652/174433923-0a6069e7-d10d-4107-8f0d-f4a8a9d56976.png)
![grafik](https://user-images.githubusercontent.com/23106652/174433975-8f23b0b0-e48e-4be1-8cb7-d8e3e7ab6b8e.png)

使用这个UI替换了原版练习菜单。另外在妖精大战争和黄昏酒场（没有练习模式）的开始游戏（"Start Game"）界面添加了练习菜单。您可以选择符卡和符卡阶段或者修改某些参数。

![Uwabami Breakers](https://user-images.githubusercontent.com/23106652/174434103-5fee7a13-0254-4602-a468-42330b985bb2.png)
![Great Fairy Wars](https://user-images.githubusercontent.com/23106652/174434121-063142f2-ef3d-4721-ab96-a252343cdb0e.png)

您可以用键盘或手柄控制练习菜单。在此菜单中，可以用 UP 或DOWN 选择选项，并用 LEFT 或 RIGHT 更改选项。您也可以用鼠标操作菜单。最后按下 SHOT 确认.

Thprac 会自动将您输入的选项保存到 replay 中，播放 replay 时，thcrap 会自动应用这些选项。 **使用 thcrap 游玩和“自定义”模式练习的 replay 不适用于原版游戏**

## 退格键(Backspace)菜单
在任何支持的游戏中 (除了 TH09) 你可以按下 BACKSPACE 来显示 “退格键菜单”。菜单显示了可以使用 F 功能键激活的选项。

![TH18](https://user-images.githubusercontent.com/23106652/174434813-73748a66-0f6d-4c6e-9f3a-895a49b93434.png)
![TH17](https://user-images.githubusercontent.com/23106652/174434834-6bd93104-1ed2-48ae-a440-9d9cb871ea03.png)

## 高级选项
在任何支持的游戏中，你可以按下 F12 来打开高级选项。
![TH08](https://user-images.githubusercontent.com/23106652/174434977-683da583-324b-4bd5-8408-13373dfd5a93.png)
![TH18](https://user-images.githubusercontent.com/23106652/174435006-e906d30d-0ef5-4930-ae57-1f0919beb5af.png)

## 统计 (神灵庙)
在 TH13 中，按下 L 可显示更高级的 HUD
![TH13](https://user-images.githubusercontent.com/23106652/174435095-51554934-83d3-4d05-847d-834f3068bc28.png)

## 游戏特定功能
### TH09 工具 (花映塚专用)
![TH09 Tools](https://user-images.githubusercontent.com/23106652/174434249-2bf1d70a-101c-4538-a4e6-8eeaf273dd88.png) 

在对战模式开启游戏时，你可以在“模式”选项中选择“自定义”。如果这么做，这个窗口会出现在游戏中。它可以根据需要移动和调整窗口大小，并且可以按 F11 完全隐藏。

### TH06 暂停菜单
![TH06](https://user-images.githubusercontent.com/23106652/174436027-734d642a-300c-45ab-9591-b6219aca087b.png)
选择“退出游戏”时将提示您保存repaly。警告：这样保存的 replay 在您选择保存的时间点后还会继续播放。在此之后自机会停止不动并且停止射击。

### 为天空璋、鬼形兽、虹龙洞修复 ESC + R 重开快捷键

# 鸣谢 (2022年5月27日及之后)
- 开发: [32th System](https://www.youtube.com/channel/UChyVpooBi31k3xPbWYsoq3w), [Cao Minh](https://github.com/hoangcaominh), [raviddog](https://github.com/raviddog) 
- 中文翻译: [CrestedPeak9](https://twitter.com/CrestedPeak9), DeepL
- 日语翻译: [Yu-miya](https://www.twitch.tv/toho_yumiya), CyrusVorazan, DeepL
- 将所有report从 [旧的bugtracker](https://github.com/ack7139/thprac/issues) 移到 [此处](https://github.com/touhouworldcup/thprac/issues): [toimine](https://www.youtube.com/channel/UCtxu8Rg0Vh8mX6iENenetuA)

# 如何构建:
捆绑了一个 Visual Studio 解决方案，该解决方案使用 Visual Studio 2022 build tools (v143) 进行构建。但是，thprac 也可以使用 Visual Studio 2019 build tools (v142) 完美编译。您的构建将完全支持 Windows XP，而不需要 v141_xp build tools。

如果您想在本地代码仓库中添加新的字符串，请将其放入thprac_games_def.json 并使用 [thprac devtools](https://github.com/touhouworldcup/thprac_utils) 来生成新的 thrac_locale_def.h

# Q&A

## 为什么 thprac 项目移动了?
Ack 已经完全停止了 thprac 的开发，且暂时无法联系。这是他在原本存储库上的声明
> 我将无限期地停止未来 thprac/Marketeer 的所有开发，您可以自由地继续开发它，前提是您遵守许可条款。
> 由于我的技术不足，代码中充斥着晦涩难懂的文字和糟糕的逻辑，把整个事情搞得一团糟。如果打扰到了您，我很抱歉。

## 我的杀毒软件报毒了。这其中含有病毒吗?
Thprac 不含有恶意代码，但由于其工作方式，杀毒软件可能误报。如果您的杀毒软件报毒了，请将 thprac 添加到例外/信任区

## 英文和其他语言支持
Thprac 与 thcrap 兼容。Thprac 的启动器也可以启动 thcrap，与 [Universal THCRAP Launcher](https://github.com/thpatch/Universal-THCRAP-Launcher/) 相似。使用thcrap启动游戏时，也可以应用 thprac。Thprac 与静态英文补丁不兼容，例如 gensokyo.org 制作的补丁

## 找不到红魔乡
确保你的 exe 名为 "東方紅魔郷.exe" 或 "th06.exe"。如果thcrap 仍然找不到，启动器可以启动具有任何文件名的 exe 并应用 thprac。

## 怎么在 TH06/TH07/TH08/TH10 的游戏进行到半时保存 replay？
不能。thprac 2.0.8.3 的说明中说：
> 因为这些游戏的编程方式，添加这个功能会十分麻烦，所以目前没有直接的解决方案

## “永续BGM” 是什么意思？
当你重新开始游戏时，其可以防止BGM重置

## “暴力取材” 是什么意思？(文花帖/文花帖DS)
始终将相机锁定在 boss 身上并消除相机的装填时间

## 如何切换语言？
Thprac 会自动将语言设置得与启动器一致。Thcrap 会根据 Windows 设置的语言自动设置。“如何使用” 中的视频解释了如何在一开始时就在启动器中更改语言。在游戏中可以使用 ALT + 1、2、3 在游戏中切换语言。此快捷键在启动器中无法使用。

## 在何处报告bugs？
[向上滚动并单击 “Issue” 选项卡。或者单击此文本](https://github.com/touhouworldcup/thprac/issues)

## 高级选项显示 “不支持的 VsyncPatch 版本”
你可以在 [此处](https://maribelhearn.com/tools#vpatch) 找到 vpatch 的兼容版本。尽可能使用 rev7 中的 DLL

## 使用 vpatch 时似乎无法调整 FPS
某些工具（例如“DX8 to DX9 转换器”）与 VsyncPatch 冲突

## 什么是“辉针城 - 魔理沙激光相关”
这是修复辉针城中臭名昭著的魔理沙激光在 replay 中不同步错误的尝试
