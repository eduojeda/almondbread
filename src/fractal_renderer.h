#ifndef FRACTAL_RENDERER_H
#define FRACTAL_RENDERER_H

using namespace std;

#include <cmath>
#include <complex>

#include "renderer.h"

// Computes every pixel's orbit directly in the shader, at the precision SHADER_PRECISION selects.
class FractalRenderer : public Renderer {
public:
    FractalRenderer(int viewportWidth, int viewportHeight);
    ~FractalRenderer();
    void draw(ParamInput& input) override;

private:
    ShaderProgram* shaderProgram_;

    void initializeShaders();
    void setFragmentShaderParams(complex<double> start, complex<double> delta, complex<double> cursorCoords, int maxIterations);
    void setComplexUniform(const char* name, complex<double> value);
};

#endif
