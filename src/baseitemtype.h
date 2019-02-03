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

#include "item.h"

class CBaseItemType {
    public:
    CBaseItemType(std::string name, std::string class_id, std::vector<TItemTag> tags, TItemTag shaper_tag, TItemTag elder_tag)
    : name_(name), class_id_(class_id), tags_(tags), shaper_tag_(shaper_tag), elder_tag_(elder_tag) {}
    std::string name() { return name_; }
    std::vector<TItemTag> tags() { return tags_; }

    TItemTag shaperTag() { return shaper_tag_; }
    TItemTag elderTag() { return elder_tag_; }

    bool hasShaper() { return shaper_tag_ != 0; }
    bool hasElder() { return elder_tag_ != 0; }

    private:
    std::string name_;
    std::string class_id_; // Probably better as an enum ...
    std::vector<TItemTag> tags_;
    TItemTag shaper_tag_;
    TItemTag elder_tag_;
};

class CBaseItemTypeManager {
    public:
    CBaseItemTypeManager();
    void addBaseItem(std::string name, std::string class_id, std::vector<TItemTag> tags, TItemTag shaper_tag, TItemTag elder_tag);
    std::vector<std::string> baseItemNames(); // Used to populate the item name combobox
    void sortItems();
    std::vector<TItemTag> itemTags(std::string name, EItemSpecial special);

    private:
    std::vector<CBaseItemType*> baseItemTypes_;
};
