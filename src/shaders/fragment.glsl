#version 400 core

out vec4 fragColor;

in vec2 TexCoord;
uniform sampler2D texture1;

uniform bool gpuMode;
uniform dvec2 start;
uniform dvec2 delta;
uniform int maxIterations;

int mandelbrot(double cRe, double cIm, int maxIter);
vec4 getColor(int iterations, int maxIterations);

void main() {
    if (gpuMode) {
        double re = start.x + delta.x * gl_FragCoord.x;
        double im = start.y + delta.y * gl_FragCoord.y;
        int iterations = mandelbrot(re, im, maxIterations);

        fragColor = getColor(iterations, maxIterations);
    } else {
        fragColor = texture(texture1, TexCoord);
    }
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
        if (zReSq + zImSq >= 4.0) {
            return iter;
        }

        zIm = zRe * zIm;
        zIm += zIm; // same as x2, multiplication is expensive
        zIm += cIm;
        zRe = zReSq - zImSq + cRe;
    }

    return maxIter;
}