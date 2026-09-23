#ifndef COLORIZER_H
#define COLORIZER_H

#include <vector>

#include <glad/glad.h>

#include "render_target.h"
#include "shader_program.h"

// Colors the iteration counts the fractal was rendered to with a color scheme. Keeping counts
// rather than colors means switching schemes is instant, with no re-render of deep views.
class Colorizer {
public:
    Colorizer();
    ~Colorizer();

    void draw(const RenderTarget& image, int maxIterations, int scheme, int screenWidth, int screenHeight);

private:
    ShaderProgram* shaderProgram_;
    GLuint VAO_ = 0;
    std::vector<GLuint> paletteTextures_;
};

#endif
