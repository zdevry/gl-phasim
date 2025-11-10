#include <cstdio>
#include "utils.hpp"

void debugPrintCallback(
    GLenum source, GLenum type, GLuint id, GLenum sev,
    GLsizei length, const GLchar* message, const void* userParam)
{
    printf("OPENGL: %s\n", message);
}

GLFWwindow* initWindow(int width, int height, bool vsync, bool debug) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    printf("Renderer: %s\n", glGetString(GL_RENDERER));

    if (!vsync) {
        glfwSwapInterval(0);
    }

    if (debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debugPrintCallback, nullptr);
    }

    glViewport(0, 0, width, height);

    return window;
}
