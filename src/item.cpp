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

#include <chrono>
#include <iostream>

#include "item.h"
#include "utils.h"

CItem::CItem(TLevel level, std::string baseType, std::vector<CModGroup*> groups)
: level_(level), baseType_(baseType), groups_(groups) {}

TWeight CItem::totalWeight() {
    TWeight out = 0;
    for (auto m : mods()) {
        out += m->weight;
    }
    return out;
}

TWeight CItem::prefixWeight() {
    TWeight out = 0;
    for (auto m : mods()) {
        if (m->isPrefix()) {
            out += m->weight;
        }
    }
    return out;
}

TWeight CItem::suffixWeight() {
    TWeight out = 0;
    for (auto m : mods()) {
        if (m->isSuffix()) {
            out += m->weight;
        }
    }
    return out;
}

const std::vector<CMod*> CItem::mods() {
    std::vector<CMod*> out;
    for (auto& g : groups_) {
        out.insert(out.end(), g->mods.begin(), g->mods.end());
    }
    return out;
}
const std::vector<CMod*> CItem::prefixMods() {
    std::vector<CMod*> out;
    for (auto m : mods()) {
        if (m->isPrefix()) {
            out.push_back(m);
        }
    }
    return out;
}

const std::vector<CMod*> CItem::suffixMods() {
    std::vector<CMod*> out;
    for (auto m : mods()) {
        if (m->isSuffix()) {
            out.push_back(m);
        }
    }
    return out;
}

void CItem::updateWeight(std::vector<CWeightModifier*> weights) {
    for (auto g : groups_) {
        for (auto w : weights) {
            if (g->matchModTypeTag(w->tag)) {
                g->updateWeight(static_cast<double>(w->weight) / 100, // Weights modifiers for fossil are stored as a percentage in ggpk files :)
                                w->tag);
            }
        }
    }
    // Now remove groups with 0 weight
    // See https://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
    for (auto g = groups_.cbegin(); g != groups_.cend();) {
        if ((*g)->weight() == 0) {
            g = groups_.erase(g);
        } else {
            g++;
        }
    }
}

double CItem::findModPercent(std::string modId) {
    for (auto m : mods()) {
        if (m->id == modId) {
            return static_cast<double>(m->weight) * 100 / totalWeight(); // Don't forget to make it a percentage
        }
    }
    return 0;
}

std::vector<CMod*> CItem::simulateCraft(uint64_t affixNumber) {
    Profiler::start("prologue");

    std::vector<CMod*> out;
    uint64_t prefix = 0;
    uint64_t suffix = 0;
    bool prefixRemoved = false;
    bool suffixRemoved = false; // Avoid unnecessary iteration over groups if we already removed the bad groups
    Profiler::end("prologue");
    Profiler::start("weightUpd");
    // To make this as fast as possible, we'll pick a random index, based on weights (using STD)
    // to mark a group as invalid, we'll simply put the matching weight at 0 (so you can never roll
    // it) Which means that we don't remove anything from any container
    // We are using a small cache to avoid accessing each group + calling to func (weight() and
    // isPrefix())
    if (weightsCache_.size() == 0 || isPrefixCache_.size() == 0) {
        cacheForCraft();
    }
    std::vector<uint64_t> weights = weightsCache_;


    Profiler::end("weightUpd");

    for (uint64_t i = 0; i < affixNumber; i++) {

        Profiler::start("rand");

        size_t index = Utils::discreteDistribution(weights);
        Profiler::end("rand");
        CModGroup* chosenGroup = groups_[index];


        // don't forget to save our mod :D
        CMod* chosenMod = chosenGroup->chooseOne();

        out.push_back(chosenMod);
        // update weights
        weights[index] = 0;
        // Update counts
        if (isPrefixCache_[index]) {
            prefix += 1;
        } else { // Not beeing a prefix == beeing a suffix
            suffix += 1;
        }

        Profiler::start("erasePrefix");
        // No need to remove prefixes if we also have 3 suffix (end of craft)
        if (prefix == 3 && !prefixRemoved && suffix != 3) {
            prefixRemoved = true;
            for (size_t i = 0; i < weights.size(); i++) {
                if (isPrefixCache_[i]) {
                    weights[i] = 0;
                }
            }
        }
        Profiler::end("erasePrefix");
        Profiler::start("eraseSuffix");

        if (suffix == 3 && !suffixRemoved && prefix != 3) {
            suffixRemoved = true;
            for (size_t i = 0; i < weights.size(); i++) {
                if (isPrefixCache_[i]) {
                    weights[i] = 0;
                }
            }
        }
        Profiler::end("eraseSuffix");

    } // for affixNumber

    return out;
}

void CItem::cacheForCraft() {
    emptyCache();
    for (auto g : groups_) {
        weightsCache_.push_back(g->weight());
        isPrefixCache_.push_back(g->isPrefix());
    }
}
