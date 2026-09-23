#include "reference_orbit.h"

#include <algorithm>
#include <cmath>

namespace {

const int SMALL_EXPONENT = -64;

// Orbits near c = -2 hover just below |z| = 2 for many iterations, closer than a double can tell
// apart. No bounded orbit ever exceeds 2, so a radius of 4 costs about one iteration and
// avoids declaring them escaped by rounding.
const double ESCAPE_RADIUS_SQUARED = 16.0;

}

ReferenceOrbit::ReferenceOrbit(): escaped_(false) {}

void ReferenceOrbit::reset(const BigComplex& c, int fracLimbs) {
    c_ = c.withFracLimbs(fracLimbs);
    z_ = BigComplex(BigFixed(fracLimbs), BigFixed(fracLimbs));
    escaped_ = false;
    texels_.clear();
    appendCurrentIteration();
}

void ReferenceOrbit::extendTo(int maxIterations) {
    while (!escaped_ && getLastIndex() < maxIterations) {
        BigFixed re2 = z_.re * z_.re;
        BigFixed im2 = z_.im * z_.im;
        BigFixed reIm = z_.re * z_.im;
        z_ = BigComplex(re2 - im2 + c_.re, reIm.twice() + c_.im);
        appendCurrentIteration();
    }
}

const BigComplex& ReferenceOrbit::getC() const {
    return c_;
}

int ReferenceOrbit::getFracLimbs() const {
    return c_.fracLimbs();
}

int ReferenceOrbit::getLastIndex() const {
    return (int) texels_.size() / 4 - 1;
}

const std::vector<float>& ReferenceOrbit::getTexels() const {
    return texels_;
}

void ReferenceOrbit::appendCurrentIteration() {
    double reMantissa, imMantissa;
    int reExponent, imExponent;
    z_.re.toScaled(reMantissa, reExponent);
    z_.im.toScaled(imMantissa, imExponent);

    double re = std::ldexp(reMantissa, reExponent);
    double im = std::ldexp(imMantissa, imExponent);
    if (re * re + im * im >= ESCAPE_RADIUS_SQUARED) {
        escaped_ = true;
    }

    if (reMantissa == 0.0 && imMantissa == 0.0) {
        texels_.insert(texels_.end(), {0.0f, 0.0f, 0.0f, 1.0f});
        return;
    }

    int exponent = reMantissa == 0.0 ? imExponent : imMantissa == 0.0 ? reExponent : std::max(reExponent, imExponent);
    if (exponent > SMALL_EXPONENT) {
        texels_.insert(texels_.end(), {(float) re, (float) im, 0.0f, 0.0f});
    } else {
        texels_.insert(texels_.end(), {
            (float) std::ldexp(reMantissa, reExponent - exponent),
            (float) std::ldexp(imMantissa, imExponent - exponent),
            (float) exponent,
            1.0f
        });
    }
}
