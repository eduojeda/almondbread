#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>

#include "param_input.h"
#include "shader_program.h"

enum ShaderPrecision { PRECISION_FLOAT, PRECISION_DOUBLE_FLOAT, PRECISION_DOUBLE, PRECISION_ARBITRARY };

// PRECISION_ARBITRARY renders by perturbation against an arbitrary-precision reference orbit, so zoom
// depth is limited only by how long you are willing to wait for a frame. The other modes compute each
// pixel directly in the shader and stop resolving detail at about 1e4x (float), 1e11x (double-float)
// and 1e12x (double) zoom. Apple's OpenGL runs on Metal, which has no hardware doubles, so
// PRECISION_DOUBLE falls back to software rendering there.
// Override with -DALMONDBREAD_PRECISION=PRECISION_FLOAT etc. to compare modes.
#ifndef ALMONDBREAD_PRECISION
#define ALMONDBREAD_PRECISION PRECISION_ARBITRARY
#endif
const ShaderPrecision SHADER_PRECISION = ALMONDBREAD_PRECISION;

class Renderer {
public:
    Renderer(int viewportWidth, int viewportHeight);
    virtual ~Renderer();
    virtual void draw(ParamInput& input) = 0;

protected:
    int width_, height_;

    void initializeScreenQuad();
    void drawScreenQuad();

private:
    GLuint VBO_ = 0, VAO_ = 0, EBO_ = 0;
};

#endif
