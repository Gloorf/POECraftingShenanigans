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
#include <QCheckBox>
#include <QMainWindow>
#include <QTableWidget>

#include "application.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
    MainWindow(std::unique_ptr<CApplication> app);
    ~MainWindow();
    public slots:
    void onItemChanged(); // Called when something about the object property is changed ; recreate an item and update display

    void onOptimiseClicked();
    void onSimulateClicked();
    void onSimulateJSONClicked();

    private:
    Ui::MainWindow* ui;
    std::unique_ptr<CApplication> app_;
    CItem* currentItem_;

    void initDisplayTab();
    void initOptimiseTab();
    void initSimulateTab();

    void updateModTables();
    void updateSingleTable(QTableWidget* table, std::vector<CMod*> mods, bool isPrefix); // isPrefix is only used to calculate % across prefix/suffix
    void updateWeightData();
    void updateCountData();
    void updateOptimiseBox();
    void updateSimulateBox();


    std::vector<std::string> getIncludedFossils();
    std::vector<QCheckBox*> includedFossilsBoxes_;
    std::string JSONExportPath_;
};
