## [English](/README.md)
# **thprac**
> thprac 是一个辅助练习东方 Project 的弹幕射击游戏的工具。

# [下载](https://github.com/touhouworldcup/thprac/releases/latest) - [下载测试版](https://nightly.link/touhouworldcup/thprac/workflows/main/master/thprac.zip)

> 注意测试版只会更新到下一个正式版，而不是下一个测试版。

## **目录**

* [如何下载](#downloading)
* [如何使用](#usage)
* [兼容性](#compatability)
* [功能介绍](#features)
* [退格键菜单](#backspace-menu)
* [高级选项](#advanced-options)
* [统计](#statistics)
* [某些游戏的专属功能](#game-specific-features)
* [贡献者名单](#credits)
* [常见问题](#faq)

## <a name="downloading"></a>如何下载

点击链接下载 [最新正式版](https://github.com/touhouworldcup/thprac/releases/latest) 或 [最新测试版](https://nightly.link/touhouworldcup/thprac/workflows/main/master/thprac.zip)。注意测试版只会更新到下一个正式版，而**不**是下一个测试版。

## <a name="usage"></a>如何使用

此工具有多种启动方式：最常用的一种是将此工具的可执行文件放在游戏的文件夹下，并在启动游戏后启动此工具。还有一种方式是使用 thprac 自带的游戏启动器。

### 在游戏文件夹内启动 thprac

**此方法不适用于 Steam 版游戏。** 你可以将 `thprac.exe` 放到游戏的文件夹下。若使用此方法，thprac 会自动检查是否有 vpatch。你需要确保游戏已更新到最新版，且游戏的可执行文件名符合下述规则：

- thXX.exe（大多数游戏）
- 東方紅魔郷.exe（红魔乡）
- alcostg.exe（黄昏酒场）

### 启动游戏后再启动 thprac

**大多数情况下此方法都适用。** 首先，以任意方式运行游戏（通过 thcrap、vpatch、Steam、等等），然后启动 `thprac.exe`。thprac 会检查目前正在运行的游戏并尝试对其应用 thprac。你可能需要返回一次主菜单才能使 thprac 生效。

### 使用 thprac 自带的启动器

下面这个视频说明了如何使用 thprac 自带的启动器。（需要科学上网）
[![A quick and dirty overview of thprac 2.0](https://i.ytimg.com/vi/sRV4PDQceYo/maxresdefault.jpg)](https://www.youtube.com/watch?v=sRV4PDQceYo)

## 使用命令行参数

thprac 支持以下命令行选项：

- `<path to game exe>` 命令 `thprac.exe <path to game exe>` 将直接运行指定的游戏并附加 thprac，而不会有任何确认提示。这允许你通过将东方游戏的 exe 文件直接拖动到 thprac.exe 上，游戏启动并附加 thprac。
- `--attach <pid>` 立刻将 thprac 注入到拥有 ID `<PID>` 的进程，不会有任何确认提示。
- `--attach` （不带其他参数）。命令 `thprac.exe --attach` 将会将 thprac 注入到第一个其找到的 东方游戏进程，不带任何确认提示。
- `--without-vpatch` 阻止自动应用 vpatch。

示例命令：

```
thprac.exe --attach 1234
thprac.exe --attach
thprac.exe C:\Users\Name\Desktop\Games\Touhou\th17\th17.exe
thprac.exe --without-vpatch C:\Users\Name\Desktop\Games\Touhou\th17\th17.exe
```

## <a name="compatability"></a>兼容性

thprac 正式支持自 **Windows Vista** 以来的所有 Windows 版本。若使用 [One-Core-API](https://github.com/Skulltrail192/One-Core-API-Binaries)，thprac 可能在 Windows XP 下也能工作，但我们**不**常测试 Windows XP 下的兼容性。

## <a name="features"></a>功能介绍

thprac 为所有主要游戏（包括妖精大战争和黄昏酒场）提供了一个增强的练习模式。

![东方虹龙洞](https://user-images.githubusercontent.com/23106652/174433923-0a6069e7-d10d-4107-8f0d-f4a8a9d56976.png)
![grafik](https://user-images.githubusercontent.com/23106652/174433975-8f23b0b0-e48e-4be1-8cb7-d8e3e7ab6b8e.png)

thprac 使用上图中的 UI 替代了原版的练习菜单。对于一些特定的符卡，你还可以选择符卡的阶段，或修改符卡参数。另外，对于那些没有官方练习界面的游戏（黄昏酒场、妖精大战争等），thprac 在“开始游戏”界面添加了练习界面。

![Uwabami Breakers](https://user-images.githubusercontent.com/23106652/174434103-5fee7a13-0254-4602-a468-42330b985bb2.png)
![Great Fairy Wars](https://user-images.githubusercontent.com/23106652/174434121-063142f2-ef3d-4721-ab96-a252343cdb0e.png)

您可以用键盘或手柄控制练习菜单。在此菜单中，可以用 UP 或DOWN 选择选项，并用 LEFT 或 RIGHT 更改选项。您也可以用鼠标操作菜单。最后按下 SHOT 确认.

Thprac 会自动将您输入的选项保存到 replay 中，播放 replay 时，thcrap 会自动应用这些选项。 **使用 thcrap 游玩和“自定义”模式练习的 replay 不适用于原版游戏**

## <a name="backspace-menu"></a> 退格键菜单

在任意一款 thprac 支持的游戏里（花映冢、兽王园除外），你都可以通过按下退格键（Backspace）以显示“退格键菜单”。此菜单显示了一系列可以通过功能键（F1 等）打开的功能。

![TH18](https://user-images.githubusercontent.com/23106652/174434813-73748a66-0f6d-4c6e-9f3a-895a49b93434.png)
![TH17](https://user-images.githubusercontent.com/23106652/174434834-6bd93104-1ed2-48ae-a440-9d9cb871ea03.png)

## <a name="advanced-options"></a>高级选项
在任意一款 thprac 支持的游戏里，你都可以通过按下 F12 键来打开高级选项菜单；这些选项提供了和游戏内容相关的 bug 修复、补丁，或一些生活质量（QoL）功能。
![TH08](https://user-images.githubusercontent.com/23106652/174434977-683da583-324b-4bd5-8408-13373dfd5a93.png)
![TH18](https://user-images.githubusercontent.com/23106652/174435006-e906d30d-0ef5-4930-ae57-1f0919beb5af.png)

## <a name="game-specific-features"></a>游戏特定功能
### 弹幕狂们的黑市：强制选波

![100th Black Market](https://github.com/touhouworldcup/thprac/assets/23106652/02c55e5e-5c89-462f-beea-9ab07cbb1051)

如果在 thprac 提供的界面中使用”自定义练习“模式，则每次游戏将要选择一个波次的敌人时，上述界面就会出现并允许玩家选择某个特定的波次。

### 花映冢：实用工具

![TH09 Tools](https://user-images.githubusercontent.com/23106652/174434249-2bf1d70a-101c-4538-a4e6-8eeaf273dd88.png) 

在对战模式开启游戏时，你可以在“模式”选项中选择“自定义练习”。如果这么做，上图中的窗口会出现在游戏中。它可以根据需要移动和调整窗口大小，并且可以按 F11 完全隐藏。

### 兽王园：实用工具

![TH19 Tools](https://github.com/touhouworldcup/thprac/assets/23106652/a2cdb385-b61d-4111-af6b-b195e85bf18a)

### 红魔乡：暂停菜单

![TH06](https://user-images.githubusercontent.com/23106652/174436027-734d642a-300c-45ab-9591-b6219aca087b.png)
选择“退出游戏”时，游戏将提示您保存 replay。

**警告：** 这样保存的 replay 在您选择保存的时间点后还会继续播放。在此之后自机会停止不动并且停止射击。

### 秘封噩梦日记、鬼形兽、虹龙洞：为 ESC + R 提供 bug 修复

## <a name="credits"></a> 贡献者名单（2022 年 5 月 27 日及之后）
- 开发: [32th System](https://www.youtube.com/channel/UChyVpooBi31k3xPbWYsoq3w), [muter3000](https://github.com/muter3000), [zero318](https://github.com/zero318), [Lmocinemod](https://github.com/Lmocinemod), [Cao Minh](https://github.com/hoangcaominh), [raviddog](https://github.com/raviddog) 
- 中文翻译: [CrestedPeak9](https://twitter.com/CrestedPeak9), maksim71_doll, DeepL
- 日语翻译: [Yu-miya](https://www.twitch.tv/toho_yumiya), CyrusVorazan, DeepL
- 将所有 bug 追踪报告从 [旧的 bug 追踪处](https://github.com/ack7139/thprac/issues) 移到 [此处](https://github.com/touhouworldcup/thprac/issues): [toimine](https://www.youtube.com/channel/UCtxu8Rg0Vh8mX6iENenetuA)
- 中文 README 翻译：[TNT569](https://github.com/TNT569)、[H-J-Granger](https://github.com/H-J-Granger)

## <a name="faq"></a> 常见问题

### 综合

#### 为什么 thprac 项目移动了?
原本的开发者 Ack 已经无限期停止了 thprac 或 Marketeer 的开发。这是他的声明：
> 我将无限期停止未来 thprac 和 Marketeer 的所有开发，您可以自由地继续开发它，只要您遵守许可条款。
> 由于我的技术不足，代码中充斥着晦涩难懂的文字和糟糕的逻辑，把所有事搞得一团糟。如果对你造成困扰，我很抱歉。

我们目前不能联系到 Ack，且目前 thprac 的开发已被其他人接手。

#### 我的杀毒软件报毒了。这个工具安全吗?
thprac 不含有恶意代码，但由于其工作方式的特殊性，杀毒软件可能会报毒。如果出现了这种情况，你可以选择将 thprac 添加到杀毒软件的例外区（或信任区）。你也可以尝试使用 [这个版本](https://github.com/touhouworldcup/thprac/issues/112)。如果仍然报毒，请向我们报告。

### 兼容性

#### thprac 和翻译补丁兼容吗？
thprac 与 thcrap 兼容。thprac 的启动器也可以启动 thcrap，与 [Universal THCRAP Launcher](https://github.com/thpatch/Universal-THCRAP-Launcher/) 相似。使用 thcrap 启动游戏时，也可以应用 thprac。thprac 与静态英文补丁不兼容，例如 gensokyo.org 制作的补丁。

#### thprac 找不到红魔乡。我该怎么办？
确保你的 exe 文件名为 "東方紅魔郷.exe" 或 "th06.exe"。如果 thprac 仍然找不到，你可以手动启动具有任何文件名的红魔乡 exe 并应用 thprac。

### 关于功能

#### 怎么在 TH06/TH07/TH08/TH10 的游戏进行到半时保存 replay？

在游戏中途保存 replay 由于技术限制而暂不支持。thprac 2.0.8.3 的说明中说：

> 因为这些游戏的编程方式，添加这个功能会十分麻烦，所以目前没有直接的解决方案。

然而，目前已经支持在红魔乡游戏进行到半时保存 replay。

注：在游戏中途保存的 replay 不会在保存的时刻后回到标题界面。

#### “永续BGM” 是什么意思？
当你重新开始游戏时，其可以防止 BGM 重置。

#### “暴力取材” 是什么意思？(文花帖/文花帖DS)
始终将相机锁定在 boss 身上并消除相机的装填时间。

#### 如何切换语言？

- thprac 启动器的语言会根据你的 Windows 显示语言自动选择。
- 若要修改游戏中界面的语言，请使用快捷键：
  - **ALT + 1**：日语
  - **ALT + 2**：中文
  - **ALT + 3**：英语

上述快捷键不能在启动器本身使用。请查看“如何使用”里的视频，其内有更详细的说明。

### Bug 报告

#### 在哪里报告 bug？

请在 GitHub 中的 [Issues 标签栏](https://github.com/touhouworldcup/thprac/issues) 反馈 bug 或建议。

### 技术问题

#### 高级选项显示“不支持的 VsyncPatch 版本”
你可以在 [此处](https://maribelhearn.com/tools#vpatch) 找到我们兼容的 vpatch 版本。如果可以，请尽可能使用 **rev7** 中的 DLL。

#### 使用 vpatch 时似乎无法调整 FPS
某些工具（例如“DX8 to DX9 Converter”）与 VsyncPatch 冲突。注意 replay 的播放速度调整（加速/减速）仅在神灵庙中得到支持。

#### 什么是“辉针城 - 魔理沙激光相关”？
这个功能能够修复辉针城里臭名昭著的魔理沙激光炸 replay 的 bug。
