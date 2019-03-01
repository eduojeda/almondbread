#version 400 core

in vec2 TexCoord;
out vec4 fragColor;

uniform dvec2 base;
uniform dvec2 delta;
uniform int maxIterations;

int mandelbrot(double cRe, double cIm, int maxIter);
vec4 getColor(int iterations, int maxIterations);

void main() {
    double re = base.x + delta.x * gl_FragCoord.x;
    double im = base.y + delta.y * gl_FragCoord.y;
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

int mandelbrot(double cRe, double cIm, int maxIter) {
    double zRe = 0.0;
    double zIm = 0.0;
    double zReSq = 0.0;
    double zImSq = 0.0;

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