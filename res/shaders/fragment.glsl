#version 400 core

out vec4 fragColor;

in vec2 TexCoord;
uniform sampler2D screenTexture;
uniform sampler1D paletteTexture;

uniform bool gpuMode;
uniform dvec2 start;
uniform dvec2 delta;
uniform dvec2 cursorCoords;
uniform int maxIterations;

int mandelbrot(double zRe, double zIm, double cRe, double cIm, int maxIter);
vec4 getColor(int iterations, int maxIterations);

void main() {
    if (gpuMode) {
        double re = start.x + delta.x * gl_FragCoord.x;
        double im = start.y + delta.y * gl_FragCoord.y;

        //int iterations = mandelbrot(0.0, 0.0, re, im, maxIterations); // Mandelbrot
        int iterations = mandelbrot(re, im, cursorCoords.x, cursorCoords.y, maxIterations); // Julia

        fragColor = getColor(iterations, maxIterations);
    } else {
        fragColor = mix(texture(paletteTexture, 0.5), texture(screenTexture, TexCoord), 0.5);
    }
};

vec4 getColor(int iterations, int maxIterations) {
    if (iterations == maxIterations) {
        return texture(paletteTexture, 0.0);
    }

    float norm = float(iterations) / float(maxIterations);
    return texture(paletteTexture, norm);
}

int mandelbrot(double zRe, double zIm, double cRe, double cIm, int maxIter) {
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