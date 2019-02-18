#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <cmath>

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void errorCallback(int error, const char *description);
GLFWwindow* initGLWindow();
GLubyte *generateTextureImageData(int width, int height);
void setColor(GLubyte *pixel, int iterations, int maxIterations);

const unsigned int SCREEN_WIDTH = 900;
const unsigned int SCREEN_HEIGHT = 900;