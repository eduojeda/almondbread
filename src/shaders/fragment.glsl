#version 330 core

in vec2 TexCoord;
out vec4 fragColor;

uniform vec2 base;
uniform vec2 delta;
uniform int maxIterations;

int mandelbrot(float cRe, float cIm, int maxIter);
vec4 getColor(int iterations, int maxIterations);

void main() {
    float re = base.x + delta.x * gl_FragCoord.x;
    float im = base.y + delta.y * gl_FragCoord.y;
    int iterations = mandelbrot(re, im, maxIterations);

    fragColor = getColor(iterations, maxIterations);
};

vec4 getColor(int iterations, int maxIterations) {
    if (iterations == maxIterations) {
        return vec4(0.0, 0.0, 0.0, 1.0);
    }

    float mod = float(iterations) / float(maxIterations);
    return vec4(mod, mod, mod, 1.0);
}

int mandelbrot(float cRe, float cIm, int maxIter) {
    float zRe = 0.0;
    float zIm = 0.0;
    float zReSq = 0.0;
    float zImSq = 0.0;

    for (int iter = 0 ; iter <= maxIter ; iter++) {
        zReSq = zRe * zRe;
        zImSq = zIm * zIm;
        if (zReSq + zImSq >= 4) {
            return iter;
        }

        zIm = zRe * zIm;
        zIm += zIm; // same as x2, multiplication is expensive
        zIm += cIm;
        zRe = zReSq - zImSq + cRe;
    }

    return maxIter;
}