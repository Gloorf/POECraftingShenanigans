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
#include <vector>

#include "constants.h"
#include "ggpk.h"

class CModType {
    public:
    CModType(std::string name, TModTypeKey key, std::vector<TModTypeTag> tags)
    : name(name), key(key), tags_(tags) {}
    bool hasModTypeTag(TItemTag tag) {
        return std::find(tags_.begin(), tags_.end(), tag) != tags_.end();
    }
    std::string name;
    TModTypeKey key;

    private:
    std::vector<TModTypeTag> tags_;
};

class CModTypeManager : public CGGPKDatFile {
    public:
    CModType* find(TModTypeKey key); // This function is memoized for performance
    void fillCache();

    private:
    std::vector<CModType*> findCache_;
};
