#pragma once

#include "glad/glad.h"

struct GridRenderResources {
    int gridWidth;
    int gridHeight;

    GLuint gridProgram;
    GLuint gridVAO;
    GLuint gridDataBuffer;
    GLuint gridBuffer;
};

bool makeGrid(int width, int height, const char* gridVertPath, const char* gridFragPath, GridRenderResources& res);
void subGridData(const GridRenderResources& res, const int* gridData);
void drawGrid(const GridRenderResources& res);
