#ifndef MAIN_H
#define MAIN_H

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

#include "fractal_renderer.h"
#include "perturbation_renderer.h"
#include "param_input.h"

#ifdef _WIN32
extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

GLFWwindow* initializeGLWindow();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void errorCallback(int error, const char* description);

const int VIEWPORT_WIDTH = 900;
const int VIEWPORT_HEIGHT = 900;

#endif