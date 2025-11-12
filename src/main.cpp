#include <string>
#include <fstream>
#include <chrono>
#include "utils.hpp"
#include "text.hpp"
#include "system.hpp"
#include "grid.hpp"
#include "plot.hpp"

struct ProgramInput {
    int gridWidth;
    int gridHeight;
    float proportion;
    float initialTemperature;

    int winWidth;
    int winHeight;
    int vsync;
    int debug;

    int metropolisStepsPerFrame;
    int plotSkip;
    int plotLength;
    float textWidth;
    float textMargin;

    std::string textsheetPath;
    int textsheetWidth;
    int textsheetHeight;
    int textsheetNumChars;
    std::string textVertPath;
    std::string textFragPath;

    std::string gridVertPath;
    std::string gridFragPath;

    std::string plotVertPath;
    std::string plotFragPath;
};

std::string readConfigLine(std::ifstream& f) {
    std::string line;
    std::getline(f, line);

    return line.substr(0, line.find(' '));
}

ProgramInput readProgramInputFromFile(const char* path) {
    ProgramInput input;

    std::ifstream f(path);

    if (!f.good()) {
        printf("Failed to open %s\n", path);
        exit(1);
    }

    input.gridWidth = stoi(readConfigLine(f));
    input.gridHeight = stoi(readConfigLine(f));
    input.proportion = stof(readConfigLine(f));
    input.initialTemperature = stof(readConfigLine(f));
    input.winWidth = stoi(readConfigLine(f));
    input.winHeight = stoi(readConfigLine(f));
    input.vsync = stoi(readConfigLine(f));
    input.debug = stoi(readConfigLine(f));
    input.metropolisStepsPerFrame = stoi(readConfigLine(f));
    input.plotSkip = stoi(readConfigLine(f));
    input.plotLength = stoi(readConfigLine(f));
    input.textWidth = stof(readConfigLine(f));
    input.textMargin = stof(readConfigLine(f));
    input.textsheetPath = readConfigLine(f);
    input.textsheetWidth = stoi(readConfigLine(f));
    input.textsheetHeight = stoi(readConfigLine(f));
    input.textsheetNumChars = stoi(readConfigLine(f));
    input.textVertPath = readConfigLine(f);
    input.textFragPath = readConfigLine(f);
    input.gridVertPath = readConfigLine(f);
    input.gridFragPath = readConfigLine(f);
    input.plotVertPath = readConfigLine(f);
    input.plotFragPath = readConfigLine(f);

    f.close();

    return input;
}

int main(int argc, const char** argv) {
    const char* inputPath = argc == 1 ? "phasim.conf" : argv[1];

    ProgramInput input = readProgramInputFromFile(inputPath);

    std::random_device rd;
    std::mt19937 twister(rd());

    int totalParticles = static_cast<int>(input.proportion * input.gridWidth * input.gridHeight);
    auto system = makeCellSystem(input.gridWidth, input.gridHeight, input.proportion, input.initialTemperature, twister);
    int energy = -countContancts(system);

    float aspect = static_cast<float>(input.winWidth) / static_cast<float>(input.winHeight);

    GLFWwindow* window = initWindow(input.winWidth, input.winHeight, input.vsync, input.debug);

    void* callbackRes[2] = { &system.temperature, &input.metropolisStepsPerFrame };
    glfwSetWindowUserPointer(window, callbackRes);

    glfwSetScrollCallback(window, [] (GLFWwindow* window, double xoff, double yoff) {
        void** callbackRes = reinterpret_cast<void**>(glfwGetWindowUserPointer(window));
        float* systemTemp = reinterpret_cast<float*>(callbackRes[0]);
        if (yoff >= 0.) {
            *systemTemp += 0.025f;
        } else {
            *systemTemp -= 0.025f;
            if (*systemTemp <= 0.005f) *systemTemp = 0.025f;
        }
    });

    glfwSetKeyCallback(window, [] (GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action != GLFW_PRESS) return;

        void** callbackRes = reinterpret_cast<void**>(glfwGetWindowUserPointer(window));
        int* trials = reinterpret_cast<int*>(callbackRes[1]);

        if (key == GLFW_KEY_UP)
            *trials += 2500;
        else if (key == GLFW_KEY_DOWN) {
            *trials -= 2500;
            if (*trials < 0)
                *trials = 0;
        }
    });


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto textsheetData = loadTextsheetData(input.textsheetPath.c_str(), input.textsheetWidth, input.textsheetHeight);
    TextRenderResources textRes;
    if (!loadTextResources(
        input.textVertPath.c_str(), input.textFragPath.c_str(),
        textsheetData, input.textsheetWidth, input.textsheetHeight,
        input.textsheetNumChars, aspect, textRes
    )) return 1;

    GridRenderResources gridRes;
    if (!makeGrid(input.gridWidth, input.gridHeight, input.gridVertPath.c_str(), input.gridFragPath.c_str(), gridRes))
        return 1;


    float plotLeft = -1.f + input.textMargin + 11 * input.textWidth / aspect;
    float plotTop = -1.f + input.textMargin + 8 * input.textWidth;
    float plotWidth = 1.f - input.textMargin - plotLeft;
    float plotHeight = plotTop - (-1.f + input.textMargin);
    PlotRenderResources plotRes;
    if (!makePlot(
        input.plotVertPath.c_str(), input.plotFragPath.c_str(),
        plotLeft, plotTop, plotWidth / (input.plotLength - 1), plotHeight / 2.0, 0.01f, input.plotLength, plotRes
    )) return 1;

    auto lastSecondTime = std::chrono::steady_clock::now();
    int framesSinceLastSecond = 0;
    int fps = 0;

    int framesUntilPlotRecords = input.plotSkip;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, 1);

        for (int i = 0; i < input.metropolisStepsPerFrame; ++i)
            energy += metropolisStep(system, twister);
        subGridData(gridRes, system.cells.data());
        drawGrid(gridRes);


        float energyPerParticle = static_cast<float>(energy) / totalParticles;

        if (framesUntilPlotRecords <= 0) {
            appendPlotValue(plotRes, energyPerParticle);
            framesUntilPlotRecords = input.plotSkip;
        }
        drawPlot(plotRes);
        --framesUntilPlotRecords;

        char textBuffer[64];

        sprintf(textBuffer, "T = %.3f", system.temperature);
        drawText(textRes, textBuffer, -1.f + input.textMargin / aspect,
            1.f - input.textMargin, input.textWidth);

        sprintf(textBuffer, "E = %.3f", energyPerParticle);
        drawText(textRes, textBuffer, -1.f + input.textMargin / aspect,
            -1.f + input.textMargin + 2 * input.textWidth, input.textWidth);

        int fpsTextLength = sprintf(textBuffer, "%i", fps);
        drawText(textRes, textBuffer, 1.f - (input.textMargin + fpsTextLength * input.textWidth) / aspect,
            1.f - input.textMargin, input.textWidth);

        int trialsTextLength = sprintf(textBuffer, "%i", input.metropolisStepsPerFrame);
        drawText(textRes, textBuffer, 1.f - (input.textMargin + trialsTextLength * input.textWidth) / aspect,
            1.f - input.textMargin - 2 * input.textWidth, input.textWidth);

        glfwSwapBuffers(window);
        ++framesSinceLastSecond;
        auto currentTime = std::chrono::steady_clock::now();
        auto timeSinceLastSecond = std::chrono::duration_cast<std::chrono::duration<double>>(
            currentTime - lastSecondTime
        );

        if (timeSinceLastSecond.count() >= 1.f) {
            lastSecondTime = currentTime;
            fps = framesSinceLastSecond;
            framesSinceLastSecond = 0;
        }
    }

    int actualEnergy = -countContancts(system);
    if (energy != actualEnergy) {
        printf("Energy deviation! Calculated %i != actual %i (%+i)\n",
            energy, actualEnergy, energy - actualEnergy);
    }

    glfwTerminate();
}
