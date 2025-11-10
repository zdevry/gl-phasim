#include <algorithm>
#include <cmath>
#include "system.hpp"

GridcellSystem makeCellSystem(int width, int height, float proportion, float temperature, std::mt19937& twister) {
    GridcellSystem system;

    system.width = width;
    system.height = height;
    system.temperature = temperature;

    int totalCells = width * height;
    int filledCells = static_cast<int>(proportion * totalCells);

    system.cells.resize(totalCells);
    for (int i = 0; i < filledCells; ++i) {
        system.cells[i] = 1;
    }

    std::shuffle(std::begin(system.cells), std::end(system.cells), twister);

    for (int i = 0; i < totalCells; ++i)
        (system.cells[i] ? system.filledIdxs : system.blankIdxs)
            .push_back(i);

    system.filledIdxSampler = std::uniform_int_distribution<int>(0, filledCells - 1);
    system.blankIdxSampler = std::uniform_int_distribution<int>(0, totalCells - filledCells - 1);
    system.acceptanceSampler = std::uniform_real_distribution<float>(0.f, 1.f);

    return system;
}

bool isFilled(const GridcellSystem& system, int i, int j) {
    return i >= 0 && i < system.width && j >= 0 && j < system.height
        && system.cells[i + j * system.width];
}

int getAdjacent(const GridcellSystem& system, int i, int j) {
    return isFilled(system, i - 1, j)
        + isFilled(system, i + 1, j)
        + isFilled(system, i, j - 1)
        + isFilled(system, i, j + 1);
}

int getAdjacentByIdx(const GridcellSystem& system, int idx) {
    int i = idx % system.width;
    int j = idx / system.width;

    return getAdjacent(system, i, j);
}

int metropolisStep(GridcellSystem& system, std::mt19937& twister) {
    int swapFli = system.filledIdxSampler(twister);
    int swapBli = system.blankIdxSampler(twister);
    int swapFi = system.filledIdxs[swapFli];
    int swapBi = system.blankIdxs[swapBli];

    int energyChange = getAdjacentByIdx(system, swapFi) - getAdjacentByIdx(system, swapBi);
    int idxDifference = abs(swapFi - swapBi);
    if (idxDifference == 1 || idxDifference == system.width) { // account for adjacencies
        ++energyChange;
    }

    float acceptance = std::exp(-energyChange / system.temperature);

    if (system.acceptanceSampler(twister) > acceptance)
        return 0;

    int swaptmp = system.filledIdxs[swapFli];
    system.filledIdxs[swapFli] = system.blankIdxs[swapBli];
    system.blankIdxs[swapBli] = swaptmp;

    system.cells[swapFi] = 0;
    system.cells[swapBi] = 1;

    return energyChange;
}

int countContancts(const GridcellSystem& system) {
    int totalContacts = 0;
    for (int j = 0; j < system.height; ++j)
        for (int i = 0; i < system.width; ++i) {
            if (!isFilled(system, i, j)) continue;

            totalContacts += getAdjacent(system, i, j);
        }
    return totalContacts / 2;
}
