# **thprac**
> thprac is a tool for practicing Touhou shmups.

## **Table of Contents**
* [Downloading](#downloading)
* [Usage](#usage)
* [Compatability](#compatability)
* [Features](#features)
* [Backspace Menu](#backspace-menu)
* [Advanced Options](#advanced-options)
* [Statistics](#statistics)
* [Game Specific Features](#game-specific-features)
* [Credits](#credits)
* [FAQ](#faq)

## <a name="downloading"></a> **Downloading**
If you want to download the tool, you can download the [latest release](https://github.com/touhouworldcup/thprac/releases/latest) here and the [latest beta](https://nightly.link/touhouworldcup/thprac/workflows/main/master/thprac.zip) here.

## <a name="usage"></a> **Usage**
You can use this tool in a variety of ways, the main ways to do this are **placing the executable in the same folder as your game**, **running the executable after running your game**, or **use the thprac launcher**.

### **Using the folder**
**This method will NOT work with Steam games.** You can run the thprac tool by placing the `thprac.exe` file in the same folder as your Touhou game. This method will also automatically detect vpatch. You must ensure that the game is updated to the latest version and follows one of the following naming schemes:
- thXX.exe (most games)
- 東方紅魔郷.exe (Embodiment of Scarlet Devil)
- alcostg.exe (Uwabami Breakers)

### **Running the Executable**
**This method will work for most methods.** First, run the game in whichever method you want (whether it's thcrap, using vpatch, Steam, etc.) and then run `thcrap.exe`. thcrap will then detect the running game attempt to attach itself to it. You may have to return to the menu screen to see thprac take effect.

### **Using the Launcher**
This video explains how to use the launcher.
[![A quick and dirty overview of thprac 2.0](https://i.ytimg.com/vi/sRV4PDQceYo/maxresdefault.jpg)](https://www.youtube.com/watch?v=sRV4PDQceYo)

## <a name="compatability"></a> **Compatability**
thprac is compatible with all Windows versions proceeding and including Windows Vista. You may attempt to apply thprac on Windows XP using the [One-Core-API](https://github.com/Skulltrail192/One-Core-API-Binaries) but this is not guranteed to work.

thprac is also compatible with **Wine** and the Steam Deck. Do note that the backspace menu is tailored to work well with the Steam Deck, although this is untested.

## <a name="features"></a> **Features**
thprac features an enhanced practice mode that supports all main games along with Great Fairy Wars and Uwabami Breakers, do note that thprac **does not support Phantasmagoria of Flower View**.

![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174433923-0a6069e7-d10d-4107-8f0d-f4a8a9d56976.png)

![Imperishable Night](https://user-images.githubusercontent.com/23106652/174433975-8f23b0b0-e48e-4be1-8cb7-d8e3e7ab6b8e.png)

thprac replaces the original practice menu with a new UI shown in the image above, certain spells allow you to select phases or change certain parameters.

![Uwabami Breakers](https://user-images.githubusercontent.com/23106652/174434103-5fee7a13-0254-4602-a468-42330b985bb2.png)
![Great Fairy Wars](https://user-images.githubusercontent.com/23106652/174434121-063142f2-ef3d-4721-ab96-a252343cdb0e.png)

This menu can be controlled using your keyboard or your controller, using this, you can selection options using the **UP** and **DOWN** key and change the option using the **LEFT** and **RIGHT** keys. You can also use your mouse to interact with the menu.

thprac will also automatically save the options you selected into any replay saved. When playing back the replay, thprac will automatically apply said options. **Replays saved whilst thprac is active using "Custom" mode will NOT work with the unmodified game.**

## <a name="backspace-menu"></a> **Backspace Menu**
In any supported game (**except Touhou 9**), you can press the **backspace** key to enter the backspace menu, the backspace menu will show you a variety of options that can be activated using the function (F) keys.

Do note that on the **Steam Deck** the options are clickable, you can use your left thumb to tap the options to toggle them. As a result, you only need to map the backspace key in Steam Input.

![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174434813-73748a66-0f6d-4c6e-9f3a-895a49b93434.png)
![Wily Beast and Weakest Creature](https://user-images.githubusercontent.com/23106652/174434834-6bd93104-1ed2-48ae-a440-9d9cb871ea03.png)

## <a name="advanced-options"></a> **Advanced Options**
In any supported game, you may use the F12 key to access the advanced options, these options may offer fixes or patches related to the game or other quality of life features.

![Imperishable Night](https://user-images.githubusercontent.com/23106652/174434977-683da583-324b-4bd5-8408-13373dfd5a93.png)
![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174435006-e906d30d-0ef5-4930-ae57-1f0919beb5af.png)

## <a name="statistics"></a> **Statistics (WIP)**
If you are in Touhou 13, you can press the "L" key to access a more advanced HUD.

![TH13](https://user-images.githubusercontent.com/23106652/174435095-51554934-83d3-4d05-847d-834f3068bc28.png)

## <a name="game-specific-features"></a> **Game-specific Features**
Some games such as **100th Black Market** and **Phantasmagoria of Flower View** have game specific features to make practicing easier.

### 100th BM - Force Wave
![100th Black Market](https://github.com/touhouworldcup/thprac/assets/23106652/02c55e5e-5c89-462f-beea-9ab07cbb1051)

If thprac is enabled using "Custom" mode, this popup will appear whenever the game is about to randomly choose a wave.

### PoFV - Tools
![Phantasmagoria of Flower View](https://user-images.githubusercontent.com/23106652/174434249-2bf1d70a-101c-4538-a4e6-8eeaf273dd88.png) 

When starting a game in Match Mode, you can select "Custom" in a "Mode" prompt. If you do that, this window will appear in your game. You can move and adjust this window as much as you want. This window can be hidden by pressing the "F11" key.

### UDoALG - Tools
![TH19 Tools](https://github.com/touhouworldcup/thprac/assets/23106652/a2cdb385-b61d-4111-af6b-b195e85bf18a)

It's just like TH09 Tools, but for TH19

### EoSD - Pause Menu
![TH06](https://user-images.githubusercontent.com/23106652/174436027-734d642a-300c-45ab-9591-b6219aca087b.png)

Choosing "Exit" will prompt you to save a replay. **Warning**: replays saved like this will continue after the point you chose to save the replay. The player character will stand still and not shoot.

### ESC + R fix for Violet Detector, Wily Beast and Weakest Creature and Unconnected Marketeers

## <a name="credits"></a> **Credits (post May 27th, 2022)**
- Development: [32th System](https://www.youtube.com/channel/UChyVpooBi31k3xPbWYsoq3w), [muter3000](https://github.com/muter3000), [zero318](https://github.com/zero318), [Lmocinemod](https://github.com/Lmocinemod), [Cao Minh](https://github.com/hoangcaominh), [raviddog](https://github.com/raviddog) 
- Chinese translations: [CrestedPeak9](https://twitter.com/CrestedPeak9), maksim71_doll, DeepL
- Japanese translations: [Yu-miya](https://www.twitch.tv/toho_yumiya), CyrusVorazan, DeepL
- Transferring all reports from the [old bugtracker](https://github.com/ack7139/thprac/issues) [here](https://github.com/touhouworldcup/thprac/issues): [toimine](https://www.youtube.com/channel/UCtxu8Rg0Vh8mX6iENenetuA)
- Chinese README.md translations: [TNT569](https://github.com/TNT569) 

## <a name="faq"></a> **Frequently Asked Questions (FAQ)**

## General

### Why did thprac move?
The original developer, Ack, has indefinitely ceased all future development of thprac/Marketeer. Below is their statement:
> I will indefinitely cease all future development on thprac/Marketeer, you are free to continue working on it, granting that you follow the license's terms.  
> Due to my insufficient skill, the code is glutted with obscure writings and terrible logic, turning the whole thing into a complete mess. I'm sorry if that disturbs you.  

Ack cannot be contacted at this time, and development has been taken over by others.

---

### My antivirus says thprac is malware. Is it safe?
Thprac contains no malicious code, but its behavior may trigger antivirus software. If flagged, add an exception or trust rule in your antivirus. Alternatively, try [this version](https://github.com/touhouworldcup/thprac/issues/112) and report back if the issue persists.

---

## Compatibility

### Does thprac work with English patches?
Thprac is compatible with **thcrap** and can function as a thcrap launcher, similar to the [Universal THCRAP Launcher](https://github.com/thpatch/Universal-THCRAP-Launcher/). It does not support static English patches, like those made by gensokyo.org.

---

### thprac cannot find Embodiment of Scarlet Devil. What should I do?
Ensure your executable is named either `東方紅魔郷.exe` or `th06.exe`. If thprac still fails to detect it, you can manually launch the game with any filename through the launcher, which will attach thprac to the game.

---

## Features

### How do I save a replay mid-game in TH06-10?
Saving replays mid-game is not supported due to technical limitations. As per the thprac 2.0.8.3 manual:
> Because of how these games were programmed, adding this feature would be quite troublesome, so currently there's no direct solution to this.  

However, TH06 now supports this functionality, albeit with quirks. Note: replays saved mid-game do not return to the title screen after the save point.

---

### What does "Everlasting BGM" mean?
This option prevents the background music (BGM) from resetting when you restart the game.

---

### What does "Coercive Reporting" mean? (Shoot the Bullet/Double Spoiler)
This feature locks the camera onto the boss at all times and eliminates the camera's recharge time.

---

### How do I switch languages?
- The launcher automatically selects its language based on your Windows settings.  
- To change the language in-game, use the shortcuts:  
  - **ALT + 1**: Language 1  
  - **ALT + 2**: Language 2  
  - **ALT + 3**: Language 3  

These shortcuts do not work in the launcher itself. Refer to the video in the "How to Use" section for detailed instructions.

---

## Bug Reporting

### Where can I report bugs?
Visit the [Issues tab on GitHub](https://github.com/touhouworldcup/thprac/issues) to report bugs or suggest improvements.

---

## Technical Issues

### Unsupported VsyncPatch version in advanced options
Ensure you're using a compatible version of VsyncPatch. You can download it from [here](https://maribelhearn.com/tools#vpatch). Use DLLs from **rev7** whenever possible.

---

### FPS adjustment seems to be broken when using vpatch
Some tools, like "DX8 to DX9 Converter," conflict with VsyncPatch. Replay speed adjustment (slowdown/speedup) is only supported in **TH13**.

---

## Game-Specific

### What is "DDC - Marisa Laser Related"?
This fixes the infamous Marisa laser desync bug in **Double Dealing Character (TH14)**.









