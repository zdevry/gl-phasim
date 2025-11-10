#include "utils.hpp"
#include "text.hpp"
#include "system.hpp"
#include "grid.hpp"

int main() {
    std::random_device rd;
    std::mt19937 twister(rd());

    int gridWidth = 320;
    int gridHeight = 180;

    auto system = makeCellSystem(gridWidth, gridHeight, 0.1, 0.2, twister);
    int energy = -countContancts(system);

    int winWidth = 1280;
    int winHeight = 720;
    float aspect = static_cast<float>(winWidth) / static_cast<float>(winHeight);

    GLFWwindow* window = initWindow(winWidth, winHeight, true, true);

    glfwSetWindowUserPointer(window, &system.temperature);
    glfwSetScrollCallback(window, [] (GLFWwindow* window, double xoff, double yoff) {
        float* systemTemp = reinterpret_cast<float*>(glfwGetWindowUserPointer(window));
        if (yoff >= 0.) {
            *systemTemp += 0.025f;
        } else {
            *systemTemp -= 0.025f;
            if (*systemTemp <= 0.005f) *systemTemp = 0.025f;
        }
    });

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto textsheetData = loadTextsheetData("assets/textsheet_960x120_R8.bin", 960, 120);
    TextRenderResources textRes;
    if (
        !loadTextResources(
            "shaders/text.vert", "shaders/text.frag",
            textsheetData, 960, 120, 16, aspect, textRes
        )
    ) return 1;

    GridRenderResources gridRes;
    if (!makeGrid(gridWidth, gridHeight, "shaders/grid.vert", "shaders/grid.frag", gridRes))
        return 1;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        for (int i = 0; i < 25'000; ++i)
            energy += metropolisStep(system, twister);
        subGridData(gridRes, system.cells.data());
        drawGrid(gridRes);

        char textBuffer[64];

        sprintf(textBuffer, "E = %i", energy);
        drawText(textRes, textBuffer, -1.f + 0.1 / aspect, 0.9f, 0.05f);

        sprintf(textBuffer, "T = %.3f", system.temperature);
        drawText(textRes, textBuffer, -1.f + 0.1 / aspect, -0.8f, 0.05f);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
