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

#include <exception>
#include <string>

#include <QJsonArray>

#include "constants.h"

// This is starting to be ugly
class NullAttributeAccess : public std::exception {


    public:
    NullAttributeAccess();
    explicit NullAttributeAccess(const std::string& name);
    const char* what() const noexcept;

    private:
    std::string message_;
};
/*
 * Hold a GGPKRow, and a get() method for easier access to the underlying attributes
 * */

class CGGPKRowAccessor {
    public:
    explicit CGGPKRowAccessor(TGGPKRow attributes, TRowId rowId)
    : attributes_(attributes), rowId_(rowId) {}
    template <typename T> T get(std::string key) {
        bool exists = attributes_.find(key) != attributes_.end();
        if (exists && attributes_[key].has_value()) {
            return std::get<T>(attributes_[key].value());
        } else {
            display();
            throw NullAttributeAccess(key);
        }
    }
    void display(); // Debug function

    protected:
    TGGPKRow attributes_;
    TRowId rowId_;
};
/*
 * This is built using the "header" part of a json export of a .dat file
 * Hold a column : its name, its type, and a value() function that transform a QJSonValue into the appropriate variant
 *
 * */
class CGGPKColumn {
    public:
    CGGPKColumn(TGGPKHeader name, std::string type) : name(name), type_(type) {}
    TJsonNullableType value(QJsonValue value); // Transform
    std::string name;

    private:
    std::string type_;
};

/*
 * Allow an easy access to a GGPK dat file, using the json export (I'm not actually going to parse binary, I'm not that crazy)
 * */
class CGGPKDatFile {
    public:
    CGGPKDatFile() {}
    void addColumn(TGGPKHeader name, std::string type);
    void addRow(QJsonArray data, TRowId rowId);

    protected:
    std::vector<CGGPKColumn*> columns_;
    std::vector<CGGPKRowAccessor*> rows_; // For now a simple vector containing all mods will do ; will look into it again if perf boost are needed
};
