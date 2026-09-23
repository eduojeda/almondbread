#include "colorizer.h"

#include <algorithm>

#include "color_schemes.h"

namespace {

const int CYCLE_LENGTH = 256;

}

Colorizer::Colorizer() {
    shaderProgram_ = new ShaderProgram("res/shaders/fullscreen_vertex.glsl", "res/shaders/colorize_fragment.glsl");
    shaderProgram_->link();

    // Core profile needs a vertex array bound to draw, even one without attributes.
    glGenVertexArrays(1, &VAO_);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (const ColorScheme& scheme : colorSchemes()) {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_1D, texture);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, (GLsizei) scheme.colors.size() / 3, 0, GL_RGB, GL_UNSIGNED_BYTE, scheme.colors.data());
        paletteTextures_.push_back(texture);
    }
}

Colorizer::~Colorizer() {
    glDeleteTextures((GLsizei) paletteTextures_.size(), paletteTextures_.data());
    glDeleteVertexArrays(1, &VAO_);
    delete shaderProgram_;
}

void Colorizer::draw(const RenderTarget& image, int maxIterations, int scheme, int screenWidth, int screenHeight) {
    int count = (int) paletteTextures_.size();
    int index = ((scheme % count) + count) % count;
    const ColorScheme& colors = colorSchemeAt(scheme);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenWidth, screenHeight);

    shaderProgram_->use();
    GLuint program = shaderProgram_->getId();
    glUniform1i(glGetUniformLocation(program, "iterations"), 0);
    glUniform1i(glGetUniformLocation(program, "palette"), 1);
    glUniform3f(glGetUniformLocation(program, "interiorColor"),
        colors.interior[0] / 255.0f, colors.interior[1] / 255.0f, colors.interior[2] / 255.0f);
    glUniform1i(glGetUniformLocation(program, "cycleLength"), std::max(1, std::min(maxIterations, CYCLE_LENGTH)));
    glUniform2f(glGetUniformLocation(program, "imageSize"), (float) image.getWidth(), (float) image.getHeight());
    glUniform2f(glGetUniformLocation(program, "screenSize"), (float) screenWidth, (float) screenHeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image.getTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, paletteTextures_[index]);

    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
