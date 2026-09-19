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
#include "image_palette.h"

// Apple's OpenGL runs on top of Metal, which has no hardware double precision: a double-precision
// shader silently falls back to software rendering there. Float limits zoom depth but stays on the GPU.
#ifdef __APPLE__
const bool USE_DOUBLE_PRECISION = false;
#else
const bool USE_DOUBLE_PRECISION = true;
#endif

class FractalRenderer {
public:
    FractalRenderer(int viewportWidth, int viewportHeight);
    ~FractalRenderer();
    void draw(ParamInput& input);

private:
    int width_, height_;
    GLuint VBO_, VAO_, EBO_;
    GLuint paletteTexture_;
    ShaderProgram* shaderProgram_;

    void initializeShaders();
    void initializeScreenQuad();
    void initializePaletteTexture(const char* path);
    void setFragmentShaderParams(complex<double> start, complex<double> delta, complex<double> cursorCoords, int maxIterations);
    void setComplexUniform(const char* name, complex<double> value);
};

#endif
