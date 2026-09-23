#include "text_overlay.h"

#include <algorithm>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#include <stb_easy_font.h>
#pragma GCC diagnostic pop

namespace {

// stb_easy_font's vertex layout: x, y, z as floats, then four color bytes.
struct FontVertex {
    float x, y, z;
    unsigned char color[4];
};

const float LINE_HEIGHT = 12.0f;
const float GLYPH_HEIGHT = 9.0f;
const float PADDING = 3.0f;

void appendQuad(std::vector<FontVertex>& vertices, float left, float top, float right, float bottom) {
    vertices.push_back({left, top, 0.0f, {0, 0, 0, 0}});
    vertices.push_back({right, top, 0.0f, {0, 0, 0, 0}});
    vertices.push_back({right, bottom, 0.0f, {0, 0, 0, 0}});
    vertices.push_back({left, bottom, 0.0f, {0, 0, 0, 0}});
}

}

TextOverlay::TextOverlay() {
    shaderProgram_ = new ShaderProgram("res/shaders/text_vertex.glsl", "res/shaders/text_fragment.glsl");
    shaderProgram_->link();

    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);
    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), (void*) 0);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &EBO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBindVertexArray(0);
}

TextOverlay::~TextOverlay() {
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteBuffers(1, &EBO_);
    delete shaderProgram_;
}

void TextOverlay::draw(const std::vector<std::string>& lines, int framebufferWidth, int framebufferHeight, float scale, float margin) {
    if (lines.empty()) {
        return;
    }

    // The backdrop is the first quad, the glyphs follow. Positions are in font units from the top-left.
    std::vector<FontVertex> vertices;
    appendQuad(vertices, 0.0f, 0.0f, 0.0f, 0.0f);
    float widest = 0.0f;
    std::vector<FontVertex> lineVertices;
    for (size_t i = 0; i < lines.size(); i++) {
        std::vector<char> text(lines[i].begin(), lines[i].end());
        text.push_back('\0');
        widest = std::max(widest, (float) stb_easy_font_width(text.data()));

        // stb_easy_font averages about 17 vertices per character; the busiest glyphs need more.
        lineVertices.resize(text.size() * 64);
        int quads = stb_easy_font_print(PADDING, PADDING + i * LINE_HEIGHT, text.data(), NULL,
            lineVertices.data(), (int) (lineVertices.size() * sizeof(FontVertex)));
        vertices.insert(vertices.end(), lineVertices.begin(), lineVertices.begin() + 4 * quads);
    }
    float bottom = PADDING + (lines.size() - 1) * LINE_HEIGHT + GLYPH_HEIGHT + PADDING;
    vertices[1].x = vertices[2].x = widest + 2.0f * PADDING;
    vertices[2].y = vertices[3].y = bottom;

    int quads = (int) vertices.size() / 4;
    ensureIndices(quads);

    shaderProgram_->use();
    GLuint program = shaderProgram_->getId();
    glUniform2f(glGetUniformLocation(program, "framebufferSize"), (float) framebufferWidth, (float) framebufferHeight);
    glUniform2f(glGetUniformLocation(program, "offset"), margin, margin);
    glUniform1f(glGetUniformLocation(program, "scale"), scale);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(FontVertex), vertices.data(), GL_STREAM_DRAW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLint color = glGetUniformLocation(program, "color");
    glUniform4f(color, 0.0f, 0.0f, 0.0f, 0.6f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*) 0);
    glUniform4f(color, 1.0f, 1.0f, 1.0f, 1.0f);
    glDrawElements(GL_TRIANGLES, 6 * (quads - 1), GL_UNSIGNED_INT, (void*) (6 * sizeof(GLuint)));
    glDisable(GL_BLEND);
    glBindVertexArray(0);
}

void TextOverlay::ensureIndices(int quads) {
    if (quads <= indexedQuads_) {
        return;
    }

    indexedQuads_ = std::max(quads, 2 * indexedQuads_);
    std::vector<GLuint> indices;
    indices.reserve(6 * indexedQuads_);
    for (GLuint q = 0; q < (GLuint) indexedQuads_; q++) {
        indices.insert(indices.end(), {4 * q, 4 * q + 1, 4 * q + 2, 4 * q, 4 * q + 2, 4 * q + 3});
    }
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}
