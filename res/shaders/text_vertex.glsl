#version 400 core

// Positions arrive in font units measured from the top-left, with y pointing down.
layout (location = 0) in vec2 position;

uniform vec2 framebufferSize;
uniform vec2 offset;
uniform float scale;

void main() {
    vec2 pixel = offset + position * scale;
    gl_Position = vec4(pixel.x / framebufferSize.x * 2.0 - 1.0, 1.0 - pixel.y / framebufferSize.y * 2.0, 0.0, 1.0);
}
