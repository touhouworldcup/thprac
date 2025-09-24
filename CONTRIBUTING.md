## Welcome!
If you have been sending a lot of pull requests, congratulations, you might have been added as a collaborator to this project. Before proceeding though, please read through these...

# Contribution guidelines

## 1. Commits on the `master` branch must not compromise the competitive integrity of [Touhou World Cup](https://touhouworldcup.com/).

According to Section "6.1.2. Allowed Game Modifications" in the [official handbook](https://docs.google.com/document/d/e/2PACX-1vQrmgcwhgKARoUnk5BPE0Oyv4fAgHilZs1pUa1RQJtm0X_z93L8eI0lNt1Y-iZQK3v4_Ab9vx1HzpkN/pub#h.9j7h74u3d3zf) all GitHub Actions builds created from a commit on the `master` branch** are allowed.

What this means in detail is that players are allowed to keep thprac injected into their game while attempting a credit, as long as all features from the backspace menu are disabled. This is because thprac gets out of the way and does not allow for any of its features to activate during a full game run.

To clarify: players are allowed to show themselves use other mods during TWC, but they must restart restart their games before attempting credits, otherwise these credits are not counted. If a player shows themselves using thprac on the other hand, they do **NOT** need to restart the game for credits to be considered valid.

To further clarify: This does not mean that all commits on the `master` branch must be perfectly stable. If a bug happens, that's OK.

As a result: the following rule emerges

## 2. Any feature that works during a full game run MUST either cause any replay saved from that run to desync, or the replay must have some other indicator to show that a thprac feature was used.

For example, enabling Market Manipulation in TH18 in the shop that shows up at the end of stages allows the player to give themselves any card. These cards then show up in the next stage both while the run is being played and in the final replay. This allows players to cheat in cards without causing a replay desync. To mitigate this, giving yourself any card causes the Blue Magatama to get added as well. A card that is otherwise impossible to obtain and therefore very clearly indicates that the player used Market Manipulation.

### 2.1. Exceptions

There are however exceptions. These are
- Removing the score limit
- Allowing a score beyond the maximum to be saved into a replay
- Proacively preventing replays from desyncing

## 3. Preferrably use other branches for developing big features (and always when working on support for a new game)
Other branches are not affected by the rules set in 1. and 2. Feel free to do whatever you want there.

## 4. Rebases and force pushes
In order to ensure that all commits on the `master` branch retain competitive integrity, a member of the TWC Committee may rebase, then force push to the master branch. If a force push happens, do the following

```
git checkout master
git fetch
git rebase --hard origin/master
```

This will bring your local commits back up to date with GitHub.

Note that not all committee members are in the `touhouworldcup` GitHub organization. The ones that are, are in a @Committee team. Only members of this team are given the ability to force push to the master branch.

## 5. Code style and guidelines

snake_case vs camelCase vs PascalCase: there is no consistency regarding which one is used when. If you are decising between the three, either try to replicate the surrounding code, or go with what you most naturally gravitate towards.

The same applies to putting the opening curly brace on the same line or on a new line. Your automated formatter of choice may gravitate to one or the other, but this project does both.

Avoid nesting too deeply. Unless you need deep nesting of `if` blocks to ensure optimal code generation from MSVC, prefer early returns and using the defer macro. Note that you might find parts of thprac's code that is very deeply nested and either doesn't need optimal codegen, or just doesn't achieve it.

Memory addresses: Parts of thprac's code still use raw integer literals in places where memory addresses are needed. Avoid doing that. Instead, there should be an enum named `addrs` at the top of the file somewhere. Define a name for your memory address up there and then stick with using that name. If the thprac_thXX.cpp file that you're working in doesn't have this enum, create it.

Using raw integer literals offsets is fine, but if you catch yourself using a lot of different offsets from the same address, it might make more sense to declare a struct.

Add `[skip ci]` to commit descriptions of any commits that do not modify any C++ code.