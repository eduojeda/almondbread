#include "fractal_renderer.h"

static const char* shaderPrecisionDefine() {
    switch (SHADER_PRECISION) {
        case PRECISION_DOUBLE: return "#define FRACTAL_PRECISION_DOUBLE\n";
        case PRECISION_DOUBLE_FLOAT: return "#define FRACTAL_PRECISION_DOUBLE_FLOAT\n";
        default: return "";
    }
}

static const char* shaderPrecisionName() {
    switch (SHADER_PRECISION) {
        case PRECISION_DOUBLE: return "double";
        case PRECISION_DOUBLE_FLOAT: return "double-float";
        default: return "float";
    }
}

FractalRenderer::FractalRenderer(int viewportWidth, int viewportHeight): Renderer(viewportWidth, viewportHeight) {
    initializeShaders();
    initializeScreenQuad();
    initializePaletteTexture(shaderProgram_, "res/palette.png");
}

FractalRenderer::~FractalRenderer() {
    delete shaderProgram_;
}

void FractalRenderer::draw(ParamInput& paramInput) {
    shaderProgram_->use();
    double range = paramInput.getRange().toDouble();
    const BigComplex& origin = paramInput.getOrigin();
    complex<double> start = complex<double>(origin.re.toDouble(), origin.im.toDouble()) - complex<double>(range / 2.0, range / 2.0);
    complex<double> delta = complex<double>(range / width_, range / height_);
    complex<double> cursorCoords = paramInput.getCursorCoords();

    setFragmentShaderParams(start, delta, cursorCoords, paramInput.getMaxIters());

    bindPaletteTexture();
    drawScreenQuad();
}

void FractalRenderer::setFragmentShaderParams(complex<double> start, complex<double> delta, complex<double> cursorCoords, int maxIterations) {
    setComplexUniform("start", start);
    setComplexUniform("delta", delta);
    setComplexUniform("cursorCoords", cursorCoords);
    glUniform1i(glGetUniformLocation(shaderProgram_->getId(), "maxIterations"), maxIterations);
}

void FractalRenderer::setComplexUniform(const char* name, complex<double> value) {
    int location = glGetUniformLocation(shaderProgram_->getId(), name);
    switch (SHADER_PRECISION) {
        case PRECISION_DOUBLE:
            glUniform2d(location, value.real(), value.imag());
            break;
        case PRECISION_DOUBLE_FLOAT: {
            // Each component becomes a (hi, lo) float pair; lo carries the part of the double that hi cannot hold.
            float reHi = (float) value.real();
            float imHi = (float) value.imag();
            glUniform4f(location, reHi, (float) (value.real() - reHi), imHi, (float) (value.imag() - imHi));
            break;
        }
        default:
            glUniform2f(location, (float) value.real(), (float) value.imag());
            break;
    }
}

void FractalRenderer::initializeShaders() {
    cout << "Shader precision: " << shaderPrecisionName() << endl;
    shaderProgram_ = new ShaderProgram("res/shaders/vertex.glsl", "res/shaders/fragment.glsl", shaderPrecisionDefine());
    shaderProgram_->link();
    shaderProgram_->use();
}
