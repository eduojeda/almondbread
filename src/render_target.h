#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <glad/glad.h>

// An offscreen image the fractal renders into. Deep frames can take a long time to render, so the
// fractal is rendered only when the view changes, and each displayed frame copies this image to
// the screen before drawing the overlay on top.
class RenderTarget {
public:
    RenderTarget(int width, int height);
    ~RenderTarget();

    // Directs rendering into the image, with the viewport covering all of it.
    void bind();
    void blitToScreen(int screenWidth, int screenHeight);

private:
    int width_, height_;
    GLuint framebuffer_ = 0, texture_ = 0;
};

#endif
