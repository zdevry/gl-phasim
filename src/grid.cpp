#include "grid.hpp"
#include "utils.hpp"

bool makeGrid(int width, int height, const char* gridVertPath, const char* gridFragPath, GridRenderResources& res) {
    res.gridWidth = width;
    res.gridHeight = height;

    glCreateVertexArrays(1, &res.gridVAO);

    if (!createSimpleProgram(gridVertPath, gridFragPath, res.gridProgram)) return false;

    int gridUniformData[2] = { width, height };
    glCreateBuffers(1, &res.gridDataBuffer);
    glNamedBufferStorage(res.gridDataBuffer, 2 * sizeof(int), gridUniformData, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, res.gridDataBuffer);

    glCreateBuffers(1, &res.gridBuffer);
    glNamedBufferStorage(res.gridBuffer, width * height * sizeof(int), nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, res.gridBuffer);

    return true;
}

void subGridData(const GridRenderResources& res, const int* gridData) {
    glNamedBufferSubData(res.gridBuffer, 0, res.gridWidth * res.gridHeight * sizeof(int), gridData);
}

void drawGrid(const GridRenderResources& res) {
    glUseProgram(res.gridProgram);
    glBindVertexArray(res.gridVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
