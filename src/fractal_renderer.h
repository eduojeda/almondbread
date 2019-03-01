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
    const int COLOR_DEPTH = 3;

    int width_, height_;
    GLuint VBO_, VAO_, EBO_;
    ShaderProgram* shaderProgram_;
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

    int mandelbrot(double cRe, double cIm, int maxIter);
    void setFragmentShaderParams(double originRe, double originIm, double range, int maxIterations);
    void renderToTexture(double originRe, double originIm, double range, int maxIterations);
    void renderLinesToBuffer(GLubyte* buffer, int fromLine, int toLine, double originRe, double originIm, double range, int maxIterations);
    void setColor(GLubyte* pixel, int iterations, int maxIterations);
};

#endif