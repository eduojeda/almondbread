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

enum ShaderPrecision { PRECISION_FLOAT, PRECISION_DOUBLE_FLOAT, PRECISION_DOUBLE };

// Apple's OpenGL runs on top of Metal, which has no hardware double precision: a double-precision
// shader silently falls back to software rendering there. Double-float emulation (a pair of floats
// per value, ~48 bits of mantissa) keeps rendering on the GPU with most of the zoom depth.
// Override with -DALMONDBREAD_PRECISION=PRECISION_FLOAT etc. to compare modes.
#ifndef ALMONDBREAD_PRECISION
#ifdef __APPLE__
#define ALMONDBREAD_PRECISION PRECISION_DOUBLE_FLOAT
#else
#define ALMONDBREAD_PRECISION PRECISION_DOUBLE
#endif
#endif
const ShaderPrecision SHADER_PRECISION = ALMONDBREAD_PRECISION;

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
