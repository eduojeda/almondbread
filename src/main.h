#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void errorCallback(int error, const char *description);
int setUpShaderProgram();
GLFWwindow* initGLWindow();
GLubyte *generateTextureImageData(int width, int height, int depth);

const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 768;

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec2 aTexCoord;\n"
                                 "out vec2 TexCoord;\n"
                                 "void main() {\n"
                                 "   gl_Position = vec4(aPos, 1.0);\n"
                                 "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n" // can be done away with if in and out are named the same?
                                 "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "in vec2 TexCoord;\n"
                                   "out vec4 FragColor;\n"
                                   "uniform sampler2D texture1;\n"
                                   "void main() {\n"
                                   "   FragColor = texture(texture1, TexCoord);\n"
                                   "}\0";
