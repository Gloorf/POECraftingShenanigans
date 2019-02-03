## Presentation ##

This project aims to simulate all kinds of crafting in PoE.

Currently, it can :

* Show all the possible mods on any item, elder/shaper or not, with the weight (probability to roll)
* Show the effect of any fossil combination : the above, but with fossils
* Find the best fossil combination for a single mod : what gives the mod the more %chance to spawn
* Simulate as much craft as possible (won't recommend doing more than 1M), and observe two mods : how many times they each spawned, and how many time they spawned together. Obviously you can use fossil on those
* Export the craft to a JSON file if you want to analyze it further.

What it can't do :

* Be pretty. Just look at the design. (Seriously, do it)
* Essence craft. If someone wants to give some raw data (i.e spam 4k essence on an item), feel free to do it and I'll implement it !
* Sanctified fossil. Same as essence : without raw data, I prefer *not* to implement it rather than badly implementing it
* Bloodstained / Glyphic / Hollow. I need to find a good way to implement "100% sure" mods.
* Be pretty. It's so strange it's almost a piece of art (almost.)
* Be more usable around mods (giving the rolls + stat, like "+11-15 to Intelligence" instead of Intelligence3
* Observe more than 2 mods in "simulate craft". I'll add it later
* Jewel crafting. For some reason the mods are strange, and I haven't figured it yet


# Usage #

It should be fairly easy : select an item base, an item level, if it's an elder/shaper base, and the fossil you want.

If you want to optimise (or simulate while observing) a delve/elder mod, just add the appropriate fossil / change the item type, and it will magically show in the dropdown

## Protip ##

If you hover a mod Id in display tab, you'll have the affix name (like 'of Smothering'). If you hover a weight in display tab, you'll have the %chance to roll (both across all mods, and only prefix/suffix)

## Where does the data comes from ##

Various origin :
* `data/ggpk` comes from Content.ggpk, extracted with pypoe. I pull all the weight data, and fossil crafting modifiers from here
* `data/wiki_export` comes from the wiki exporter of pypoe, so ultimately from Content.ggpk; I use it (with the script `analyze_wiki_export.py` to generate a `SimpleBaseItemTypes.json` hich contains all base items type (duh) with their tags, and shaper/elder tag if relevant.
* `data/ironring_level3` comes from me spamming 4k chaos on a poor level 3 iron ring. I used an ahk script `ctrlc_save.akh` to save automatically the result of a ctrl-c to a file (I'm not completely masochist). It's used to find the repartition of affix number (how many items are 4-mods, 5-mods, 6-mods). the data is analyzed with the script `analyze_ironring.py` which puts the output in `IronRingData.json`


## Building your own ##

This project use qt5, with no additional libraries. Features from c++17 are used, so you'll need a fairly recent compiler. It should be relativly straightforward to do, and compile with clang, gcc, mingw & MSVC2017 (other compiler not tested).
