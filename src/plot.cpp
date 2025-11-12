#include "utils.hpp"
#include "plot.hpp"

bool makePlot(
    const char* plotVertPath, const char* plotFragPath,
    float originX, float originY, float scaleX, float scaleY,
    float lineWidth, int bufferLength,
    PlotRenderResources& res)
{
    res.bufferLength = bufferLength;
    res.startIndex = 0;

    glCreateVertexArrays(1, &res.plotVAO);

    if (!createSimpleProgram(plotVertPath, plotFragPath, res.plotProgram)) return false;


    float floatPlotData[5] = { originX, originY, scaleX, scaleY, lineWidth };
    int intPlotData[2] = { 0, bufferLength };

    glCreateBuffers(1, &res.plotDataBuffer);
    glNamedBufferStorage(res.plotDataBuffer, 5 * sizeof(float) + 2 * sizeof(int), nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(res.plotDataBuffer, 0, 5 * sizeof(float), floatPlotData);
    glNamedBufferSubData(res.plotDataBuffer, 5 * sizeof(float), 2 * sizeof(int), intPlotData);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, res.plotDataBuffer);

    glCreateBuffers(1, &res.plotBuffer);
    glNamedBufferStorage(res.plotBuffer, bufferLength * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, res.plotBuffer);

    return true;
}

void appendPlotValue(PlotRenderResources& res, float val) {
    --res.startIndex;
    if (res.startIndex < 0) res.startIndex += res.bufferLength;

    glNamedBufferSubData(res.plotBuffer, res.startIndex * sizeof(float), sizeof(float), &val);
    glNamedBufferSubData(res.plotDataBuffer, 20, sizeof(int), &res.startIndex);
}

void drawPlot(const PlotRenderResources& res) {
    glUseProgram(res.plotProgram);
    glBindVertexArray(res.plotVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * res.bufferLength);
}
