## [简体中文](/README_CN.md) [日本語](/README_JP.md)

# **thprac**
> thprac is a tool for practicing Touhou shmups.

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/V7V7O03J4)

## [Download](https://github.com/touhouworldcup/thprac/releases/latest) - [Download Beta](https://nightly.link/touhouworldcup/thprac/workflows/main/master/thprac.zip)
> Note that beta versions will only update to the next stable version, not the next beta version.

## **Table of Contents**
* [Downloading](#downloading)
* [Usage](#usage)
* [Compatability](#compatability)
* [Features](#-features)
* [Quick Menu](#quick-menu)
* [Advanced Options](#advanced-options)
* [Game Specific Features](#game-specific-features)
* [Credits](#credits)
* [Building from Source](#building-from-source)
* [FAQ](#faq)

## <a name="downloading"></a> **Downloading**
If you want to download the tool, you can download the [latest release](https://github.com/touhouworldcup/thprac/releases/latest) here and the [latest beta](https://nightly.link/touhouworldcup/thprac/workflows/main/master/thprac.zip) here. Do note that the latest beta will update into the next stable release and NOT the next beta release.

## <a name="usage"></a> **Usage**
You can use this tool in a variety of ways; the main ways to do this are **placing the executable in the same folder as your game**, **running the executable after running your game**, or **using thprac's launcher**.

### **thprac in the Game Folder**
**This method will NOT work with Steam games.** You can run thprac by placing the `thprac.exe` file in the same folder as your game. This method will also automatically detect vpatch. You must ensure that the game is updated to the latest version and follows one of the following naming schemes. This method also works with vpatch.
- thXX.exe (most games)
- 東方紅魔郷.exe (Embodiment of Scarlet Devil)
- alcostg.exe (Uwabami Breakers)

### **thprac after Running the Game**
**This method will work for most methods.** First, run the game in whichever method you want (whether it's thcrap, using vpatch, Steam, etc.) and then run `thprac.exe`. thprac will then detect the running game and attempt to attach itself to it. You may have to return to the menu screen to see thprac take effect.

### **thprac Using the Launcher**
This video explains how to use the launcher.
[![A quick and dirty overview of thprac 2.0](https://i.ytimg.com/vi/sRV4PDQceYo/maxresdefault.jpg)](https://www.youtube.com/watch?v=sRV4PDQceYo)

### Command line
The following command line options are supported:
- `<path to game exe>` The command `thprac.exe <path to game exe>` will run a game and attach thprac without any confirmation prompt. This makes it possible to drag a Touhou game executable onto thprac.exe and it will run with thprac.
- `--attach <pid>` immediately inject thprac into a process with ID `<pid>` without any confirmation prompts.
- `--attach` (without any other flags). The command `thprac.exe --attach` will attach thprac to the first Touhou game process it finds without opening any kind of confirmation prompt.
- `--without-vpatch` Prevent automatically applying vpatch.

Example commands:
```
thprac.exe --attach 1234
thprac.exe --attach
thprac.exe C:\Users\Name\Desktop\Games\Touhou\th17\th17.exe
thprac.exe --without-vpatch C:\Users\Name\Desktop\Games\Touhou\th17\th17.exe
```

## <a name="compatability"></a> **Compatability**
thprac officially supports every version of Windows starting with **Windows Vista**. Windows XP may work using [One-Core-API](https://github.com/Skulltrail192/One-Core-API-Binaries), but this is **NOT** being actively tested.

thprac is also compatible with **Wine** and the Steam Deck. Do note that the Quick Menu is tailored to work well with the Steam Deck, although this is untested.

## <a name="features"></a> **Features**

thprac features an enhanced practice mode that supports all main games along with Great Fairy Wars and Uwabami Breakers.

![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174433923-0a6069e7-d10d-4107-8f0d-f4a8a9d56976.png)

![Imperishable Night](https://user-images.githubusercontent.com/23106652/174433975-8f23b0b0-e48e-4be1-8cb7-d8e3e7ab6b8e.png)

thprac replaces the original practice menu with a new UI shown in the image above; certain spells allow you to select phases or change certain parameters. For the games without an original practice mode (Uwabami Breakers, Great Fairy War, etc.), a practice menu is added to the "Start Game" menu.

![Uwabami Breakers](https://user-images.githubusercontent.com/23106652/174434103-5fee7a13-0254-4602-a468-42330b985bb2.png)
![Great Fairy Wars](https://user-images.githubusercontent.com/23106652/174434121-063142f2-ef3d-4721-ab96-a252343cdb0e.png)

This menu can be controlled using your keyboard or your controller. Using this, you can select options using the **UP** and **DOWN** keys and change the option using the **LEFT** and **RIGHT** keys and the **SHOT** key to select. You can also use your mouse to interact with the menu.

thprac will also automatically save the options you selected into any replay saved. When playing back the replay, thprac will automatically apply said options. **Replays saved while thprac is active using "Custom" mode will NOT work with the unmodified game.**

## <a name="quick-menu"></a> **Quick Menu**
In any supported game **(except Touhou 9 and 19)**, you can press the **`Backspace`** key to enter the Quick Menu. This menu shows a variety of options that can be activated using the function (F) keys.

These options are always clickable, meaning you can interact with them using a mouse on a desktop computer or by tapping on them with your left thumb when using a Steam Deck. To use this feature on the Steam Deck, map the **`Backspace`** key in Steam Input.

The hotkey for this menu can be changed from the launcher's settings tab to a different key or combination of keys if you want it bound to something else.

![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174434813-73748a66-0f6d-4c6e-9f3a-895a49b93434.png)
![Wily Beast and Weakest Creature](https://user-images.githubusercontent.com/23106652/174434834-6bd93104-1ed2-48ae-a440-9d9cb871ea03.png)

## <a name="advanced-options"></a> **Advanced Options**
In any supported game **(except Touhou 9 and 19)**, you may use the **`F12`** key to access the Advanced Options; these options may offer fixes or patches related to the game or other quality of life features.

Like the Quick Menu, the hotkey for this can also be changed from the launcher's settings tab.

![Imperishable Night](https://user-images.githubusercontent.com/23106652/174434977-683da583-324b-4bd5-8408-13373dfd5a93.png)
![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174435006-e906d30d-0ef5-4930-ae57-1f0919beb5af.png)

## <a name="game-specific-features"></a> **Game-specific Features**
### 100th BM - Force Wave
![100th Black Market](https://github.com/touhouworldcup/thprac/assets/23106652/02c55e5e-5c89-462f-beea-9ab07cbb1051)

If thprac is enabled using "Custom" mode, the popup above will appear when the game is about to choose a wave.

### PoFV - Tools
![Phantasmagoria of Flower View](https://user-images.githubusercontent.com/23106652/174434249-2bf1d70a-101c-4538-a4e6-8eeaf273dd88.png) 

When starting a game in Match Mode, you can select "Custom" in a "Mode" prompt. If you do that, this window will appear in your game. You can move and adjust this window as much as you want. This replaces Advanced Options and can be opened with **`F12`**. 

### UDoALG - Tools
![TH19 Tools](https://github.com/touhouworldcup/thprac/assets/23106652/a2cdb385-b61d-4111-af6b-b195e85bf18a)

It's just like PoFV Tools, but for UDoALG.

### EoSD - Pause Menu
![TH06](https://user-images.githubusercontent.com/23106652/174436027-734d642a-300c-45ab-9591-b6219aca087b.png)

Choosing "Exit" will now prompt you to save a replay. 

**Warning**: replays saved like this will continue after the point you chose to save the replay. The player character will stand still and not shoot.

### ESC + R fix for EoSD, VD and beyond

## <a name="credits"></a> **Credits (post May 27th, 2022)**
- Development: [32th System](https://www.youtube.com/channel/UChyVpooBi31k3xPbWYsoq3w), [muter3000](https://github.com/muter3000), [zero318](https://github.com/zero318), [Lmocinemod](https://github.com/Lmocinemod), [Cao Minh](https://github.com/hoangcaominh), [raviddog](https://github.com/raviddog) 
- Chinese translations: [CrestedPeak9](https://twitter.com/CrestedPeak9), maksim71_doll, DeepL
- Japanese translations: [Yu-miya](https://www.twitch.tv/toho_yumiya), CyrusVorazan, DeepL
- Moving all reports from the [old bugtracker](https://github.com/ack7139/thprac/issues) [here](https://github.com/touhouworldcup/thprac/issues): [toimine](https://www.youtube.com/channel/UCtxu8Rg0Vh8mX6iENenetuA)
- English README.md: [Galagyy](https://github.com/Galagyy)
- Chinese README.md translation: [TNT569](https://github.com/TNT569), [H-J-Granger](https://github.com/H-J-Granger)
- Japanese README.md translation: [wefma](https://github.com/wefma)

## <a name="building-from-source"></a> **Building from Source**
### First time setup (only needs to be performed once)
Inside the folder named thprac, compile loc_json.cpp into loc_json.exe in whatever way you like most. I recommend this command
```
cl /Isrc\3rdparties\yyjson /nologo /EHsc /O2 /std:c++20 loc_json.cpp .\src\3rdParties\yyjson\yyjson.c /Fe:loc_json.exe
```

### Building on the command line
In a Visual Studio Developer Command Prompt, run the following command
```
msbuild thprac.sln -t:restore,build -p:RestorePackagesConfig=true,Configuration=Release
```

### Building from the Visual Studio GUI
Open `thprac.sln` click on "Build", then "Build solution"

## <a name="faq"></a> **Frequently Asked Questions (FAQ)**

### General

#### Why did thprac move?
The original developer, Ack, has indefinitely ceased all future development of thprac/Marketeer. Below is their statement:
> I will indefinitely cease all future development on thprac/Marketeer, you are free to continue working on it, granting that you follow the license's terms.  
> Due to my insufficient skill, the code is glutted with obscure writings and terrible logic, turning the whole thing into a complete mess. I'm sorry if that disturbs you.  

Ack cannot be contacted at this time, and development has been taken over by others.

#### My antivirus says thprac is malware. Is it safe?
Thprac contains no malicious code, but its behavior may trigger your antivirus. If flagged, add an exception or trust rule in your antivirus to allow thprac to work. Alternatively, you can try [this version](https://github.com/touhouworldcup/thprac/issues/112) and report back if the issue persists.

---

### Compatibility

#### Does thprac work with English patches?
Thprac is compatible with **thcrap** and can function as a thcrap launcher, similar to the [Universal THCRAP Launcher](https://github.com/thpatch/Universal-THCRAP-Launcher/). It does not support static English patches, like those created by gensokyo.org.

---

#### thprac cannot find Embodiment of Scarlet Devil. What should I do?

Ensure your executable is named either `東方紅魔郷.exe` or `th06.exe`. If thprac still fails to detect it, you can manually launch the game with any filename through the launcher, which will attach thprac to the game.

---

### Features

#### How do I save a replay mid-game in TH06-10?
Saving replays mid-game is not supported due to technical limitations. As per the thprac 2.0.8.3 manual:
> Because of how these games were programmed, adding this feature would be quite troublesome, so currently there's no direct solution to this.  

However, EoSD now supports this functionality.

Note: replays saved mid-game do not return to the title screen after the save point.

---

#### What does "Everlasting BGM" mean?
This option prevents the background music (BGM) from resetting when you restart the game.

---

#### What does "Coercive Reporting" mean? (Shoot the Bullet/Double Spoiler)
This feature locks the camera onto the boss at all times and eliminates the camera's recharge time.

---

#### How do I switch languages?
- The launcher automatically selects its language based on your Windows settings.  
- To change the language in-game, use the shortcuts:  
  - **`ALT + 1`**: Japanese 
  - **`ALT + 2`**: Chinese 
  - **`ALT + 3`**: English 

The hotkey for changing languages can be changed from the launcher's settings tab.

These shortcuts do not work in the launcher itself. Refer to the video in the **"How to Use"** section for detailed instructions.

---

### Bug Reporting

#### Where can I report bugs?
Visit the [Issues tab on GitHub](https://github.com/touhouworldcup/thprac/issues) to report bugs or suggest improvements.

---

### Technical Issues

#### Unsupported VsyncPatch version in advanced options
Ensure you're using a compatible version of VsyncPatch. You can download it from [here](https://maribelhearn.com/tools#vpatch). Use DLLs from **rev7** whenever possible.

---

#### FPS adjustment seems to be broken when using vpatch

Some tools, like "DX8 to DX9 Converter," conflict with VsyncPatch. Replay speed adjustment (slowdown/speedup) is only supported in **TH13**.

---

### Game-Specific

#### What is "DDC - Marisa Laser Related"?
This fixes the infamous Marisa laser desync bug in **Double Dealing Character (TH14)**. Please refer to the original developer, Ack's [demonstration video on Bilibili](https://www.bilibili.com/video/av285566068) (in Chinese, [backup video link on YouTube](https://www.youtube.com/watch?v=Hkh_AEGHLto))
