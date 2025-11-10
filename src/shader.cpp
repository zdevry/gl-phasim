#include <vector>
#include <fstream>
#include "utils.hpp"

bool compileShader(const char* path, GLenum type, GLuint& shader) {
    std::ifstream f(path, std::ios::ate);
    size_t length = f.tellg();
    f.seekg(0, std::ios::beg);

    std::vector<char> shaderCode(length);
    f.read(shaderCode.data(), length);
    f.close();

    const char* codeSources[] = { shaderCode.data() };
    GLint codeLengths[] = { static_cast<GLint>(length) };

    shader = glCreateShader(type);
    glShaderSource(shader, 1, codeSources, codeLengths);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        printf("%s failed to compile\n", path);
        return false;
    }

    return true;
}

bool createSimpleProgram(const char* vertPath, const char* fragPath, GLuint& program) {
    GLuint vertShader;
    GLuint fragShader;

    bool compileSuccess = compileShader(vertPath, GL_VERTEX_SHADER, vertShader);
    compileSuccess &= compileShader(fragPath, GL_FRAGMENT_SHADER, fragShader);

    if (!compileSuccess) return false;

    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        printf("Program(%s, %s) failed to link\n", vertPath, fragPath);
        return false;
    }

    return true;
}
