#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void errorCallback(int error, const char *description);
int setUpShaderProgram();
GLFWwindow* initGLWindow();

const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 768;

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main() {\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main() {\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\0";
