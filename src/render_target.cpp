#include "render_target.h"

#include <iostream>

RenderTarget::RenderTarget(int width, int height): width_(width), height_(height) {
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Offscreen framebuffer is incomplete." << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget() {
    glDeleteFramebuffers(1, &framebuffer_);
    glDeleteTextures(1, &texture_);
}

void RenderTarget::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glViewport(0, 0, width_, height_);
}

void RenderTarget::blitToScreen(int screenWidth, int screenHeight) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    GLenum filter = screenWidth == width_ && screenHeight == height_ ? GL_NEAREST : GL_LINEAR;
    glBlitFramebuffer(0, 0, width_, height_, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, filter);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenWidth, screenHeight);
}
