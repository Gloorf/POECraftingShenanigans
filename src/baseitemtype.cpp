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
#include <algorithm>

#include "baseitemtype.h"

CBaseItemTypeManager::CBaseItemTypeManager() {}

void CBaseItemTypeManager::addBaseItem(std::string name,
                                       std::string class_id,
                                       std::vector<TItemTag> tags,
                                       TItemTag shaper_tag,
                                       TItemTag elder_tag) {
    baseItemTypes_.push_back(new CBaseItemType(name, class_id, tags, shaper_tag, elder_tag));
}

std::vector<std::string> CBaseItemTypeManager::baseItemNames() {
    std::vector<std::string> out;
    for (auto item : baseItemTypes_) {
        out.push_back(item->name());
    }
    return out;
}

void CBaseItemTypeManager::sortItems() {
    std::sort(baseItemTypes_.begin(), baseItemTypes_.end(),
              [](const auto& lhs, const auto& rhs) { return lhs->name() < rhs->name(); });
}

std::vector<TItemTag> CBaseItemTypeManager::itemTags(std::string name, EItemSpecial special) {
    for (auto item : baseItemTypes_) {
        if (item->name() == name) {
            std::vector<TItemTag> out = item->tags();
            if (special == EItemSpecial::Elder && item->hasElder()) {
                out.push_back(item->elderTag());
            }
            if (special == EItemSpecial::Shaper && item->hasShaper()) {
                out.push_back(item->shaperTag());
            }
            return out;
        }
    }
    return {};
}
