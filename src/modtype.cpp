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

#include "modtype.h"

CModType* CModTypeManager::find(TModTypeKey key) {
    if (findCache_.size() < key) {
        fillCache();
    }
    return findCache_[key];
}

void CModTypeManager::fillCache() {
    findCache_ = {};
    for (size_t i = 0; i < rows_.size(); i++) {
        findCache_.push_back(new CModType(rows_[i]->get<std::string>("Name"), i,
                                          rows_[i]->get<std::vector<uint64_t>>("TagsKeys")));
    }
}
