#version 400 core

// The host defines FRACTAL_PRECISION_DOUBLE or FRACTAL_PRECISION_DOUBLE_FLOAT right after the
// #version line; with neither, plain float is used. The fractal code is written against the real_*
// macros so the same iteration loop runs in every mode.

#if defined(FRACTAL_PRECISION_DOUBLE)

#define real double
#define real2 dvec2
#define real_from_float(f) double(f)
#define real_hi(a) float(a)
#define real2_x(v) (v).x
#define real2_y(v) (v).y
#define real_add(a, b) ((a) + (b))
#define real_sub(a, b) ((a) - (b))
#define real_mul(a, b) ((a) * (b))
#define real_mul_float(a, f) ((a) * double(f))
#define real_twice(a) ((a) + (a))
#define real_abs(a) abs(a)

#elif defined(FRACTAL_PRECISION_DOUBLE_FLOAT)

// Double-float arithmetic: a value is vec2(hi, lo) with |lo| <= ulp(hi) / 2, which gives about
// 48 bits of mantissa (float has 24, double 53) using only float hardware. The error-free
// transformations below (Knuth's two-sum, Dekker's split product) are exactly zero in real
// arithmetic and only capture rounding error, so the compiler must not reassociate or fuse them:
// hence `precise` on every intermediate.

vec2 df_two_sum(float a, float b) {
    precise float s = a + b;
    precise float bb = s - a;
    precise float err = (a - (s - bb)) + (b - bb);
    return vec2(s, err);
}

// Cheaper two-sum, valid only when |a| >= |b|.
vec2 df_quick_two_sum(float a, float b) {
    precise float s = a + b;
    precise float err = b - (s - a);
    return vec2(s, err);
}

// Splits a 24-bit mantissa into two 12-bit halves so their partial products are exact.
vec2 df_split(float a) {
    precise float t = 4097.0 * a;
    precise float hi = t - (t - a);
    precise float lo = a - hi;
    return vec2(hi, lo);
}

vec2 df_two_prod(float a, float b) {
    precise float p = a * b;
    vec2 as = df_split(a);
    vec2 bs = df_split(b);
    precise float err = ((as.x * bs.x - p) + as.x * bs.y + as.y * bs.x) + as.y * bs.y;
    return vec2(p, err);
}

vec2 df_add(vec2 a, vec2 b) {
    precise vec2 s = df_two_sum(a.x, b.x);
    precise vec2 t = df_two_sum(a.y, b.y);
    s.y += t.x;
    s = df_quick_two_sum(s.x, s.y);
    s.y += t.y;
    return df_quick_two_sum(s.x, s.y);
}

vec2 df_mul(vec2 a, vec2 b) {
    precise vec2 p = df_two_prod(a.x, b.x);
    p.y += a.x * b.y + a.y * b.x;
    return df_quick_two_sum(p.x, p.y);
}

vec2 df_mul_float(vec2 a, float b) {
    precise vec2 p = df_two_prod(a.x, b);
    p.y += a.y * b;
    return df_quick_two_sum(p.x, p.y);
}

#define real vec2
#define real2 vec4 // (re.hi, re.lo, im.hi, im.lo)
#define real_from_float(f) vec2(f, 0.0)
#define real_hi(a) (a).x
#define real2_x(v) (v).xy
#define real2_y(v) (v).zw
#define real_add(a, b) df_add(a, b)
#define real_sub(a, b) df_add(a, -(b))
#define real_mul(a, b) df_mul(a, b)
#define real_mul_float(a, f) df_mul_float(a, f)
#define real_twice(a) (2.0 * (a))
#define real_abs(a) ((a).x < 0.0 ? -(a) : (a))

#else

#define real float
#define real2 vec2
#define real_from_float(f) (f)
#define real_hi(a) (a)
#define real2_x(v) (v).x
#define real2_y(v) (v).y
#define real_add(a, b) ((a) + (b))
#define real_sub(a, b) ((a) - (b))
#define real_mul(a, b) ((a) * (b))
#define real_mul_float(a, f) ((a) * (f))
#define real_twice(a) ((a) + (a))
#define real_abs(a) abs(a)

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
    real re = real_add(real2_x(start), real_mul_float(real2_x(delta), gl_FragCoord.x));
    real im = real_add(real2_y(start), real_mul_float(real2_y(delta), gl_FragCoord.y));

    int iterations = mandelbrot(real_from_float(0.0), real_from_float(0.0), re, im, maxIterations); // Mandelbrot
    //int iterations = mandelbrot(re, im, real2_x(cursorCoords), real2_y(cursorCoords), maxIterations); // Julia

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
    real zReSq, zImSq;

    for (int iter = 0 ; iter <= maxIter ; iter++) {
        zReSq = real_mul(zRe, zRe);
        zImSq = real_mul(zIm, zIm);
        if (real_hi(zReSq) + real_hi(zImSq) >= 4.0) {
            return iter;
        }

        zIm = real_mul(zRe, zIm);
        zIm = real_twice(zIm); // same as x2, multiplication is expensive
        zIm = real_add(zIm, cIm);
        zRe = real_add(real_sub(zReSq, zImSq), cRe);
    }

    return maxIter;
}

int burningShip(real zRe, real zIm, real cRe, real cIm, int maxIter) {
    real zReSq, zImSq;

    for (int iter = 0 ; iter <= maxIter ; iter++) {
        zReSq = real_mul(zRe, zRe);
        zImSq = real_mul(zIm, zIm);
        if (real_hi(zReSq) + real_hi(zImSq) >= 4.0) {
            return iter;
        }

        zIm = real_mul(zRe, zIm);
        zIm = real_twice(zIm); // same as x2, multiplication is expensive
        zIm = real_abs(real_add(zIm, cIm));
        zRe = real_abs(real_add(real_sub(zReSq, zImSq), cRe));
    }

    return maxIter;
}
