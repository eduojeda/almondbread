#ifndef PERTURBATION_RENDERER_H
#define PERTURBATION_RENDERER_H

#include <cmath>

#include "bilinear_approximation.h"
#include "renderer.h"
#include "reference_orbit.h"

// Renders at any zoom depth: one reference orbit is computed on the CPU at arbitrary precision, and
// the GPU iterates each pixel's small offset from it (see res/shaders/perturbation.glsl).
class PerturbationRenderer : public Renderer {
public:
    PerturbationRenderer(int viewportWidth, int viewportHeight);
    ~PerturbationRenderer();
    void draw(ParamInput& input) override;

private:
    ShaderProgram* shaderProgram_;
    GLuint orbitBuffer_ = 0, orbitTexture_ = 0;
    GLuint blaBuffer_ = 0, blaTexture_ = 0;
    int maxOrbitLength_;
    ReferenceOrbit orbit_;
    BilinearApproximation bla_;
    // Relative error each skipped iteration may add; about float's own rounding error.
    double blaEpsilon_ = std::ldexp(1.0, -24);
    bool hasOrbit_ = false;
    int uploadedLastIndex_ = -1;
    int orbitResets_ = 0;
    int blaOrbitResets_ = -1;
    int blaLastIndex_ = 0;
    int blaLog2MaxDc_ = 0;

    void updateReferenceOrbit(const BigComplex& center, FloatExp spacing, int maxIterations);
    void uploadOrbit();
    void updateBilinearApproximation(double log2MaxDc);
    static void uploadTexels(GLuint buffer, GLuint texture, GLenum unit, const vector<float>& texels);
};

#endif
