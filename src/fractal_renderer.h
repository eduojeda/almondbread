#ifndef FRACTAL_RENDERER_H
#define FRACTAL_RENDERER_H

#include <chrono>
#include <cmath>
#include "shader_loader.h"

class FractalRenderer {
private:
    unsigned int width, height;
    unsigned int VBO, VAO, EBO;
    unsigned int texture;

    float quadVertices[20] = {
        // positions         // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };

    unsigned int quadIndices[6] = {
        0, 1, 3,
        1, 2, 3
    };

    void initShaders();
    void initQuad();
    void initTexture();
    GLubyte *generateTextureImageData();
    void setColor(GLubyte *pixel, int iterations, int maxIterations);
    int mandelbrot(double cRe, double cIm, int maxModSq, int maxIter);

public:
    FractalRenderer(unsigned int viewportWidth, unsigned int viewportHeight);
    ~FractalRenderer();
    void draw();
};

#endif