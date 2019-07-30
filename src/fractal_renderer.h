#ifndef FRACTAL_RENDERER_H
#define FRACTAL_RENDERER_H

using namespace std;

#include <cmath>
#include <vector>
#include <thread>
#include <complex>

#include "shader_program.h"
#include "param_input.h"
#include "rainbow_palette.h"

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
    GLuint screenTexture_, paletteTexture_;
    ShaderProgram* shaderProgram_;
    GLubyte* textureImgBuffer_;

    void initializeShaders();
    void initializeScreenQuad();
    void initializeScreenTexture();
    void initializePaletteTexture(const char* path);

    int mandelbrot(double cRe, double cIm, int maxIter);
    void setFragmentShaderParams(complex<double> start, complex<double> delta, complex<double> cursorCoords, int maxIterations);
    void renderToTexture(complex<double> start, complex<double> delta, int maxIterations);
    void renderLinesToBuffer(GLubyte* buffer, int fromLine, int toLine, complex<double> start, complex<double> delta, int maxIterations);
    void setColor(GLubyte* pixel, int iterations, int maxIterations);
};

#endif