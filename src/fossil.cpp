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

#include "constants.h"
#include "fossil.h"

// UGLY
const std::vector<std::string> CFossilManager::allNames_
= { "Scorched", "Frigid",    "Metallic",   "Jagged",   "Aberrant",     "Pristine",   "Dense",
    "Corroded", "Prismatic", "Aetheric",   "Serrated", "Lucent",       "Shuddering", "Bound",
    "Perfect",  "Enchanted", "Encrusted",  "Faceted",  "Bloodstained", "Hollow",     "Fractured",
    "Glyphic",  "Tangled",   "Sanctified", "Gilded" };

CFossil* CFossilManager::find(std::string name) {
    TRowId rowId = rowIdFromName(name);
    CGGPKRowAccessor* row = rows_[rowId];
    std::vector<TRowId> additionalModRowIds = row->get<std::vector<uint64_t>>("Mods_Keys0");
    std::vector<CWeightModifier*> weights;
    // First negative weights ...
    auto negativeWeightsKeys = row->get<std::vector<uint64_t>>("Unknown_TagsKeys");
    auto negativeWeightsValues = row->get<std::vector<uint64_t>>("Unknown_Values");
    for (size_t i = 0; i < negativeWeightsKeys.size(); i++) {
        weights.push_back(new CWeightModifier(negativeWeightsKeys[i], negativeWeightsValues[i]));
    }
    // Then positive weights
    auto positiveWeightsKeys = row->get<std::vector<uint64_t>>("Weight_TagsKeys");
    auto positiveWeightsValues = row->get<std::vector<uint64_t>>("Weight_Values");
    for (size_t i = 0; i < positiveWeightsKeys.size(); i++) {
        weights.push_back(new CWeightModifier(positiveWeightsKeys[i], positiveWeightsValues[i]));
    }

    return new CFossil(name, additionalModRowIds, weights);
}

TRowId CFossilManager::rowIdFromName(std::string name) {
    for (size_t i = 0; i < allNames_.size(); i++) {
        if (allNames_[i] == name) {
            return i;
        }
    }
    // TODO real error handling
    return 0;
}
