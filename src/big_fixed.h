#ifndef BIG_FIXED_H
#define BIG_FIXED_H

#include <cstdint>
#include <string>
#include <vector>

// Signed fixed-point number with arbitrary precision: the value is the two's complement integer held
// in the limbs, divided by 2^(32 * fracLimbs). The top limb is the integer part, so values must stay
// within +-2^31. That is plenty here: every Mandelbrot value we track stays below 64.
class BigFixed {
public:
    BigFixed();
    explicit BigFixed(int fracLimbs);

    // mantissa * 2^exponent, truncated to the given precision.
    static BigFixed fromScaled(double mantissa, int exponent, int fracLimbs);
    static BigFixed fromDouble(double value, int fracLimbs);

    int fracLimbs() const;
    BigFixed withFracLimbs(int fracLimbs) const;

    BigFixed operator+(const BigFixed& other) const;
    BigFixed operator-(const BigFixed& other) const;
    BigFixed operator-() const;
    BigFixed operator*(const BigFixed& other) const;
    BigFixed twice() const;

    bool isNegative() const;
    // Splits the value into mantissa * 2^exponent with |mantissa| in [0.5, 1), or (0, 0) for zero.
    void toScaled(double& mantissa, int& exponent) const;
    double toDouble() const;
    std::string toDecimalString(int digits) const;

private:
    std::vector<uint32_t> limbs_;

    void negateInPlace();
    std::vector<uint32_t> magnitude() const;
};

struct BigComplex {
    BigFixed re, im;

    BigComplex() {}
    BigComplex(const BigFixed& re, const BigFixed& im): re(re), im(im) {}

    BigComplex operator+(const BigComplex& other) const { return BigComplex(re + other.re, im + other.im); }
    BigComplex operator-(const BigComplex& other) const { return BigComplex(re - other.re, im - other.im); }
    BigComplex withFracLimbs(int fracLimbs) const { return BigComplex(re.withFracLimbs(fracLimbs), im.withFracLimbs(fracLimbs)); }
    int fracLimbs() const { return re.fracLimbs(); }
};

// Fractional limbs needed to resolve steps of size 2^log2Step, plus 64 guard bits against rounding
// errors that the fractal's dynamics amplify.
int fracLimbsForStep(double log2Step);

#endif
