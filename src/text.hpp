#pragma once

#include <vector>
#include "glad/glad.h"

struct TextRenderResources {
    GLuint textProgram;
    GLuint textVAO;
    GLuint textData;
    GLuint textBuffer;
    GLuint textsheetTexture;
};

std::vector<char> loadTextsheetData(const char* path, int width, int height);

bool loadTextResources(
    const char* textVertPath, const char* textFragPath,
    const std::vector<char>& textsheetData, int textsheetWidth, int textsheetHeight,
    int textsheetNumChars, float screenAspect,
    TextRenderResources& res);

void drawText(const TextRenderResources& res, const char* msg, float x, float y, float size);
