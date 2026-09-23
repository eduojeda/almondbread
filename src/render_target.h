#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <glad/glad.h>

// An offscreen image of escape iteration counts that the fractal renders into, one float per pixel
// with -1 for points that never escaped. Deep frames can take a long time to render, so the fractal
// is rendered only when the view changes, and each displayed frame colors this image (see
// Colorizer) before drawing the overlay on top.
class RenderTarget {
public:
    RenderTarget(int width, int height);
    ~RenderTarget();

    // Directs rendering into the image, with the viewport covering all of it.
    void bind();

    GLuint getTexture() const;
    int getWidth() const;
    int getHeight() const;

private:
    int width_, height_;
    GLuint framebuffer_ = 0, texture_ = 0;
};

#endif
