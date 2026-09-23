#include "perturbation_renderer.h"

#include <algorithm>
#include <cmath>

namespace {

const int MAX_BLA_LEVELS = 32;

}

PerturbationRenderer::PerturbationRenderer(int viewportWidth, int viewportHeight): Renderer(viewportWidth, viewportHeight) {
    cout << "Shader precision: arbitrary (perturbation)" << endl;
    shaderProgram_ = new ShaderProgram("res/shaders/vertex.glsl", "res/shaders/perturbation.glsl");
    shaderProgram_->link();
    shaderProgram_->use();

    initializeScreenQuad();
    glUniform1i(glGetUniformLocation(shaderProgram_->getId(), "referenceOrbit"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram_->getId(), "blaTable"), 2);

    glGenBuffers(1, &orbitBuffer_);
    glGenTextures(1, &orbitTexture_);
    glGenBuffers(1, &blaBuffer_);
    glGenTextures(1, &blaTexture_);
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxOrbitLength_);
}

PerturbationRenderer::~PerturbationRenderer() {
    glDeleteTextures(1, &blaTexture_);
    glDeleteBuffers(1, &blaBuffer_);
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

    // The farthest pixel from the reference bounds |dc| for every pixel.
    double farthestX = std::max(std::fabs(pixelOffsetX), std::fabs(pixelOffsetX + width_));
    double farthestY = std::max(std::fabs(pixelOffsetY), std::fabs(pixelOffsetY + height_));
    double log2Spacing = std::max(spacingX.log2(), spacingY.log2());
    updateBilinearApproximation(0.5 * std::log2(farthestX * farthestX + farthestY * farthestY) + log2Spacing);

    GLuint program = shaderProgram_->getId();
    shaderProgram_->use();
    glUniform2f(glGetUniformLocation(program, "pixelOffset"), pixelOffsetX, pixelOffsetY);
    glUniform2f(glGetUniformLocation(program, "spacingMantissa"),
        (float) std::ldexp(spacingX.mantissa, spacingX.exponent - spacingExponent),
        (float) std::ldexp(spacingY.mantissa, spacingY.exponent - spacingExponent));
    glUniform1i(glGetUniformLocation(program, "spacingExponent"), spacingExponent);
    glUniform1i(glGetUniformLocation(program, "maxIterations"), maxIterations);
    glUniform1i(glGetUniformLocation(program, "referenceLast"), orbit_.getLastIndex());

    int levels = std::min(bla_.getLevelCount(), MAX_BLA_LEVELS);
    glUniform1i(glGetUniformLocation(program, "blaLevels"), levels);
    glUniform1i(glGetUniformLocation(program, "blaLast"), blaLastIndex_);
    if (levels > 0) {
        glUniform1iv(glGetUniformLocation(program, "blaLevelStart"), levels, bla_.getLevelStarts().data());
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, orbitTexture_);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_BUFFER, blaTexture_);

    // Deep views can take seconds per frame. Drawing in bands that are each submitted on their own
    // keeps every GPU command short, so the operating system's GPU watchdog does not kill the context.
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
        orbitResets_++;
    }

    orbit_.extendTo(maxIterations);
    if (orbit_.getLastIndex() != uploadedLastIndex_) {
        uploadOrbit();
    }
}

void PerturbationRenderer::uploadOrbit() {
    uploadTexels(orbitBuffer_, orbitTexture_, GL_TEXTURE1, orbit_.getTexels());
    uploadedLastIndex_ = orbit_.getLastIndex();
}

// The table depends on the orbit and on the largest |dc| in view. Rounding that bound up to a power
// of two keeps it valid while the view shrinks, and since the orbit only ever grows at the end, a
// table built for a shorter orbit stays valid too. So a plain zoom rebuilds it only every few dozen
// frames instead of on every one.
void PerturbationRenderer::updateBilinearApproximation(double log2MaxDc) {
    int bound = (int) std::ceil(log2MaxDc);
    int lastIndex = orbit_.getLastIndex();
    if (blaOrbitResets_ == orbitResets_ && blaLog2MaxDc_ == bound && lastIndex <= blaLastIndex_ + blaLastIndex_ / 8) {
        return;
    }

    bla_.build(orbit_.getTexels(), lastIndex, bound, blaEpsilon_);
    blaOrbitResets_ = orbitResets_;
    blaLastIndex_ = lastIndex;
    blaLog2MaxDc_ = bound;
    // A buffer texture without storage is invalid to have bound even when the shader never reads it.
    const vector<float> emptyNode(8, 0.0f);
    uploadTexels(blaBuffer_, blaTexture_, GL_TEXTURE2, bla_.getLevelCount() > 0 ? bla_.getTexels() : emptyNode);
}

void PerturbationRenderer::uploadTexels(GLuint buffer, GLuint texture, GLenum unit, const vector<float>& texels) {
    glBindBuffer(GL_TEXTURE_BUFFER, buffer);
    glBufferData(GL_TEXTURE_BUFFER, texels.size() * sizeof(float), texels.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_BUFFER, texture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffer);
}
