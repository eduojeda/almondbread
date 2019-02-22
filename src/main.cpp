#include "main.h"

int main() {
    GLFWwindow* window = initGLWindow();
    ParamInput paramInput(window);
    FractalRenderer renderer(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    while (!glfwWindowShouldClose(window)) {
        //glfwWaitEvents(); //instead of changed_?
        if (paramInput.hasChanged()) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            renderer.draw(paramInput);

            glfwSwapBuffers(window);
        }

        glfwPollEvents();
        paramInput.update();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow* initGLWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, "Almondbread", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetErrorCallback(errorCallback);

    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD." << std::endl;
        exit(-1);
    }

    return window;
}

void errorCallback(int error, const char* description) {
    std::cerr << description << std::endl;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}