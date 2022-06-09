# thprac
A tool for practicing Touhou Shmups
## [Download](https://github.com/touhouworldcup/thprac/releases/latest)

# Credits (May 27th 2022 and after)
- [32th System](https://www.youtube.com/channel/UChyVpooBi31k3xPbWYsoq3w), [Cao Minh](https://github.com/hoangcaominh), [raviddog](https://github.com/raviddog): Development
- [CrestedPeak9](https://twitter.com/CrestedPeak9): Chinese translations

# How to build:
A Visual Studio solution is bundled that uses the Visual Studio 2022 build tools (v143) for debug builds and the Visual Studio 2017 build tools with Windows XP support (v141_xp) for release builds.

If you add a new string to the codebase that needs to be localized, put it into thprac_games_def.json and run thprac_multi to generate a new thprac_locale_def.h

# Why did thprac move?
Ack has completely stopped thprac development and can not be contacted at the moment. This is the statement on his original repository
> I will indefinitely cease all future development on thprac/Marketeer, you are free to continue working on it, granting that you follow the license's terms.
> Due to my insufficient skill, the code is glutted with obscure writings and terrible logic, turning the whole thing into a complete mess. I'm sorry if that disturbs you.
