#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "fractal_renderer.h"
#include "param_input.h"

GLFWwindow* initGLWindow();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void errorCallback(int error, const char* description);

const int VIEWPORT_WIDTH = 900;
const int VIEWPORT_HEIGHT = 900;