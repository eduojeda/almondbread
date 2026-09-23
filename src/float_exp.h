#ifndef FLOAT_EXP_H
#define FLOAT_EXP_H

#include <cmath>
#include <cstdio>
#include <string>

#include "big_fixed.h"

// A double with a separate exponent, for sizes such as the view range that leave double's
// range (1e-308) at deep zoom.
struct FloatExp {
    double mantissa; // |mantissa| in [0.5, 1), or 0
    int exponent;

    FloatExp(): mantissa(0.0), exponent(0) {}

    FloatExp(double value, int exponent) {
        int k;
        mantissa = std::frexp(value, &k);
        this->exponent = value == 0.0 ? 0 : exponent + k;
    }

    explicit FloatExp(double value): FloatExp(value, 0) {}

    explicit FloatExp(const BigFixed& value) {
        value.toScaled(mantissa, exponent);
    }

    FloatExp operator*(double factor) const { return FloatExp(mantissa * factor, exponent); }
    FloatExp operator/(double divisor) const { return FloatExp(mantissa / divisor, exponent); }
    FloatExp operator*(const FloatExp& other) const { return FloatExp(mantissa * other.mantissa, exponent + other.exponent); }
    FloatExp operator/(const FloatExp& other) const { return FloatExp(mantissa / other.mantissa, exponent - other.exponent); }

    double toDouble() const { return std::ldexp(mantissa, exponent); }
    double log2() const { return std::log2(std::fabs(mantissa)) + exponent; }
    BigFixed toBigFixed(int fracLimbs) const { return BigFixed::fromScaled(mantissa, exponent, fracLimbs); }

    std::string toString(int decimals = 6) const {
        if (mantissa == 0.0) {
            return "0";
        }
        double log10Value = std::log10(std::fabs(mantissa)) + exponent * std::log10(2.0);
        double power = std::floor(log10Value);
        double digits = std::pow(10.0, log10Value - power);
        if (digits >= 10.0 - 0.5 * std::pow(10.0, -decimals)) {
            digits /= 10.0;
            power += 1.0;
        }
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%s%.*fe%+.0f", mantissa < 0.0 ? "-" : "", decimals, digits, power);
        return buffer;
    }
};

#endif
