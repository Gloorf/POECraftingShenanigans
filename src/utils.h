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

#include <algorithm>
#include <cfloat>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

#include "constants.h"

#define PROFILER_ON false
namespace Utils {

// Return a vector of combination, each combination beeing a vector of int (starting at 0 so you can use it directly in an array)
std::vector<TFossilCombination> combinations(uint64_t K, uint64_t N);

uint64_t randomInt(uint64_t min, uint64_t max);
uint64_t randomInt(uint64_t max);
size_t discreteDistribution(std::vector<uint64_t> weights);
uint64_t randomAffixNumber(uint64_t fourModWeight, uint64_t fiveModWeight, uint64_t sixModWeight);

} // namespace Utils

class Profiler {
    public:
    static std::map<std::string, std::chrono::nanoseconds> Results_;
    static std::map<std::string, std::chrono::high_resolution_clock::time_point> Current_;

    void static start(std::string name);
    void static end(std::string name);
    void static display();
};
