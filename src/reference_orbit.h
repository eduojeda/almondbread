#ifndef REFERENCE_ORBIT_H
#define REFERENCE_ORBIT_H

#include <vector>

#include "big_fixed.h"

// The orbit Z_{n+1} = Z_n^2 + C of one point, computed at arbitrary precision and packed for the GPU
// as one RGBA float texel per iteration: (re, im, 0, 0) when |Z_n| >= 2^-64, otherwise
// (mantissa re, mantissa im, exponent, 1) so values below float's range survive.
class ReferenceOrbit {
public:
    ReferenceOrbit();

    void reset(const BigComplex& c, int fracLimbs);
    // Continues the orbit until it escapes or reaches maxIterations; earlier iterations are kept.
    void extendTo(int maxIterations);

    const BigComplex& getC() const;
    int getFracLimbs() const;
    int getLastIndex() const;
    const std::vector<float>& getTexels() const;

private:
    BigComplex c_;
    BigComplex z_;
    bool escaped_;
    std::vector<float> texels_;

    void appendCurrentIteration();
};

#endif
