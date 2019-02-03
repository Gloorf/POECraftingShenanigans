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

#include <iostream>

#include <QString>

#include "mod.h"
#include "utils.h"

std::vector<CMod*>
CModManager::findMods(std::vector<TItemTag> tags, TLevel level, bool allowDelveMods, std::vector<TRowId> rowIds) {
    std::vector<CMod*> out;

    auto cacheKey = std::make_tuple(tags, level, allowDelveMods, rowIds);
    if (findModCache_.find(cacheKey) != findModCache_.end()) {
        auto c = findModCache_[cacheKey];
        // Since we store & return pointer, we'll make a copy of object, or else we will progressively corrupt our CMod* collection by modifying them (during updateWeight for exampl)
        for (auto m : findModCache_[cacheKey]) {
            out.push_back(m->clone());
        }
        return out;
    }

    bool restrictRowIds = rowIds.size() != 0;
    // TODO : maybe it's better to have string constants (like "GenerationType") #defined or const-d in some header ?
    for (size_t i = 0; i < rows_.size(); i++) {
        if (restrictRowIds && find(rowIds.begin(), rowIds.end(), i) == rowIds.end()) {
            continue;
        }
        auto row = rows_[i];

        // First let's filter prefixes / suffixes
        EGenerationType generationType
        = static_cast<EGenerationType>(row->get<uint64_t>("GenerationType"));
        if (generationType != EGenerationType::Prefix && generationType != EGenerationType::Suffix) {
            continue;
        }
        // Then let's filter domain
        EDomain domain = static_cast<EDomain>(row->get<uint64_t>("Domain"));
        bool validDomain = domain == EDomain::Item;
        validDomain |= domain == EDomain::Delve && allowDelveMods;
        if (!validDomain) {
            continue;
        }
        // Level filter !
        if (row->get<uint64_t>("Level") > level) {
            continue;
        }
        // Now let's see if we have a matching tag :)
        // We need to find the first mod tag which matches our item tags
        TWeight weight = findWeight(row, tags);
        if (weight != 0) {
            CMod* tmp = new CMod(row->get<std::string>("Id"), row->get<std::string>("Name"),
                                 row->get<std::string>("CorrectGroup"), row->get<std::uint64_t>("Level"),
                                 weight, row->get<std::uint64_t>("ModTypeKey"), generationType);
            out.push_back(tmp);
        }
    }

    findModCache_[cacheKey] = out;
    // Don't forget that on first match, we'll also need to make copy
    out = {};
    for (auto m : findModCache_[cacheKey]) {
        out.push_back(m->clone());
    }
    return out;
}

std::vector<CModGroup*> CModManager::groupMods(std::vector<CMod*> mods) {
    // First we'll group mod by groupName ; this will allow us to easily remove a whole group (and speed up crafting)
    std::map<std::string, std::vector<CMod*>> modsByGroup;
    for (auto m : mods) {
        // Init the map if needed (first time we met a group
        if (modsByGroup.find(m->groupName) == modsByGroup.end()) {
            modsByGroup[m->groupName] = {};
        }
        modsByGroup[m->groupName].push_back(m);
    }
    std::vector<CModGroup*> groups;
    for (auto& pair : modsByGroup) {
        std::string name = pair.first;
        std::vector<CMod*> mods = pair.second;
        uint64_t weight = 0;
        TModTypeKey key = mods[0]->modType()->key;
        bool multipleModTypeKey = false;
        for (auto m : mods) {
            if (m->modType()->key != key) {
                multipleModTypeKey = true;
            }
            weight += m->weight;
        }
        // This is safe trust me
        CModType* modType = mods[0]->modType();
        groups.push_back(new CModGroup(name, weight, mods, modType, multipleModTypeKey));
    }
    return groups;
}


TWeight CModManager::findWeight(CGGPKRowAccessor* row, std::vector<TItemTag> tags) {
    auto modTags = row->get<std::vector<uint64_t>>("SpawnWeight_TagsKeys");
    auto modWeights = row->get<std::vector<uint64_t>>("SpawnWeight_Values");
    for (uint64_t i = 0; i < modTags.size(); i++) {
        auto it = std::find(tags.begin(), tags.end(), modTags[i]);
        if (it != tags.end()) {
            return modWeights[i];
        }
    }
    return 0;
}


CMod* CMod::clone() {
    CMod* out = new CMod(id, name, groupName, level, weight, modTypeKey, generationType);
    out->modType(this->modType_);
    return out;
}

bool CMod::matchModTypeTag(TItemTag tag) {
    if (!modType_) return false;
    return modType_->hasModTypeTag(tag);
}

CModGroup::CModGroup(std::string name, TWeight weight, std::vector<CMod*> mods, CModType* modType, bool multipleModTypeKey)
: name(name), originalWeight_(weight), mods(mods), modType(modType) {
    weightModifier_ = 1;
    if (true || multipleModTypeKey) {
        modType = nullptr;
    }
}

bool CModGroup::matchModTypeTag(TItemTag tag) {

    // Slow fallback
    for (auto m : mods) {
        if (m->matchModTypeTag(tag)) return true;
    }
    return false;
}

void CModGroup::updateWeight(double modifier, TItemTag tag) {
    originalWeight_ = 0;
    for (auto m : mods) {
        if (m->matchModTypeTag(tag)) {
            m->weight *= modifier;
        }
    }
}

TWeight CModGroup::weight() const {
    TWeight out = 0;

    for (auto m : mods) {
        out += m->weight;
    }
    return out;
}


CMod* CModGroup::chooseOne() {
    TWeight usedWeight = weight();
    auto choice = Utils::randomInt(usedWeight);
    uint64_t cumWeight = 0;
    for (auto m : mods) {
        cumWeight += m->weight;
        if (choice <= cumWeight) {
            return m;
        }
    }
    return nullptr;
}
