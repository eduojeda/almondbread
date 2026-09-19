#version 400 core

// FRACTAL_DOUBLE_PRECISION is defined by the host when the GPU has hardware double precision.
#ifdef FRACTAL_DOUBLE_PRECISION
#define real double
#define real2 dvec2
#else
#define real float
#define real2 vec2
#endif

out vec4 fragColor;

uniform sampler1D paletteTexture;
uniform real2 start;
uniform real2 delta;
uniform real2 cursorCoords;
uniform int maxIterations;

int mandelbrot(real zRe, real zIm, real cRe, real cIm, int maxIter);
vec4 getColor(int iterations, int maxIterations);

void main() {
    real re = start.x + delta.x * gl_FragCoord.x;
    real im = start.y + delta.y * gl_FragCoord.y;

    int iterations = mandelbrot(0.0, 0.0, re, im, maxIterations); // Mandelbrot
    //int iterations = mandelbrot(re, im, cursorCoords.x, cursorCoords.y, maxIterations); // Julia

    fragColor = getColor(iterations, maxIterations);
}

vec4 getColor(int iterations, int maxIterations) {
    if (iterations == maxIterations) {
        return texture(paletteTexture, 1.0);
    }

    float norm = float(iterations) / float(maxIterations);
    return texture(paletteTexture, norm);
}

int mandelbrot(real zRe, real zIm, real cRe, real cIm, int maxIter) {
    real zReSq = 0.0;
    real zImSq = 0.0;

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

int burningShip(real zRe, real zIm, real cRe, real cIm, int maxIter) {
    real zReSq = 0.0;
    real zImSq = 0.0;

    for (int iter = 0 ; iter <= maxIter ; iter++) {
        zReSq = zRe * zRe;
        zImSq = zIm * zIm;
        if (zReSq + zImSq >= 4.0) {
            return iter;
        }

        zIm = zRe * zIm;
        zIm += zIm; // same as x2, multiplication is expensive
        zIm = abs(zIm + cIm);
        zRe = abs(zReSq - zImSq + cRe);
    }

    return maxIter;
}
