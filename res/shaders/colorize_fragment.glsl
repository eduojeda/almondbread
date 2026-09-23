#version 400 core

out vec4 fragColor;

uniform sampler2D iterations;   // escape iteration per pixel, or -1 for points that never escaped
uniform sampler1D palette;
uniform vec3 interiorColor;
uniform int cycleLength;
uniform vec2 imageSize;
uniform vec2 screenSize;

void main() {
    ivec2 texel = min(ivec2(gl_FragCoord.xy * imageSize / screenSize), ivec2(imageSize) - 1);
    float n = texelFetch(iterations, texel, 0).r;
    if (n < 0.0) {
        fragColor = vec4(interiorColor, 1.0);
        return;
    }

    // Deep views need thousands of iterations but a small window of them spans the whole image, so
    // the palette repeats every cycleLength iterations instead of being stretched over all of them.
    fragColor = texture(palette, fract(n / float(cycleLength)));
}
