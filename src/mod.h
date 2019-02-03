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
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <QJsonArray>

#include "ggpk.h"
#include "modtype.h"

/*
 * A trimmed version of a mod in Mods.json : holds only the useful values for an item mod, with the
 * assurance they are real values (and not null values)
 * */
class CMod {
    public:
    CMod(std::string id, std::string name, std::string groupName, TLevel level, TWeight weight, TModTypeTag modTypeKey, EGenerationType generationType)
    : id(id), name(name), groupName(groupName), level(level), weight(weight),
      modTypeKey(modTypeKey), generationType(generationType) {}
    CMod* clone();
    bool isPrefix() { return generationType == EGenerationType::Prefix; }
    bool isSuffix() { return generationType == EGenerationType::Suffix; }
    bool matchModTypeTag(TItemTag tag);
    void modType(CModType* modType) { modType_ = modType; }
    CModType* modType() { return modType_; }
    std::string id; // The internal ID, e.g "LocalIncreasedPhysicalDamagePercentAndAccuracyRating1"
    std::string name; // In english, e.g "Squire's"

    std::string groupName; // Maybe better as a custom type ?
    TLevel level;
    TWeight weight;
    TModTypeKey modTypeKey; // Used to find the correct modType
    EGenerationType generationType;

    private:
    CModType* modType_;
};

class CModGroup {
    public:
    CModGroup() {}
    CModGroup(std::string name, std::vector<CMod*> mods);

    bool matchModTypeTag(TItemTag tag); // return true if the tag match any of the group modtype
    void updateWeight(double modifier, TItemTag tag);

    TWeight weight() const;
    // Those two functions are actually wrong, because at least one group (DamageTakenBeforeLife) is spread on prefix + suffixes
    // But i'm too lazy to think about a good fix for this (it's too early in the morning), and it's pretty fucking rare
    bool isPrefix() const { return mods.size() > 1 && mods[0]->isPrefix(); }
    bool isSuffix() const { return mods.size() > 1 && mods[0]->isSuffix(); }

    std::string name;
    CMod* chooseOne();
    std::vector<CMod*> mods;

    private:
};

class CModManager : public CGGPKDatFile {
    typedef std::tuple<std::vector<TItemTag>, TLevel, bool, std::vector<TRowId>> TFindModCacheKey;
    typedef std::vector<CMod*> TFindModResult;

    public:
    // This function is memoized (time critical in fossil optimisation)
    std::vector<CMod*> findMods(std::vector<TItemTag> tags,
                                TLevel level,
                                bool allowDelveMods,
                                std::vector<TRowId> rowIds); // Return all mods for tags, restricted by level (used in base display)
    std::vector<CModGroup*> groupMods(std::vector<CMod*> mods);

    private:
    TWeight findWeight(CGGPKRowAccessor* row, std::vector<TItemTag> tags); // Return 0 if not found
    std::map<TFindModCacheKey, TFindModResult> findModCache_;
};
