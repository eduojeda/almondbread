#ifndef PERTURBATION_RENDERER_H
#define PERTURBATION_RENDERER_H

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
    int maxOrbitLength_;
    ReferenceOrbit orbit_;
    bool hasOrbit_ = false;
    int uploadedLastIndex_ = -1;

    void updateReferenceOrbit(const BigComplex& center, FloatExp spacing, int maxIterations);
    void uploadOrbit();
};

#endif
