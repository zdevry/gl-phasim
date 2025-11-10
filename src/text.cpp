#include <fstream>
#include "text.hpp"
#include "utils.hpp"

std::vector<char> loadTextsheetData(const char* path, int width, int height) {
    std::vector<char> data(width * height);
    std::ifstream f(path, std::ios::binary);

    f.read(data.data(), width * height);
    f.close();

    return data;
}

bool loadTextResources(
    const char* textVertPath, const char* textFragPath,
    const std::vector<char>& textsheetData, int textsheetWidth, int textsheetHeight,
    int textsheetNumChars, float screenAspect,
    TextRenderResources& res)
{
    glCreateVertexArrays(1, &res.textVAO);

    if (!createSimpleProgram(textVertPath, textFragPath, res.textProgram)) return false;


    glCreateTextures(GL_TEXTURE_2D, 1, &res.textsheetTexture);
    glTextureParameteri(res.textsheetTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(res.textsheetTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(res.textsheetTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(res.textsheetTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(res.textsheetTexture, 1, GL_R8, textsheetWidth, textsheetHeight);
    glTextureSubImage2D(res.textsheetTexture, 0, 0, 0, textsheetWidth, textsheetHeight, GL_RED, GL_UNSIGNED_BYTE, textsheetData.data());

    glBindTextureUnit(0, res.textsheetTexture);


    float textData[5] = { -1.f, 1.f, 0.0f, static_cast<float>(textsheetNumChars), screenAspect };

    glCreateBuffers(1, &res.textData);
    glNamedBufferStorage(res.textData, 5 * sizeof(float), textData, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, res.textData);


    glCreateBuffers(1, &res.textBuffer);
    glNamedBufferStorage(res.textBuffer, 64 * sizeof(int), nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, res.textBuffer);

    return true;
}

int encodeText(const char* src, int* dst, int max) {
    int currIdx = 0;
    for (; currIdx < max && src[currIdx] != 0; ++currIdx) {
        char curr = src[currIdx];

        switch (curr) {
            case '.':
                dst[currIdx] = 10;
                break;
            case 'T':
                dst[currIdx] = 11;
                break;
            case 'E':
                dst[currIdx] = 12;
                break;
            case '=':
                dst[currIdx] = 13;
                break;
            case '-':
                dst[currIdx] = 14;
                break;
            case ' ':
                dst[currIdx] = 15;
                break;
            default:
                dst[currIdx] = curr - '0';
                break;
        }
    }

    return currIdx;
}

void drawText(const TextRenderResources& res, const char* msg, float x, float y, float size) {
    float textData[3] = { x, y, size };
    glNamedBufferSubData(res.textData, 0, 3 * sizeof(float), textData);

    int staging[64];
    int msgLen = encodeText(msg, staging, 64);
    glNamedBufferSubData(res.textBuffer, 0, msgLen * sizeof(int), staging);

    glUseProgram(res.textProgram);
    glBindVertexArray(res.textVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, msgLen);
}
