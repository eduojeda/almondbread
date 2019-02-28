#ifndef FRACTAL_RENDERER_H
#define FRACTAL_RENDERER_H

#include <chrono>
#include <cmath>
#include <vector>
#include <thread>

#include "shader_program.h"
#include "param_input.h"

class FractalRenderer {
public:
    FractalRenderer(int viewportWidth, int viewportHeight);
    ~FractalRenderer();
    void draw(ParamInput& input);

private:
    const int ESCAPE_RADIUS = 4;
    const int QUALITY = 15;
    const int COLOR_DEPTH = 3;

    int width_, height_;
    GLuint VBO_, VAO_, EBO_;
    GLubyte* textureImgBuffer_;

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

    void initializeShaders();
    void initializeQuad();
    void initializeTexture();
    void generateTextureImage(GLubyte* buffer, double originRe, double originIm, double range);
    void renderLinesToBuffer(GLubyte* buffer, int fromLine, int toLine, double originRe, double originIm, double range);
    void setColor(GLubyte* pixel, int iterations, int maxIterations);
    int mandelbrot(double cRe, double cIm, int maxIter);
};

#endif