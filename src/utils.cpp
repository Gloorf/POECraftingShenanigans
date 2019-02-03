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
#include "utils.h"
// Adapted from http://rosettacode.org/wiki/Combinations#C.2B.2B


std::vector<std::vector<uint64_t>> Utils::combinations(uint64_t K, uint64_t N) {
    std::vector<std::vector<uint64_t>> out;
    std::vector<bool> bitmask(N); // Fill N false
    std::fill(bitmask.begin(), bitmask.begin() + static_cast<int64_t>(K),
              true); // replace K true (so we have k true and N-K false)

    // permute bitmask and save combination
    do {
        std::vector<uint64_t> singleCombination;
        for (uint64_t i = 0; i < N; ++i) // [0..N-1] integers
        {
            if (bitmask[i]) {
                singleCombination.push_back(i);
            }
        }
        out.push_back(singleCombination);
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    return out;
}

static std::random_device __randomDevice;
static std::mt19937 __mersenne(__randomDevice());

uint64_t Utils::randomInt(uint64_t min, uint64_t max) {
    std::uniform_real_distribution<double> dist(min, std::nextafter(max, DBL_MAX));

    return static_cast<uint64_t>(dist(__mersenne));
}


uint64_t Utils::randomAffixNumber(uint64_t fourModWeight, uint64_t fiveModWeight, uint64_t sixModWeight) {
    auto choice = randomInt(fourModWeight + fiveModWeight + sixModWeight);
    if (choice < fourModWeight) {
        return 4;
    } else if (choice < fourModWeight + fiveModWeight) {
        return 5;
    } else {
        return 6;
    }
}

uint64_t Utils::randomInt(uint64_t max) { return randomInt(1, max); }

size_t Utils::discreteDistribution(std::vector<uint64_t> weights) {
    std::discrete_distribution<> dist(weights.begin(), weights.end());
    return static_cast<size_t>(dist(__mersenne));
}

std::map<std::string, std::chrono::nanoseconds> Profiler::Results_;
std::map<std::string, std::chrono::high_resolution_clock::time_point> Profiler::Current_;

void Profiler::start(std::string name) {
#if PROFILER_ON
    Current_[name] = std::chrono::high_resolution_clock::now();
#endif
}

void Profiler::end(std::string name) {
#if PROFILER_ON
    auto time = std::chrono::high_resolution_clock::now();
    // As always, init to avoid += on non existant element :)
    if (Results_.find(name) == Results_.end()) {
        Results_[name] = std::chrono::nanoseconds(0);
    }
    Results_[name] += std::chrono::duration_cast<std::chrono::nanoseconds>(time - Current_[name]);
#endif
}

void Profiler::display() {
#if PROFILER_ON

    std::vector<std::tuple<std::string, std::chrono::nanoseconds>> toSort;
    for (auto& data : Results_) {
        toSort.push_back(make_tuple(data.first, data.second));
    }
    std::sort(toSort.begin(), toSort.end(),
              [](const auto& lhs, const auto& rhs) { return std::get<1>(lhs) > std::get<1>(rhs); });
    for (auto data : toSort) {
        std::cout << std::get<0>(data) << " ran in " << std::get<1>(data).count() / (1000. * 1000)
                  << "ms" << std::endl;
    }
#endif
}
