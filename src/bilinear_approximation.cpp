#include "bilinear_approximation.h"

#include <algorithm>
#include <cmath>

namespace {

// Finite stand-in for log2(0): the build runs with -ffast-math, which assumes there are no infinities.
const double NO_RADIUS = -1e300;
const float NO_RADIUS_FLOAT = -1e30f;

// (re + i im) * 2^exponent with the larger component in [0.5, 1), or zero.
struct ComplexExp {
    double re = 0.0, im = 0.0;
    int exponent = 0;

    static ComplexExp make(double re, double im, int exponent) {
        ComplexExp result;
        double largest = std::max(std::fabs(re), std::fabs(im));
        if (largest == 0.0) {
            return result;
        }
        int k;
        std::frexp(largest, &k);
        result.re = std::ldexp(re, -k);
        result.im = std::ldexp(im, -k);
        result.exponent = exponent + k;
        return result;
    }

    bool isZero() const {
        return re == 0.0 && im == 0.0;
    }

    ComplexExp operator*(const ComplexExp& other) const {
        return make(re * other.re - im * other.im, re * other.im + im * other.re, exponent + other.exponent);
    }

    ComplexExp operator+(const ComplexExp& other) const {
        if (isZero()) {
            return other;
        }
        if (other.isZero()) {
            return *this;
        }
        int e = std::max(exponent, other.exponent);
        return make(
            std::ldexp(re, exponent - e) + std::ldexp(other.re, other.exponent - e),
            std::ldexp(im, exponent - e) + std::ldexp(other.im, other.exponent - e),
            e);
    }

    double log2Abs() const {
        return isZero() ? NO_RADIUS : 0.5 * std::log2(re * re + im * im) + exponent;
    }
};

struct Node {
    ComplexExp a, b;
    double log2R;
};

// Run x followed by run y: dz -> A_y (A_x dz + B_x dc) + B_y dc. It is valid while x is, and while
// the offset x lands on, at most |A_x| |dz| + |B_x| |dc|, stays inside y's radius.
Node merge(const Node& x, const Node& y, double log2MaxDc) {
    Node result;
    result.a = y.a * x.a;
    result.b = y.a * x.b + y.b;

    double spent = x.b.log2Abs() + log2MaxDc;
    double reach = NO_RADIUS;
    if (y.log2R > NO_RADIUS && spent < y.log2R) {
        reach = y.log2R + std::log2(1.0 - std::exp2(spent - y.log2R));
    }

    double throughY;
    if (reach == NO_RADIUS) {
        throughY = NO_RADIUS;
    } else if (x.a.isZero()) {
        throughY = x.log2R;
    } else {
        throughY = reach - x.a.log2Abs();
    }
    result.log2R = std::min(x.log2R, throughY);
    return result;
}

}

void BilinearApproximation::build(const std::vector<float>& orbitTexels, int lastIndex, double log2MaxDc, double epsilon) {
    levelStarts_.clear();
    texels_.clear();
    if (epsilon <= 0.0 || lastIndex < 2) {
        return;
    }

    // One step from index m is dz -> 2 Z_m dz + dc exactly, apart from the dz^2 term, which stays
    // within epsilon of the linear term while |dz| < epsilon |2 Z_m|.
    std::vector<Node> level;
    double log2Epsilon = std::log2(epsilon);
    for (int m = 1; m < lastIndex; m++) {
        const float* t = &orbitTexels[4 * m];
        int exponent = t[3] == 0.0f ? 0 : (int) t[2];
        Node node;
        node.a = ComplexExp::make(2.0 * t[0], 2.0 * t[1], exponent);
        node.b = ComplexExp::make(1.0, 0.0, 0);
        node.log2R = node.a.isZero() ? NO_RADIUS : log2Epsilon + node.a.log2Abs();
        level.push_back(node);
    }

    int start = 0;
    while (!level.empty()) {
        levelStarts_.push_back(start);
        start += (int) level.size();
        for (const Node& node : level) {
            float log2R = node.log2R <= NO_RADIUS ? NO_RADIUS_FLOAT : (float) (node.log2R + node.a.exponent);
            texels_.insert(texels_.end(), {
                (float) node.a.re, (float) node.a.im, (float) node.a.exponent, log2R,
                (float) node.b.re, (float) node.b.im, (float) node.b.exponent, 0.0f
            });
        }

        std::vector<Node> next;
        for (size_t j = 0; j + 1 < level.size(); j += 2) {
            next.push_back(merge(level[j], level[j + 1], log2MaxDc));
        }
        level.swap(next);
    }
}

int BilinearApproximation::getLevelCount() const {
    return (int) levelStarts_.size();
}

const std::vector<int>& BilinearApproximation::getLevelStarts() const {
    return levelStarts_;
}

const std::vector<float>& BilinearApproximation::getTexels() const {
    return texels_;
}
