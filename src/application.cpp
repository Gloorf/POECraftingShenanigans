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

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


#include "application.h"
#include "utils.h"

CApplication::CApplication() {
    modManager_ = std::make_unique<CModManager>();
    baseItemTypeManager_ = std::make_unique<CBaseItemTypeManager>();
    modTypeManager_ = std::make_unique<CModTypeManager>();
    fossilManager_ = std::make_unique<CFossilManager>();
    readMods();
    readModTypes();
    readFossils();
    readBaseItemTypes();
    readIronRing();
    modTypeManager_->fillCache();
}

CItem* CApplication::createItem(TLevel level, std::string baseType, EItemSpecial special, std::vector<std::string> fossilNames) {
    // First, we need to find the tags matching baseType
    std::vector<TItemTag> tags = baseItemTypeManager_->itemTags(baseType, special);
    // Now, let's filter our mods
    std::vector<CMod*> mods = modManager_->findMods(tags, level, false, {});
    // Let's check our fossils
    std::vector<CWeightModifier*> fossilWeights;
    for (auto name : fossilNames) {
        CFossil* fossil = fossilManager_->find(name);
        if (fossil->additionalModsRowIds.size() > 0) {
            std::vector<CMod*> fossilMods
            = modManager_->findMods(tags, level, true, fossil->additionalModsRowIds);
            mods.insert(mods.end(), fossilMods.begin(), fossilMods.end());
        }
        // our new mods just act like regular mods from now on
        // Grab our new weights :)
        fossilWeights.insert(fossilWeights.end(), fossil->weights.begin(), fossil->weights.end());
    }
    // We need to insert our CModType* in the mods !
    for (auto m : mods) {
        m->modType(modTypeManager_->find(m->modTypeKey));
    }
    CItem* out = new CItem(level, baseType, modManager_->groupMods(mods));
    out->updateWeight(fossilWeights);
    return out;
}
std::vector<COptimiseResult*> CApplication::optimiseSingleMod(TLevel level,
                                                              std::string baseType,
                                                              EItemSpecial special,
                                                              std::string modId,
                                                              std::vector<std::string> possibleFossils,
                                                              uint64_t maxFossils) {
    std::vector<COptimiseResult*> out;

    std::vector<TFossilCombination> allCombinations;
    for (uint64_t i = 1; i <= maxFossils; i++) {
        auto combs = Utils::combinations(i, possibleFossils.size());
        allCombinations.insert(allCombinations.end(), combs.begin(), combs.end());
    }
    for (auto comb : allCombinations) {
        std::vector<std::string> chosenFossils;
        for (auto index : comb) {
            chosenFossils.push_back(possibleFossils[index]);
        }
        CItem* item = createItem(level, baseType, special, chosenFossils);
        double weightPercent = item->findModPercent(modId);
        out.push_back(new COptimiseResult(weightPercent, chosenFossils));
    }

    std::sort(out.begin(), out.end(), [](const auto& lhs, const auto& rhs) {
        return lhs->weightPercent > rhs->weightPercent; // reverse sort cause of >
    });

    return out;
}

CSimulateResult*
CApplication::simulateCraft(CItem* item, uint64_t craftNumber, std::string mod1, std::string mod2) {
    Profiler::start("simulateCraft");
    item->cacheForCraft();
    auto begin = std::chrono::high_resolution_clock::now();
    uint64_t mod1Count = 0;
    uint64_t mod2Count = 0;
    uint64_t mod1And2Count = 0;
    std::vector<std::vector<std::string>> savedResults;

    for (uint64_t i = 0; i < craftNumber; i++) {
        auto affixNumber = Utils::randomAffixNumber(affixData_->fourModCount, affixData_->fiveModCount,
                                                    affixData_->sixModCount);
        auto result = item->simulateCraft(affixNumber);
        bool mod1Found = false;
        bool mod2Found = false;
        std::vector<std::string> singleResult;
        for (auto mod : result) {
            singleResult.push_back(mod->id);
            if (mod->id == mod1) {
                mod1Count++;
                mod1Found = true;
            }
            if (mod->id == mod2) {
                mod2Count++;
                mod2Found = true;
            }
        }
        savedResults.push_back(singleResult);
        if (mod1Found && mod2Found) {
            mod1And2Count++;
        }
    }
    Profiler::end("simulateCraft");
    Profiler::display();
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "(non invasive calc) "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / (1000. * 1000)
              << "ms total runtime " << std::endl;
    item->emptyCache();
    return new CSimulateResult(mod1Count, mod2Count, mod1And2Count, savedResults);
}


bool CApplication::readBaseItemTypes() {

    QFile file(":/data/SimpleBaseItemTypes.json");
    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "Couldn't open SimpleBaseItemTypes.json" << std::endl;
        return false;
    }
    QJsonDocument document(QJsonDocument::fromJson(file.readAll()));
    if (!document.isArray()) {
        std::cerr << "SimpleBaseItemTypes.json is malformed" << std::endl;
        return false;
    }
    QJsonArray json = document.array();
    for (QJsonValue baseItemType : json) {
        if (!baseItemType.isObject()) {
            std::cerr
            << "SimpleBaseItemTypes.json is malformed : baseItemType is supposed to be an "
               "object but got "
            << baseItemType.type() << std::endl;
            return false;
        }
        QJsonObject obj = baseItemType.toObject();
        if (!obj.contains("name") || !obj.contains("class_id") || !obj.contains("tags_int")) {
            std::cerr
            << "SimpleBaseItemTypes.json is malformed : baseItemType is missing some attribute"
            << std::endl;
            return false;
        }

        if (!obj["name"].isString() || !obj["class_id"].isString() || !obj["tags_int"].isArray()) {
            std::cerr
            << "SimpleBaseItemTypes.json is malformed : baseItemType have attributes with "
               "the wrong type"
            << std::endl;
            return false;
        }
        std::vector<TItemTag> tags;
        for (auto tag : obj["tags_int"].toArray()) {
            tags.push_back(static_cast<TItemTag>(tag.toInt()));
        }
        // Some item are craftable but not as shaper/elder item (like jewels), so these two parameters are optional
        TItemTag shaper_tag = 0;
        TItemTag elder_tag = 0;
        if (obj.contains("shaper_tag") && obj["shaper_tag"].isDouble()) {
            shaper_tag = static_cast<TItemTag>(obj["shaper_tag"].toInt());
        }
        if (obj.contains("elder_tag") && obj["elder_tag"].isDouble()) {
            elder_tag = static_cast<TItemTag>(obj["elder_tag"].toInt());
        }
        baseItemTypeManager_->addBaseItem(obj["name"].toString().toStdString(),
                                          obj["class_id"].toString().toStdString(), tags, shaper_tag, elder_tag);
    }
    // Let's sort our items now !
    baseItemTypeManager_->sortItems();
    return true;
}

bool CApplication::readIronRing() {
    QFile file(":/data/IronRingData.json");
    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "Couldn't open IronRingData.json" << std::endl;
        return false;
    }
    QJsonDocument document(QJsonDocument::fromJson(file.readAll()));
    if (!document.isObject()) {
        std::cerr << "IronRingData.json is malformed" << std::endl;
        return false;
    }
    QJsonObject obj = document.object();
    if (!obj.contains("four_mods") || !obj.contains("five_mods") || !obj.contains("six_mods")) {
        std::cerr << "IronRingData.json is malformed, missing mod count" << std::endl;
        return false;
    }

    if (!obj["four_mods"].isDouble() || !obj["five_mods"].isDouble() || !obj["six_mods"].isDouble()) {
        std::cerr << "IronRingData.json is malformed, mod count has the wrong type" << std::endl;
        return false;
    }

    affixData_ = std::make_unique<CAffixData>(obj["four_mods"].toInt(), obj["five_mods"].toInt(),
                                              obj["six_mods"].toInt());

    return true;
}

bool CApplication::readModTypes() {
    // This is not safe to cast a unique_ptr like this but i'm a lazy fucker
    return readJsonFile(":/data/ggpk/ModType.json", &*modTypeManager_);
}

bool CApplication::readFossils() {
    // This is not safe to cast a unique_ptr like this but i'm a lazy fucker
    return readJsonFile(":/data/ggpk/DelveCraftingModifiers.json", &*fossilManager_);
}

bool CApplication::readMods() {
    // This is not safe to cast a unique_ptr like this but i'm a lazy fucker
    return readJsonFile(":/data/ggpk/Mods.json", &*modManager_);
}

bool CApplication::readJsonFile(std::string filename, CGGPKDatFile* dest) {
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "Couldn't open " << filename << std::endl;
        return false;
    }

    QJsonDocument document(QJsonDocument::fromJson(file.readAll()));
    // Sadly, Qt has no support for a JSON schema validator (and I don't want to pull in another
    // dependencies), we'll have to do it by hand Our json is fairly controlled (export from pypoe),
    // so it's mainly here to avoid crash if pypoe format changes

    if (!document.isArray()) {
        std::cerr << filename << " is malformed : root is supposed to be an array" << std::endl;
        return false;
    }
    QJsonArray json = document.array();
    if (json.size() != 1) {
        std::cerr << filename << " is malformed : root array is suppoed to be length 1 but is length "
                  << json.size() << std::endl;
        return false;
    }
    if (!json[0].isObject()) {
        std::cerr << filename << " is malformed : root[0] is supposed to be an object, but is actually "
                  << json[0].type() << std::endl;
        return false;
    }
    QJsonObject root = json[0].toObject();
    if (!root.contains("header") || !root.contains("data")) {
        std::cerr << filename << " is malformed : root[0] doesn't contains header and/or data" << std::endl;
        return false;
    }
    if (!root["header"].isArray() || !root["data"].isArray()) {
        std::cerr << filename << " is malformed : header / data is not an array" << std::endl;
        return false;
    }

    QJsonArray header = root["header"].toArray();
    QJsonArray data = root["data"].toArray();
    // At this point we should (hopefully) have decent data !
    // First, let's get our header data
    for (QJsonValue h : header) {
        if (!h.isObject()) {
            std::cerr << filename << " is malformed : expected object in header, got " << h.type() << std::endl;
            return false;
        }
        QJsonObject obj = h.toObject();
        if (!obj.contains("name") || !obj.contains("type")) {
            std::cerr << filename << " is malformed : couldn't find name/type in header object" << std::endl;
            return false;
        }
        if (!obj["name"].isString() || !obj["type"].isString()) {
            std::cerr << filename << " is malformed : in header object, name/type should be str/str but it is "
                      << obj["name"].type() << " / " << obj["type"].type() << std::endl;
            return false;
        }
        dest->addColumn(obj["name"].toString().toStdString(), obj["type"].toString().toStdString());
    }
    // Then, time to populate data ! :D
    for (TRowId rowId = 0; rowId < static_cast<uint64_t>(data.size()); rowId++) {
        auto d = data[static_cast<int>(rowId)];
        if (!d.isArray()) {
            std::cerr << filename << " is malformed : expected array in data, got " << d.type() << std::endl;
            return false;
        }
        dest->addRow(d.toArray(), rowId);
    }
    return true;
}

void CSimulateResult::exportJson(std::string filename) {
    QFile file(QString::fromStdString(filename));

    if (!file.open(QIODevice::WriteOnly)) {
        std::cerr << "Couldn't open " << filename << std::endl;
        return;
    }

    QJsonArray allData;
    for (auto craft : allModIds) {
        QJsonArray singleCraft;
        for (auto modId : craft) {
            singleCraft.append(QString::fromStdString(modId));
        }
        allData.append(singleCraft);
    }

    QJsonDocument saveDoc(allData);
    file.write(saveDoc.toJson());
}
