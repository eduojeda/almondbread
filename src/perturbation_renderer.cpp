#include "perturbation_renderer.h"

#include <algorithm>
#include <cmath>

PerturbationRenderer::PerturbationRenderer(int viewportWidth, int viewportHeight): Renderer(viewportWidth, viewportHeight) {
    cout << "Shader precision: arbitrary (perturbation)" << endl;
    shaderProgram_ = new ShaderProgram("res/shaders/vertex.glsl", "res/shaders/perturbation.glsl");
    shaderProgram_->link();
    shaderProgram_->use();

    initializeScreenQuad();
    initializePaletteTexture(shaderProgram_, "res/palette.png");
    glUniform1i(glGetUniformLocation(shaderProgram_->getId(), "referenceOrbit"), 1);

    glGenBuffers(1, &orbitBuffer_);
    glGenTextures(1, &orbitTexture_);
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxOrbitLength_);
}

PerturbationRenderer::~PerturbationRenderer() {
    glDeleteTextures(1, &orbitTexture_);
    glDeleteBuffers(1, &orbitBuffer_);
    delete shaderProgram_;
}

void PerturbationRenderer::draw(ParamInput& input) {
    FloatExp range = input.getRange();
    FloatExp spacingX = range / width_;
    FloatExp spacingY = range / height_;
    int maxIterations = std::min(input.getMaxIters(), maxOrbitLength_ - 1);

    updateReferenceOrbit(input.getOrigin(), width_ >= height_ ? spacingX : spacingY, maxIterations);

    BigComplex centerOffset = input.getOrigin() - orbit_.getC();
    float pixelOffsetX = (float) ((FloatExp(centerOffset.re) / spacingX).toDouble() - width_ / 2.0);
    float pixelOffsetY = (float) ((FloatExp(centerOffset.im) / spacingY).toDouble() - height_ / 2.0);
    int spacingExponent = std::max(spacingX.exponent, spacingY.exponent);

    GLuint program = shaderProgram_->getId();
    shaderProgram_->use();
    glUniform2f(glGetUniformLocation(program, "pixelOffset"), pixelOffsetX, pixelOffsetY);
    glUniform2f(glGetUniformLocation(program, "spacingMantissa"),
        (float) std::ldexp(spacingX.mantissa, spacingX.exponent - spacingExponent),
        (float) std::ldexp(spacingY.mantissa, spacingY.exponent - spacingExponent));
    glUniform1i(glGetUniformLocation(program, "spacingExponent"), spacingExponent);
    glUniform1i(glGetUniformLocation(program, "maxIterations"), maxIterations);
    glUniform1i(glGetUniformLocation(program, "referenceLast"), orbit_.getLastIndex());

    bindPaletteTexture();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, orbitTexture_);

    // Deep views take seconds per frame. Drawing in bands that are each submitted on their own keeps
    // every GPU command short, so the operating system's GPU watchdog does not kill the context.
    int bands = std::max(1, std::min(height_, maxIterations / 1000));
    glEnable(GL_SCISSOR_TEST);
    for (int i = 0; i < bands; i++) {
        int bottom = height_ * i / bands;
        int top = height_ * (i + 1) / bands;
        glScissor(0, bottom, width_, top - bottom);
        drawScreenQuad();
        glFlush();
    }
    glDisable(GL_SCISSOR_TEST);
}

void PerturbationRenderer::updateReferenceOrbit(const BigComplex& center, FloatExp spacing, int maxIterations) {
    int neededFracLimbs = fracLimbsForStep(spacing.log2());
    bool recompute = !hasOrbit_ || orbit_.getFracLimbs() < neededFracLimbs;
    if (!recompute) {
        // Any point works as a reference, but keeping it inside the view keeps pixel offsets small.
        BigComplex offset = center - orbit_.getC();
        double x = std::fabs((FloatExp(offset.re) / spacing).toDouble());
        double y = std::fabs((FloatExp(offset.im) / spacing).toDouble());
        recompute = x > width_ / 2.0 || y > height_ / 2.0;
    }

    if (recompute) {
        // Two spare limbs (64 bits) let the view zoom about 1e19x further before precision runs out.
        orbit_.reset(center, neededFracLimbs + 2);
        hasOrbit_ = true;
        uploadedLastIndex_ = -1;
    }

    orbit_.extendTo(maxIterations);
    if (orbit_.getLastIndex() != uploadedLastIndex_) {
        uploadOrbit();
    }
}

void PerturbationRenderer::uploadOrbit() {
    const vector<float>& texels = orbit_.getTexels();
    glBindBuffer(GL_TEXTURE_BUFFER, orbitBuffer_);
    glBufferData(GL_TEXTURE_BUFFER, texels.size() * sizeof(float), texels.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, orbitTexture_);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, orbitBuffer_);
    uploadedLastIndex_ = orbit_.getLastIndex();
}
