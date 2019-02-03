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
#include <vector>

#include <QDebug>
#include <QFileDialog>

#include "mainwindow.h"
#include "mod.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(std::unique_ptr<CApplication> app)
: ui(new Ui::MainWindow), app_(std::move(app)) {
    ui->setupUi(this);
    initDisplayTab();
    initOptimiseTab();
    initSimulateTab();
    showMaximized();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::initDisplayTab() {
    // Let's populate our combo box
    for (auto name : app_->baseItemNames()) {
        ui->itemBaseBox->addItem(QString::fromStdString(name));
    }
    // Fossil boxes in display tab
    for (auto name : app_->fossilNames()) {
        ui->fossilBox_1->addItem(QString::fromStdString(name));
        ui->fossilBox_2->addItem(QString::fromStdString(name));
        ui->fossilBox_3->addItem(QString::fromStdString(name));
        ui->fossilBox_4->addItem(QString::fromStdString(name));
    }
    // We'll take the lazy road and connect all our signal to a single slot
    connect(ui->itemBaseBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onItemChanged);
    connect(ui->levelBox, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onItemChanged);
    connect(ui->shaperElderBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onItemChanged);
    connect(ui->fossilBox_1, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onItemChanged);
    connect(ui->fossilBox_2, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onItemChanged);
    connect(ui->fossilBox_3, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onItemChanged);
    connect(ui->fossilBox_4, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onItemChanged);

    // Small easter egg ;-)
    ui->itemBaseBox->setCurrentText("Eclipse Staff");
    // Manually call it at init so we have something to show the user !
    onItemChanged();
}

void MainWindow::initOptimiseTab() {
    auto layout = ui->includedFossilsLayout;
    int row = 0;
    int column = 0;
    for (auto name : app_->fossilNames()) {
        QCheckBox* item = new QCheckBox(QString::fromStdString(name));
        if (!app_->rareFossil(name)) item->setChecked(true);
        layout->addWidget(item, row, column);
        includedFossilsBoxes_.push_back(item);
        column += 1;
        if (column == 4) {
            row += 1;
            column = 0;
        }
    }
    connect(ui->optimiseButton, &QPushButton::clicked, this, &MainWindow::onOptimiseClicked);
    // TODO
}

void MainWindow::initSimulateTab() {
    connect(ui->simulateButton, &QPushButton::clicked, this, &MainWindow::onSimulateClicked);
    connect(ui->simulateExportButton, &QPushButton::clicked, this, &MainWindow::onSimulateJSONClicked);
}

void MainWindow::onItemChanged() {
    std::vector<std::string> fossilNames;
    if (ui->fossilBox_1->currentIndex() != 0) {
        fossilNames.push_back(ui->fossilBox_1->currentText().toStdString());
    }
    if (ui->fossilBox_2->currentIndex() != 0) {
        fossilNames.push_back(ui->fossilBox_2->currentText().toStdString());
    }
    if (ui->fossilBox_3->currentIndex() != 0) {
        fossilNames.push_back(ui->fossilBox_3->currentText().toStdString());
    }
    if (ui->fossilBox_4->currentIndex() != 0) {
        fossilNames.push_back(ui->fossilBox_4->currentText().toStdString());
    }
    currentItem_ = app_->createItem(static_cast<TLevel>(ui->levelBox->value()),
                                    ui->itemBaseBox->currentText().toStdString(),
                                    static_cast<EItemSpecial>(ui->shaperElderBox->currentIndex()), fossilNames);
    updateModTables();
    updateWeightData();
    updateCountData();
    updateOptimiseBox();
    updateSimulateBox();
}

void MainWindow::onOptimiseClicked() {
    auto results
    = app_->optimiseSingleMod(static_cast<TLevel>(ui->levelBox->value()),
                              ui->itemBaseBox->currentText().toStdString(),
                              static_cast<EItemSpecial>(ui->shaperElderBox->currentIndex()),
                              ui->optimiseModBox->currentText().toStdString(), getIncludedFossils(),
                              static_cast<uint64_t>(ui->maxFossilsBox->value()));
    // ui->table
    ui->fossilTable->clearContents();
    ui->fossilTable->setRowCount(static_cast<int>(results.size()));

    for (size_t i = 0; i < results.size(); i++) {
        auto result = results[i];
        QTableWidgetItem* weightItem = new QTableWidgetItem;
        weightItem->setData(Qt::DisplayRole, static_cast<double>(result->weightPercent));
        ui->fossilTable->setItem(static_cast<int>(i), 4, weightItem);
        for (size_t j = 0; j < result->fossilNames.size(); j++) {
            ui->fossilTable->setItem(static_cast<int>(i), static_cast<int>(j),
                                     new QTableWidgetItem(QString::fromStdString(result->fossilNames[j])));
        }
    }
    ui->fossilTable->resizeColumnsToContents();
}

void MainWindow::onSimulateClicked() {
    auto result = app_->simulateCraft(currentItem_, static_cast<uint64_t>(ui->simulateNumberBox->value()),
                                      ui->simulateModBox_1->currentText().toStdString(),
                                      ui->simulateModBox_2->currentText().toStdString());
    if (JSONExportPath_ != "") {
        std::string fullPath = JSONExportPath_ + "/" + "POECraftingShenanigansExport.json";
        result->exportJson(fullPath);
    }
    ui->simulateResultBox_1->setValue(static_cast<int>(result->mod1));
    ui->simulateResultBox_2->setValue(static_cast<int>(result->mod2));
    ui->simulateResultBox_1_2->setValue(static_cast<int>(result->mod1And2));
}

void MainWindow::onSimulateJSONClicked() {
    auto filename = QFileDialog::getExistingDirectory(this, tr("Select json export path"));
    JSONExportPath_ = filename.toStdString();
}


void MainWindow::updateModTables() {
    updateSingleTable(ui->prefixTable, currentItem_->prefixMods(), true);
    updateSingleTable(ui->suffixTable, currentItem_->suffixMods(), false);
}

void MainWindow::updateSingleTable(QTableWidget* table, std::vector<CMod*> mods, bool isPrefix) {
    // QT does strange thing (like deleting data) when you add item to a sorting enabled table so ... '-'
    table->setSortingEnabled(false);
    table->clearContents();
    table->setRowCount(static_cast<int>(mods.size()));
    for (size_t i = 0; i < mods.size(); i++) {
        auto mod = mods[i];

        QTableWidgetItem* idItem = new QTableWidgetItem(QString::fromStdString(mod->id));
        idItem->setToolTip(QString::fromStdString(mod->name));
        table->setItem(static_cast<int>(i), 0, idItem);

        QTableWidgetItem* levelItem = new QTableWidgetItem;
        levelItem->setData(Qt::DisplayRole, static_cast<int>(mod->level)); // Allows us to numerically sort (default sort is alphanumeric sort)
        table->setItem(static_cast<int>(i), 1, levelItem);

        // Don't forget to force one as double to have a double result (or else we'll have an integer division)
        double weightPercent = (static_cast<double>(mod->weight) / currentItem_->totalWeight()) * 100;
        double weightLocalPercent
        = 100
          * (static_cast<double>(mod->weight)
             / (isPrefix ? currentItem_->prefixWeight() : currentItem_->suffixWeight()));
        // Could probably put the display precision in a global constant
        QString weightText
        = QString::number(mod->weight) + " (" + QString::number(weightPercent, 'f', 3)
          + "% chance to roll across all mods - " + QString::number(weightLocalPercent, 'f', 3)
          + "% chance to roll across " + (isPrefix ? "prefixes" : "suffixes") + ")";
        QTableWidgetItem* weightItem = new QTableWidgetItem;
        weightItem->setData(Qt::DisplayRole, static_cast<int>(mod->weight)); // Allows us to numerically sort (default sort is alphanumeric sort)
        weightItem->setToolTip(weightText);
        table->setItem(static_cast<int>(i), 2, weightItem);

        QTableWidgetItem* groupItem = new QTableWidgetItem;
        groupItem->setData(Qt::DisplayRole, QString::fromStdString(mod->groupName));
        table->setItem(static_cast<int>(i), 3, groupItem);
    }
    table->setSortingEnabled(true);
    table->sortItems(2, Qt::SortOrder::DescendingOrder);
    table->resizeColumnsToContents();
}

void MainWindow::updateWeightData() {
    ui->totalWeightBox->setValue(static_cast<int>(currentItem_->totalWeight()));
    ui->prefixWeightBox->setValue(static_cast<int>(currentItem_->prefixWeight()));
    ui->suffixWeightBox->setValue(static_cast<int>(currentItem_->suffixWeight()));
}

void MainWindow::updateCountData() {
    ui->totalCountBox->setValue(static_cast<int>(currentItem_->mods().size()));
    ui->prefixCountBox->setValue(static_cast<int>(currentItem_->prefixMods().size()));
    ui->suffixCountBox->setValue(static_cast<int>(currentItem_->suffixMods().size()));
}

void MainWindow::updateOptimiseBox() {
    QStringList tmp;
    ui->optimiseModBox->clear();
    // Let's do it in two step to get a sorted ComboBox

    for (auto m : currentItem_->mods()) {
        tmp.push_back(QString::fromStdString(m->id));
    }
    tmp.sort();
    ui->optimiseModBox->addItems(tmp);
}

void MainWindow::updateSimulateBox() {
    ui->simulateModBox_1->clear();
    ui->simulateModBox_2->clear();
    // Let's do it in two step to get a sorted ComboBox
    QStringList tmp;
    for (auto m : currentItem_->mods()) {
        tmp.push_back(QString::fromStdString(m->id));
    }
    tmp.sort();
    ui->simulateModBox_1->addItems(tmp);
    ui->simulateModBox_2->addItems(tmp);
}

std::vector<std::string> MainWindow::getIncludedFossils() {
    std::vector<std::string> out;
    for (auto box : includedFossilsBoxes_) {
        if (box->isChecked()) {
            out.push_back(box->text().toStdString());
        }
    }
    return out;
}
