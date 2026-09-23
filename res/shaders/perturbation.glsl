#version 400 core

// Perturbation rendering. The CPU computes one reference orbit Z_n at arbitrary precision, and each
// pixel only tracks its offset dz_n = z_n - Z_n from it, which follows
//     dz_{n+1} = 2 Z_n dz_n + dz_n^2 + dc
// where dc is the pixel's offset from the reference point. Offsets shrink far below float's range
// (about 1e-38) at deep zoom, so dz is kept rescaled as w * 2^e, with a float w near 1 and an int e.
// That rescaled step is exact to float precision whenever |Z_n| >= 2^-64. For the iterations where
// the reference passes closer to 0 than that (always including Z_0 = 0), the dz^2 and dc terms can
// dominate at scales float cannot hold, so those steps use float-with-exponent arithmetic throughout.
//
// Rebasing (Zhuoran, 2021) restarts the reference at Z_0 whenever the pixel's own orbit comes closer
// to 0 than its offset, or when the reference orbit runs out. This keeps one reference valid for
// every pixel, including pixels whose orbit escapes long after the reference's, or never.
//
// Bilinear approximation (Zhuoran, 2021) skips most iterations: while dz is small, 2^k steps from
// reference index m collapse to dz -> A dz + B dc. The CPU builds these as a binary tree (see
// BilinearApproximation), and each iteration takes the longest run whose radius dz still fits in.

out vec4 fragColor;

uniform sampler1D paletteTexture;
uniform samplerBuffer referenceOrbit; // (Z.re, Z.im, 0, 0), or (mantissa.re, mantissa.im, exponent, 1) when |Z| < 2^-64
uniform int referenceLast;            // index of the last stored reference iteration
uniform vec2 pixelOffset;             // pixel (0, 0) relative to the reference point, in pixels
uniform vec2 spacingMantissa;         // pixel size is spacingMantissa * 2^spacingExponent
uniform int spacingExponent;
uniform int maxIterations;
uniform samplerBuffer blaTable;       // per node: (A mantissa, A exponent, log2 R + A exponent), (B mantissa, B exponent, 0)
uniform int blaLevels;
uniform int blaLast;                  // reference index the table was built up to; it may trail referenceLast
uniform int blaLevelStart[32];        // first node of each level; level k covers runs of 2^k starting at 1 + j * 2^k

// Runs shorter than this cost more to apply than to iterate.
const int BLA_MIN_LEVEL = 2;

// Radius 4 rather than 2: near c = -2 orbits hover just below |z| = 2, closer than a float can tell
// apart, and no bounded orbit ever exceeds 2. Must match ReferenceOrbit.
const float ESCAPE_RADIUS_SQUARED = 16.0;
const int MIN_EXPONENT = -160;
const int MAX_EXPONENT = 127;

vec4 getColor(int iterations, int maxIterations);

float maxAbs(vec2 v) {
    return max(abs(v.x), abs(v.y));
}

vec2 cmul(vec2 a, vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

vec2 csqr(vec2 a) {
    return vec2(a.x * a.x - a.y * a.y, 2.0 * a.x * a.y);
}

// ldexp is undefined outside float's exponent range, and anything below 2^-160 is zero anyway.
vec2 scaleByPow2(vec2 v, int e) {
    return ldexp(v, ivec2(clamp(e, MIN_EXPONENT, MAX_EXPONENT)));
}

float pow2(int e) {
    return ldexp(1.0, clamp(e, MIN_EXPONENT, MAX_EXPONENT));
}

// Moves m's scale into e so the larger component of m is in [0.5, 1). Zero is left as it is.
void normalize(inout vec2 m, inout int e) {
    float a = maxAbs(m);
    if (a == 0.0) {
        return;
    }
    int k;
    frexp(a, k);
    m = scaleByPow2(m, -k);
    e += k;
}

// (m1 * 2^e1) + (m2 * 2^e2), not normalized.
void addScaled(vec2 m1, int e1, vec2 m2, int e2, out vec2 m, out int e) {
    if (m1 == vec2(0.0)) {
        m = m2;
        e = e2;
        return;
    }
    if (m2 == vec2(0.0)) {
        m = m1;
        e = e1;
        return;
    }
    e = max(e1, e2);
    m = scaleByPow2(m1, e1 - e) + scaleByPow2(m2, e2 - e);
}

void main() {
    vec2 dcMantissa = (pixelOffset + gl_FragCoord.xy) * spacingMantissa;
    int dcExponent = spacingExponent;
    normalize(dcMantissa, dcExponent);

    // dz = w * 2^e. scale is 2^e as a plain float (0 once it underflows) and d is dc / 2^e.
    vec2 w = vec2(0.0);
    int e = dcExponent;
    float scale = pow2(e);
    vec2 d = dcMantissa;

    int m = 0;
    vec4 Z = texelFetch(referenceOrbit, 0);
    int n = 0;
    while (n < maxIterations) {
        int steps = 0;
        if (m > 0 && blaLevels > BLA_MIN_LEVEL) {
            float log2Size = w == vec2(0.0) ? -1e30 : log2(length(w));
            int level = min(m == 1 ? 31 : findLSB(m - 1), blaLevels - 1);
            for (; level >= BLA_MIN_LEVEL; level--) {
                int span = 1 << level;
                if (m + span > blaLast || n + span > maxIterations) {
                    continue;
                }
                int node = blaLevelStart[level] + ((m - 1) >> level);
                vec4 a = texelFetch(blaTable, 2 * node);
                // |dz| < R, compared as log2 |w| + e + A exponent < log2 R + A exponent.
                if (float(e + int(a.z)) + log2Size < a.w) {
                    vec4 b = texelFetch(blaTable, 2 * node + 1);
                    addScaled(cmul(a.xy, w), int(a.z) + e, cmul(b.xy, dcMantissa), int(b.z) + dcExponent, w, e);
                    normalize(w, e);
                    scale = pow2(e);
                    d = scaleByPow2(dcMantissa, dcExponent - e);
                    steps = span;
                    break;
                }
            }
        }

        if (steps == 0) {
            if (Z.w == 0.0) {
                w = 2.0 * cmul(Z.xy, w) + scale * csqr(w) + d;
            } else {
                vec2 sumMantissa;
                int sumExponent;
                addScaled(cmul(Z.xy, w), int(Z.z) + e + 1, csqr(w), 2 * e, sumMantissa, sumExponent);
                addScaled(sumMantissa, sumExponent, dcMantissa, dcExponent, w, e);
                normalize(w, e);
                scale = pow2(e);
                d = scaleByPow2(dcMantissa, dcExponent - e);
            }
            steps = 1;
        }
        n += steps;
        m += steps;
        Z = texelFetch(referenceOrbit, m);

        // Full value z = Z + dz, for the escape test and for rebasing.
        vec2 zMantissa;
        int zExponent;
        bool rebase;
        if (Z.w == 0.0) {
            vec2 z = Z.xy + scale * w;
            if (dot(z, z) >= ESCAPE_RADIUS_SQUARED) {
                break;
            }
            zMantissa = z;
            zExponent = 0;
            rebase = m == referenceLast || maxAbs(z) < scale * maxAbs(w);
        } else {
            addScaled(Z.xy, int(Z.z), w, e, zMantissa, zExponent);
            vec2 z = scaleByPow2(zMantissa, zExponent);
            if (dot(z, z) >= ESCAPE_RADIUS_SQUARED) {
                break;
            }
            normalize(zMantissa, zExponent);
            rebase = m == referenceLast || maxAbs(scaleByPow2(zMantissa, zExponent - e)) < maxAbs(w);
        }

        if (rebase) {
            w = zMantissa;
            e = zExponent;
            normalize(w, e);
            scale = pow2(e);
            d = scaleByPow2(dcMantissa, dcExponent - e);
            m = 0;
            Z = texelFetch(referenceOrbit, 0);
        } else {
            float size = maxAbs(w);
            if (size > 256.0 || (size < 1.0 / 256.0 && size > 0.0)) {
                normalize(w, e);
                scale = pow2(e);
                d = scaleByPow2(dcMantissa, dcExponent - e);
            }
        }
    }

    fragColor = getColor(n, maxIterations);
}

// Deep views need thousands of iterations but a small window of them spans the whole image, so
// the palette cycles every 256 iterations instead of being stretched over maxIterations.
vec4 getColor(int iterations, int maxIterations) {
    if (iterations == maxIterations) {
        return texture(paletteTexture, 1.0);
    }

    float norm = fract(float(iterations) / float(min(maxIterations, 256)));
    return texture(paletteTexture, norm);
}
