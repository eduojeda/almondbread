#include "main.h"

int main() {
    GLFWwindow* window = initializeGLWindow();
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

GLFWwindow* initializeGLWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, "Almondbread", NULL, NULL);
    if (window == NULL) {
        cerr << "Failed to create GLFW window." << endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetErrorCallback(errorCallback);

    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD." << endl;
        exit(-1);
    }

    cout << "GL_VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "GL_RENDERER: " << glGetString(GL_RENDERER) << endl;

    return window;
}

void errorCallback(int error, const char* description) {
    cerr << description << endl;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}