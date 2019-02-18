#include "fractal_renderer.h"

inline int FractalRenderer::mandelbrot(double cRe, double cIm, int maxModSq, int maxIter) {
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

FractalRenderer::FractalRenderer(unsigned int viewportWidth, unsigned int viewportHeight) {
    width = viewportWidth;
    height = viewportHeight;

    initShaders();
    initQuad();
    initTexture();
}

void FractalRenderer::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void FractalRenderer::initShaders() {
    ShaderLoader shaderLoader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
    glUseProgram(shaderLoader.createProgram());
}

void FractalRenderer::initQuad() {
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
}

void FractalRenderer::initTexture() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLubyte *textureData = generateTextureImageData();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
    free(textureData);
}

GLubyte *FractalRenderer::generateTextureImageData() {
    int colorDepth = 3;

    int quality = 15;

    double range = 0.02;
    double originRe = -1.4;
    double originIm = 0.0;

    double cReInitial = originRe - range / 2.0;
    double cRe = cReInitial;
    double cIm = originIm - range / 2.0;
    double cReDelta = range / width;
    double cImDelta = range / height;

    int iterations = 0;
    int maxIterations = quality / range;

    GLubyte *img = (GLubyte *)malloc(sizeof(GLubyte) * colorDepth * width * height);

    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0 ; y < height ; y++, cIm += cImDelta) {
        cRe = cReInitial;
        for (int x = 0 ; x < width ; x++, cRe += cReDelta) {
            iterations = mandelbrot(cRe, cIm, 4, maxIterations);

            int offset = (y * width + x) * colorDepth;
            setColor(&img[offset], iterations, maxIterations);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Rendering time: " << elapsed.count() << " ms" << std::endl;

    return (GLubyte *)img;
}

void FractalRenderer::setColor(GLubyte *pixel, int iterations, int maxIterations) {
    float normIters = (float)iterations / maxIterations;

    pixel[0] = (int)(pow(normIters, 0.8) * 255);
    pixel[1] = (int)(pow(normIters, 1.0) * 255);
    pixel[2] = (int)(pow(normIters, 0.5) * 255);
}

FractalRenderer::~FractalRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}