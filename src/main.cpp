#include "main.h"
#include "shader_loader.h"

int main() {
    GLFWwindow* window = initGLWindow();

    ShaderLoader ShaderLoader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
    int shaderProgramId = ShaderLoader.createProgram();

    float quadVertices[] = {
        // positions         // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };

    unsigned int quadIndices[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int texture;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GLubyte *textureData = generateTextureImageData(SCREEN_WIDTH, SCREEN_HEIGHT, 3);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgramId);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(textureData);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

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

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Almondbread", NULL, NULL);
    if (window == NULL) {
        std::cout << "ERROR: Failed to create GLFW window." << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetErrorCallback(errorCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR: Failed to initialize GLAD." << std::endl;
        exit(-1);
    }

    return window;
}

inline int mandelbrot(double cRe, double cIm, int maxModSq, int maxIter) {
    double zRe = 0.0;
    double zIm = 0.0;
    double zReSq = 0.0;
    double zImSq = 0.0;
    int iter = 0;

    while (zReSq + zImSq < maxModSq && iter < maxIter) {
        zReSq = zRe*zRe;
        zImSq = zIm*zIm;
        zIm = 2.0 * zRe * zIm + cIm;
        zRe = zReSq - zImSq + cRe;
        iter++;
    }

    return iter;
}

GLubyte *generateTextureImageData(int width, int height, int depth) {
    GLubyte *img = (GLubyte *)malloc(sizeof(GLubyte) * depth * width * height);

    memset(img, 0x00, sizeof(GLubyte) * depth * width * height);

    int iters = 0;
    double range = 0.05;
    double originRe = -1.4;
    double originIm = 0.0;
    int maxIters = 10 / range;

    double cReInitial = originRe - range / 2.0;
    double cRe = cReInitial;
    double cIm = originIm - range / 2.0;
    double cReDelta = range / width;
    double cImDelta = range / height;

    int brightness, offset;

    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0 ; y < height ; y++, cIm += cImDelta) {
        cRe = cReInitial;
        for (int x = 0 ; x < width ; x++, cRe += cReDelta) {
            iters = mandelbrot(cRe, cIm, 4, maxIters);

            brightness = (int)((float)iters / maxIters * 255);
            offset = (y * width + x) * depth;
            img[offset] = brightness;
            img[offset+1] = brightness;
            img[offset+2] = brightness;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Texture: " << elapsed.count() << " ms" << std::endl;

    return (GLubyte *)img;
}

void errorCallback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}