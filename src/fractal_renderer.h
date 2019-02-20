#ifndef FRACTAL_RENDERER_H
#define FRACTAL_RENDERER_H

#include <chrono>
#include <cmath>
#include <vector>
#include <thread>
#include "shader_loader.h"

class FractalRenderer {
private:
    int width_, height_;
    double cReDelta_, cImDelta_;
    const double range_ = 0.02;
    const double originRe_ = -1.4;
    const double originIm_ = 0.0;

    const int quality_ = 15;
    const int colorDepth_ = 3;
    GLuint VBO_, VAO_, EBO_;

    float quadVertices_[20] = {
        // positions         // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };

    int quadIndices_[6] = {
        0, 1, 3,
        1, 2, 3
    };

    void initShaders();
    void initQuad();
    void initTexture();
    GLubyte* generateTextureImage();
    void renderLinesToBuffer(GLubyte* buffer, int fromLine, int toLine);
    void setColor(GLubyte* pixel, int iterations, int maxIterations);
    int mandelbrot(double cRe, double cIm, int maxModSq, int maxIter);

public:
    FractalRenderer(int viewportWidth, int viewportHeight);
    ~FractalRenderer();
    void draw();
};

#endif