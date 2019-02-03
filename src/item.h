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
#include <cstdint>
#include <string>
#include <vector>

#include "constants.h"
#include "fossil.h"
#include "mod.h"
#include "modtype.h"

class CItem {
    public:
    CItem(TLevel level, std::string baseType, std::vector<CModGroup*> groups);

    TWeight totalWeight();
    TWeight prefixWeight();
    TWeight suffixWeight();
    const std::vector<CMod*> mods();
    const std::vector<CMod*> prefixMods();
    const std::vector<CMod*> suffixMods();
    void updateWeight(std::vector<CWeightModifier*> weights);

    double findModPercent(std::string modId);
    std::vector<CMod*> simulateCraft(uint64_t affixNumber); // That's right !

    void cacheForCraft();
    void emptyCache() {
        weightsCache_ = {};
        isPrefixCache_ = {};
    }

    private:
    TLevel level_;
    std::string baseType_;
    std::vector<CModGroup*> groups_;
    std::vector<TWeight> weightsCache_;
    std::vector<bool> isPrefixCache_;
};
