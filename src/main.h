#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "fractal_renderer.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void errorCallback(int error, const char* description);
GLFWwindow* initGLWindow();

const int VIEWPORT_WIDTH = 900;
const int VIEWPORT_HEIGHT = 900;