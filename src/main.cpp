#include "utils.hpp"
#include "text.hpp"

int main() {
    int winWidth = 800;
    int winHeight = 600;
    float aspect = static_cast<float>(winWidth) / static_cast<float>(winHeight);

    GLFWwindow* window = initWindow(winWidth, winHeight, true, true);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto textsheetData = loadTextsheetData("assets/textsheet_960x120_R8.bin", 960, 120);
    TextRenderResources textRes;
    if (
        !loadTextResources(
            "shaders/text.vert", "shaders/text.frag",
            textsheetData, 960, 120, 16, aspect, textRes
        )
    ) {
        glfwTerminate();
        return 1;
    }


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        drawText(textRes, "12345", -1.f, 1.f, 0.05f);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
