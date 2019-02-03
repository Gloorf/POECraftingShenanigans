/*
Copyright (C) 2019 Guillaume DUPUY <glorf@glorf.fr>
This file is part of Poe Crafting Shenanigans.

Poe Crafting Shenanigans is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Poe Crafting Shenanigans is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Poe Crafting Shenanigans.  If not, see <http://www.gnu.org/licenses/>
*/

#pragma once

#include <memory>


#include "baseitemtype.h"
#include "fossil.h"
#include "item.h"
#include "mod.h"
#include "modtype.h"
class COptimiseResult {
    public:
    COptimiseResult(double weightPercent, std::vector<std::string> fossilNames)
    : weightPercent(weightPercent), fossilNames(fossilNames) {}
    double weightPercent;
    std::vector<std::string> fossilNames;
};

class CSimulateResult {
    public:
    CSimulateResult(uint64_t mod1, uint64_t mod2, uint64_t mod1And2, std::vector<std::vector<std::string>> allModsIds)
    : mod1(mod1), mod2(mod2), mod1And2(mod1And2), allModIds(allModsIds) {}
    uint64_t mod1;
    uint64_t mod2;
    uint64_t mod1And2;
    void exportJson(std::string filename);

    std::vector<std::vector<std::string>> allModIds;
};

class CAffixData {
    public:
    CAffixData(uint64_t fourModCount, uint64_t fiveModCount, uint64_t sixModCount)
    : fourModCount(fourModCount), fiveModCount(fiveModCount), sixModCount(sixModCount) {}
    uint64_t fourModCount;
    uint64_t fiveModCount;
    uint64_t sixModCount;
};

class CApplication {

    public:
    CApplication();
    CItem* createItem(TLevel level, std::string baseType, EItemSpecial special, std::vector<std::string> fossilNames); // used by main window
    CItem* createItemProfiling(std::map<std::string, uint64_t>& profile,
                               TLevel level,
                               std::string baseType,
                               EItemSpecial special,
                               std::vector<std::string> fossilNames); // used by main window


    std::vector<std::string> baseItemNames() {
        return baseItemTypeManager_->baseItemNames();
    } // Used to populate the item name combobox
    std::vector<std::string> fossilNames() { return fossilManager_->fossilNames(); }
    bool rareFossil(std::string name) { return fossilManager_->isRare(name); }

    std::vector<COptimiseResult*> optimiseSingleMod(TLevel level,
                                                    std::string baseType,
                                                    EItemSpecial special,
                                                    std::string modId,
                                                    std::vector<std::string> possibleFossils,
                                                    uint64_t maxFossils);
    CSimulateResult* simulateCraft(CItem* item, uint64_t craftNumber, std::string mod1, std::string mod2);

    private:
    std::unique_ptr<CModManager> modManager_;
    std::unique_ptr<CBaseItemTypeManager> baseItemTypeManager_;
    std::unique_ptr<CModTypeManager> modTypeManager_;
    std::unique_ptr<CFossilManager> fossilManager_;

    std::unique_ptr<CAffixData> affixData_;
    bool readJsonFile(std::string filename, CGGPKDatFile* dest);
    // These two are just a wrapper around readJsonFile
    bool readMods();
    bool readModTypes();
    bool readFossils();

    // This is different because BaseItemTypes.json is not directly extracted from json but built differently (currently, filtering wiki export + merging with Tags.json)
    bool readBaseItemTypes();
    // This is different because it's pretty easy
    bool readIronRing();
};
