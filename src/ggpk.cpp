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
#include <string>

#include "ggpk.h"


TJsonNullableType CGGPKColumn::value(QJsonValue value) {
    // This is pretty ugly but it works for now (yay !)
    TJsonNullableType out;
    if (value.isNull()) {
        return {};
    }

    if (type_ == "ref|string") {
        if (!value.isString()) {
            std::cerr << "Invalid value : expected string, got " << value.type() << std::endl;
        }
        out = value.toString().toStdString();
    }
    if (type_ == "int" || type_ == "ulong") {
        if (!value.isDouble()) {
            std::cerr << "Invalid value : expected double, got " << value.type() << std::endl;
        }
        out = static_cast<uint64_t>(value.toInt());
    }
    if (type_ == "ref|list|ulong" || type_ == "ref|list|uint" || type_ == "ref|list|int") {
        if (!value.isArray()) {
            std::cerr << "Invalid value : expected array, got " << value.type() << std::endl;
        }
        std::vector<uint64_t> vec; // Ugly cause repeat of type, should find out how to take it from std::variant maybe ?

        for (auto val : value.toArray()) {
            if (!val.isDouble()) {
                std::cerr << "Invalid value : expected double in array, got " << value.type() << std::endl;
                break;
            }
            vec.push_back(static_cast<uint64_t>(val.toInt()));
        }
        out = vec;
    }
    if (type_ == "bool") {
        if (!value.isBool()) {
            std::cerr << "Invalid value : expected bool, got " << value.type() << std::endl;
        }
        out = value.toBool();
    }
    return out;
}

void CGGPKRowAccessor::display() {
    for (auto kv : attributes_) {
        std::cout << kv.first << " : ";
        if (kv.second.has_value()) {
            // Modern C++ can be a wild ride !
            // This is not very beautiful, but it's used for debug, so ...
            std::visit(
            [](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::vector<uint64_t>>) {
                    for (auto v : arg) {
                        std::cout << v << ",";
                    }
                } else {
                    std::cout << arg;
                }
            },
            kv.second.value());
        } else {
            std::cout << "NULL";
        }
        std::cout << std::endl;
    }
}
NullAttributeAccess::NullAttributeAccess(const std::string& name) {
    message_ = "Tried to access '" + name + "' which is null or doesn't exist";
}

const char* NullAttributeAccess::what() const noexcept { return message_.c_str(); }

void CGGPKDatFile::addColumn(TGGPKHeader name, std::string type) {
    columns_.push_back(new CGGPKColumn(name, type));
}
void CGGPKDatFile::addRow(QJsonArray data, TRowId rowId) {
    TGGPKRow attributes;
    for (size_t i = 0; i < static_cast<size_t>(data.size()); i++) {
        CGGPKColumn* header = columns_[static_cast<size_t>(i)];
        QJsonValue value = data[static_cast<int>(i)];
        attributes[header->name] = header->value(value);
    }
    CGGPKRowAccessor* newRow = new CGGPKRowAccessor(attributes, rowId);
    rows_.push_back(newRow);
}
