#include "renderer.h"

#include "rainbow_palette.h"
#include "image_palette.h"

Renderer::Renderer(int viewportWidth, int viewportHeight): width_(viewportWidth), height_(viewportHeight) {}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteBuffers(1, &EBO_);
    glDeleteTextures(1, &paletteTexture_);
}

void Renderer::initializeScreenQuad() {
    float quadVertices[20] = {
        // positions
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };

    int quadIndices[6] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);

    // vertex shader attribute: aPos
    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::initializePaletteTexture(ShaderProgram* program, const char* path) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &paletteTexture_);
    glBindTexture(GL_TEXTURE_1D, paletteTexture_);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    RainbowPalette palette(256, 3);
    // ImagePalette palette(path);
    unsigned char* data = palette.getData();

    if (data) {
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, palette.getSize(), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
        cerr << "Failed to load palette texture from " << path << endl;
    }

    glUniform1i(glGetUniformLocation(program->getId(), "paletteTexture"), 0);
}

void Renderer::bindPaletteTexture() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, paletteTexture_);
}

void Renderer::drawScreenQuad() {
    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
