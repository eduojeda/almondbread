#ifndef TEXT_OVERLAY_H
#define TEXT_OVERLAY_H

#include <string>
#include <vector>

#include <glad/glad.h>

#include "shader_program.h"

// Draws lines of ASCII text in the top-left corner over a translucent backdrop, using the
// stb_easy_font bitmap font.
class TextOverlay {
public:
    TextOverlay();
    ~TextOverlay();

    // scale multiplies the font's own units (a line is 12 units tall); margin is in framebuffer pixels.
    void draw(const std::vector<std::string>& lines, int framebufferWidth, int framebufferHeight, float scale, float margin);

private:
    ShaderProgram* shaderProgram_;
    GLuint VAO_ = 0, VBO_ = 0, EBO_ = 0;
    int indexedQuads_ = 0;

    void ensureIndices(int quads);
};

#endif
