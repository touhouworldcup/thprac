## [简体中文](/README_CN.md)
# thprac
A tool for practicing Touhou Shmups
## [Download](https://github.com/touhouworldcup/thprac/releases/latest) - [Donate](https://ko-fi.com/thprac)

# How to use
Either: place the executable in the same folder as your game, and run it. The game must be updated to the latest version and the executable must have it's original filename (thXX.exe for most games, 東方紅魔郷.exe for Embodment of Scarlet Devil, alcostg.exe for Uwabami Breakers). Thprac will automatically detect vpatch. Steam versions will not work with this method.

Or: Run the game in whatever way you want (be it through thcrap, with vpatch, through Steam, etc...), then run the thprac exe. Thprac will automatically detect the running game and attach itself to it. You may need to return to the main menu for thprac to take effect.

Or: Use the launcher. This video explains how the launcher works.
[![A quick and dirty overview of thprac 2.0](https://i.ytimg.com/vi/sRV4PDQceYo/maxresdefault.jpg)](https://www.youtube.com/watch?v=sRV4PDQceYo)

# Features
## Enchanced practice mode (supports all main games, Great Fairy Wars and Uwabami Breakers. Does not support Phantasmagoria of Flower View)
![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174433923-0a6069e7-d10d-4107-8f0d-f4a8a9d56976.png)
![grafik](https://user-images.githubusercontent.com/23106652/174433975-8f23b0b0-e48e-4be1-8cb7-d8e3e7ab6b8e.png)

It replaces the original practice menu with this UI. It also adds a practice menu to "Start Game" in Great Fairy Wars and Uwabami Breakers, which don't have a built in practice mode. On certain spells you can select phases or change certain parameters.

![Uwabami Breakers](https://user-images.githubusercontent.com/23106652/174434103-5fee7a13-0254-4602-a468-42330b985bb2.png)
![Great Fairy Wars](https://user-images.githubusercontent.com/23106652/174434121-063142f2-ef3d-4721-ab96-a252343cdb0e.png)

The menu can be controlled with your keyboard or controller. With this method, you can select the option with UP or DOWN, and change the option with LEFT or RIGHT. You can also use the mouse. Press SHOT to confirm.

Thprac will automatically save the options you input into this into any replay saved. When playing back the replay, thprac will automatically apply those options. **Replays saved when playing with thprac and the "Mode" set to "Custom" will not work with the unmodified game**

## Backspace menu
In any supported game (except for TH09) you can press BACKSPACE to access the Backspace menu. It will show you options that can be activated with the F keys.

![TH18](https://user-images.githubusercontent.com/23106652/174434813-73748a66-0f6d-4c6e-9f3a-895a49b93434.png)
![TH17](https://user-images.githubusercontent.com/23106652/174434834-6bd93104-1ed2-48ae-a440-9d9cb871ea03.png)

## Advanced options
In any supported game, you can press F12 to access the advanced options.
![TH08](https://user-images.githubusercontent.com/23106652/174434977-683da583-324b-4bd5-8408-13373dfd5a93.png)
![TH18](https://user-images.githubusercontent.com/23106652/174435006-e906d30d-0ef5-4930-ae57-1f0919beb5af.png)

## Statistics (WIP)
In TH13, press L to view a more advanced HUD
![TH13](https://user-images.githubusercontent.com/23106652/174435095-51554934-83d3-4d05-847d-834f3068bc28.png)

## Game specific features
### TH09 Tools (for Phantasmagoria of Flower View)
![TH09 Tools](https://user-images.githubusercontent.com/23106652/174434249-2bf1d70a-101c-4538-a4e6-8eeaf273dd88.png) 

When starting a game in Match Mode, you can select "Custom" in a "Mode" prompt. If you do that, this window will appear in your game. It can be moved and resized as you want and can be fully hidden by pushing F11.

### TH06 pause menu
![TH06](https://user-images.githubusercontent.com/23106652/174436027-734d642a-300c-45ab-9591-b6219aca087b.png)
Choosing "Exit" will prompt you to save a replay. Warning: replays saved like this will keep going even after the point you chose to save the replay. The player character will stand still and not shoot.

### ESC + R fix for Violet Detector, Wily Beast and Weakest Creature and Unconnected Marketeers

# Credits (May 27th 2022 and after)
- Development: [32th System](https://www.youtube.com/channel/UChyVpooBi31k3xPbWYsoq3w), [muter3000](https://github.com/muter3000), [zero318](https://github.com/zero318), [Lmocinemod](https://github.com/Lmocinemod), [Cao Minh](https://github.com/hoangcaominh), [raviddog](https://github.com/raviddog) 
- Chinese translations: [CrestedPeak9](https://twitter.com/CrestedPeak9), DeepL
- Japanese translations: [Yu-miya](https://www.twitch.tv/toho_yumiya), CyrusVorazan, DeepL
- Transferring all reports from the [old bugtracker](https://github.com/ack7139/thprac/issues) to [here](https://github.com/touhouworldcup/thprac/issues): [toimine](https://www.youtube.com/channel/UCtxu8Rg0Vh8mX6iENenetuA)
- Chinese README translations: [TNT569](https://github.com/TNT569) 

# How to build:
A Visual Studio solution is bundled that uses the Visual Studio 2022 build tools (v143) for building. However, thprac will compile perfectly fine with Visual Studio 2019 build tools (v142) as well. Your builds will support Windows XP perfectly fine, v141_xp build tools are not needed.

If you add a new string to the codebase that needs to be localized, put it into thprac_games_def.json and use the [thprac devtools](https://github.com/touhouworldcup/thprac_utils) to generate a new thprac_locale_def.h

# Q&A

## Why did thprac move?
Ack has completely stopped thprac development and can not be contacted at the moment. This is the statement on his original repository
> I will indefinitely cease all future development on thprac/Marketeer, you are free to continue working on it, granting that you follow the license's terms.
> Due to my insufficient skill, the code is glutted with obscure writings and terrible logic, turning the whole thing into a complete mess. I'm sorry if that disturbs you.

## My antivirus says it's malware. It is really?
Thprac contains no malicious code, but due to how it works, an antivirus might complain about it. If your antivirus complains about thprac, add a trust/exception. Also try [this version](https://github.com/touhouworldcup/thprac/issues/112) and report back if your antivirus does not detect it

## English patch support
Thprac is compatible with thcrap. Thprac's launcher can act as a thcrap launcher as well, similair to [Universal THCRAP Launcher](https://github.com/thpatch/Universal-THCRAP-Launcher/). When launching a game with thcrap, thprac will be able to attach to it. Thprac is not compatible with static English patches, like the ones made by gensokyo.org

## Cannot find Embodiment of Scarlet Devil
Make sure your exe is named "東方紅魔郷.exe" or "th06.exe". If thprac still can't find it, the launcher can launch an exe with any filename and attach thprac.

## How do I save a replay mid-game in TH06TH07/TH08/TH10?
You can't. The thprac 2.0.8.3 manual says
> Because of how these games were programmed, adding this feature would be quite troublesome, so currently there's no direct solution to this

Recently, this feature was added to TH06. However, any replay saved mid game will not return to the title screen even after the point where you saved the replay. Consider this functionality "there and ready but still a bit quirky"

## What does "Everlasting BGM" mean?
It prevents the BGM from resetting when you reset the game

## What does "Coercive Reporting" mean? (Shoot the Bullet/Double Spoiler)
It locks the camera onto the boss at all time and removes the camera's recharge time

## How do I switch language?
Thprac will automatically set it's language to whatever you set in the launcher. It will automatically set the launcher's language based on your Windows language. The video in "How to use" explains how to change language in the launcher right at the start. With ALT + 1, 2 or 3 you can switch language in-game. This shortcut does not work in the launcher

## Where to report bugs?
[Scroll up and click the "Issues" tabs. Alternatively, clicking this text will take you there](https://github.com/touhouworldcup/thprac/issues)

## It says "Unsupported VsyncPatch verson" in the advanced options
You can find a compatible version of vpatch [here](https://maribelhearn.com/tools#vpatch). Use DLLs from rev7 whenever you can

## FPS adjustment seems to be broken when using vpatch
Certain tools ("DX8 to DX9 Convertor" for example) conflict with VsyncPatch

## What is "DDC - Marisa Laser Related"?
It's an attempt to fix the infamous Marisa laser desync bug in Double Dealing Character
