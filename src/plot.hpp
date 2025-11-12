#pragma once

#include <glad/glad.h>

struct PlotRenderResources {
    GLuint plotProgram;
    GLuint plotVAO;
    GLuint plotDataBuffer;
    GLuint plotBuffer;
    int startIndex;
    int bufferLength;
};

bool makePlot(
    const char* plotVertPath, const char* plotFragPath,
    float originX, float originY, float scaleX, float scaleY,
    float lineWidth, int bufferLength,
    PlotRenderResources& res);
void appendPlotValue(PlotRenderResources& res, float val);
void drawPlot(const PlotRenderResources& res);
