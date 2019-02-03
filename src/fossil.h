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

#include <string>

#include <QJsonArray>

#include "constants.h"
#include "ggpk.h"

class CWeightModifier {
    public:
    CWeightModifier(TModTypeTag tag, TWeight weight) : tag(tag), weight(weight) {}
    TModTypeTag tag;
    TWeight weight;
};

class CFossil {
    public:
    CFossil(std::string name, std::vector<TRowId> additionalModsRowIds, std::vector<CWeightModifier*> weights)
    : additionalModsRowIds(additionalModsRowIds), weights(weights), name_(name) {}
    std::vector<TRowId> additionalModsRowIds;
    std::vector<CWeightModifier*> weights;

    private:
    std::string name_;
};

class CFossilManager : public CGGPKDatFile {
    public:
    // Since I hardcoded the fossil name, we'll do a bit differently and use them as identifiers to find fossils
    CFossil* find(std::string name);

    const std::vector<std::string> fossilNames() {
        return { "Scorched",     "Frigid", "Metallic",   "Jagged",    "Aberrant",
                 "Pristine",     "Dense",  "Corroded",   "Prismatic", "Aetheric",
                 "Serrated",     "Lucent", "Shuddering", "Bound",     "Faceted",
                 "Bloodstained", "Hollow", "Fractured",  "Glyphic",   "Tangled" };
    }
    bool isRare(std::string name) {
        return name == "Faceted" || name == "Bloodstained" || name == "Hollow"
               || name == "Fractured" || name == "Glyphic" || name == "Tangled";
    }


    private:
    const static std::vector<std::string> allNames_;
    static TRowId rowIdFromName(std::string name);
};
