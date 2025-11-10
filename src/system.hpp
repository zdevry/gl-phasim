#pragma once

#include <random>
#include <vector>

struct GridcellSystem {
    int width;
    int height;
    float temperature;

    std::vector<int> cells;
    std::vector<int> filledIdxs;
    std::vector<int> blankIdxs;

    std::uniform_int_distribution<int> filledIdxSampler;
    std::uniform_int_distribution<int> blankIdxSampler;
    std::uniform_real_distribution<float> acceptanceSampler;
};

GridcellSystem makeCellSystem(int width, int height, float proportion, float temperature, std::mt19937& twister);
int metropolisStep(GridcellSystem& system, std::mt19937& twister);
int countContancts(const GridcellSystem& system);
