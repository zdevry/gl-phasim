#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* initWindow(int width, int height, bool vsync, bool debug);
bool createSimpleProgram(const char* vertPath, const char* fragPath, GLuint& program);
