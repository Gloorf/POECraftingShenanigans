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
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// Typedef for clarity
typedef uint64_t TLevel;      // An item level
typedef uint64_t TWeight;     // A weight of a mod
typedef uint64_t TItemTag;    // An item tag(s), used to find the mods who will apply to the item
typedef uint64_t TModTypeTag; // The tag(s) a mod type will apply too
typedef uint64_t TModTypeKey; // Used to identify a Mod Type (there is no ID)

typedef uint64_t TRowId;

typedef std::variant<uint64_t, std::string, std::vector<uint64_t>, bool> TGGPKCell; // a cell value in a GGPK row
typedef std::optional<TGGPKCell> TJsonNullableType; // same as TGGPKCell, but with a possibility of null value, because it comes from Json

typedef std::string TGGPKHeader;                           // a key for an header if a GGPK file
typedef std::map<std::string, TJsonNullableType> TGGPKRow; // A single row in a GGPK file

typedef std::vector<uint64_t> TFossilCombination;

enum class EItemSpecial { Normal, Shaper, Elder };
// See http://omegak2.net/poe/PyPoE/_autosummary/PyPoE.poe.constants.html#module-PyPoE.poe.constants
// For all constant origin
// Note : padding with Unknown = 0 because those value start at 1 in ggpk files
enum class EGenerationType { UNKNOWN, Prefix, Suffix };
enum class EDomain {
    UNKNOWN,
    Item,
    Flask,
    Monster,
    Chest,
    Area,
    Unknown1,
    Unknown2,
    Unknown3,
    Master,
    Crafted,
    Misc,
    Atlas,
    Leaguestone,
    AbyssJewel,
    MapDevice,
    Delve,
    DelveArea
};
