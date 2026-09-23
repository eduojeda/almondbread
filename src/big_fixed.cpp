#include "big_fixed.h"

#include <algorithm>
#include <cmath>

namespace {

// Adds value * 2^(32 * index) into little-endian limbs; anything above the top limb is dropped.
void addWordAt(std::vector<uint32_t>& limbs, int index, uint64_t value) {
    for (int i = index; value != 0 && i < (int) limbs.size(); i++) {
        uint64_t sum = (uint64_t) limbs[i] + (value & 0xFFFFFFFFu);
        limbs[i] = (uint32_t) sum;
        value = (value >> 32) + (sum >> 32);
    }
}

}

BigFixed::BigFixed(): limbs_(1, 0) {}

BigFixed::BigFixed(int fracLimbs): limbs_(fracLimbs + 1, 0) {}

BigFixed BigFixed::fromScaled(double mantissa, int exponent, int fracLimbs) {
    BigFixed result(fracLimbs);
    if (mantissa == 0.0 || !std::isfinite(mantissa)) {
        return result;
    }

    int k;
    double fraction = std::frexp(std::fabs(mantissa), &k);
    uint64_t bits = (uint64_t) std::ldexp(fraction, 53);

    // The value is bits * 2^shift, in units of the lowest limb.
    long long shift = (long long) k - 53 + exponent + 32LL * fracLimbs;
    if (shift < 0) {
        bits = shift <= -64 ? 0 : bits >> -shift;
        shift = 0;
    }

    int index = (int) (shift / 32);
    int bit = (int) (shift % 32);
    if (index < (int) result.limbs_.size()) {
        addWordAt(result.limbs_, index, (bits & 0xFFFFFFFFu) << bit);
        addWordAt(result.limbs_, index + 1, (bits >> 32) << bit);
    }

    if (mantissa < 0.0) {
        result.negateInPlace();
    }
    return result;
}

BigFixed BigFixed::fromDouble(double value, int fracLimbs) {
    return fromScaled(value, 0, fracLimbs);
}

int BigFixed::fracLimbs() const {
    return (int) limbs_.size() - 1;
}

BigFixed BigFixed::withFracLimbs(int fracLimbs) const {
    if (fracLimbs == this->fracLimbs()) {
        return *this;
    }

    BigFixed result(fracLimbs);
    int shift = fracLimbs - this->fracLimbs();
    for (int i = 0; i < (int) result.limbs_.size(); i++) {
        int source = i - shift;
        if (source >= 0) {
            result.limbs_[i] = limbs_[source];
        }
    }
    return result;
}

BigFixed BigFixed::operator+(const BigFixed& other) const {
    if (fracLimbs() != other.fracLimbs()) {
        int common = std::max(fracLimbs(), other.fracLimbs());
        return withFracLimbs(common) + other.withFracLimbs(common);
    }

    BigFixed result = *this;
    uint64_t carry = 0;
    for (size_t i = 0; i < limbs_.size(); i++) {
        uint64_t sum = (uint64_t) result.limbs_[i] + other.limbs_[i] + carry;
        result.limbs_[i] = (uint32_t) sum;
        carry = sum >> 32;
    }
    return result;
}

BigFixed BigFixed::operator-(const BigFixed& other) const {
    return *this + (-other);
}

BigFixed BigFixed::operator-() const {
    BigFixed result = *this;
    result.negateInPlace();
    return result;
}

BigFixed BigFixed::operator*(const BigFixed& other) const {
    if (fracLimbs() != other.fracLimbs()) {
        int common = std::max(fracLimbs(), other.fracLimbs());
        return withFracLimbs(common) * other.withFracLimbs(common);
    }

    const int frac = fracLimbs();
    const int size = frac + 1;
    std::vector<uint32_t> a = magnitude();
    std::vector<uint32_t> b = other.magnitude();
    std::vector<uint32_t> product(2 * size + 1, 0);

    // Only limbs from index frac up are kept. Partial products that land entirely below frac - 1 are
    // skipped, which halves the work and costs at most frac units in the last place.
    for (int i = 0; i < size; i++) {
        uint64_t ai = a[i];
        if (ai == 0) {
            continue;
        }

        uint64_t carry = 0;
        for (int j = std::max(0, frac - 1 - i); j < size; j++) {
            uint64_t sum = ai * b[j] + product[i + j] + carry;
            product[i + j] = (uint32_t) sum;
            carry = sum >> 32;
        }
        addWordAt(product, i + size, carry);
    }

    BigFixed result(frac);
    std::copy(product.begin() + frac, product.begin() + frac + size, result.limbs_.begin());
    if (isNegative() != other.isNegative()) {
        result.negateInPlace();
    }
    return result;
}

BigFixed BigFixed::twice() const {
    BigFixed result = *this;
    uint32_t carry = 0;
    for (uint32_t& limb : result.limbs_) {
        uint32_t next = limb >> 31;
        limb = (limb << 1) | carry;
        carry = next;
    }
    return result;
}

bool BigFixed::isNegative() const {
    return (limbs_.back() & 0x80000000u) != 0;
}

void BigFixed::toScaled(double& mantissa, int& exponent) const {
    std::vector<uint32_t> m = magnitude();
    int top = (int) m.size() - 1;
    while (top >= 0 && m[top] == 0) {
        top--;
    }
    if (top < 0) {
        mantissa = 0.0;
        exponent = 0;
        return;
    }

    // Three limbs hold at least 65 significant bits, more than a double keeps.
    int lowest = std::max(0, top - 2);
    double value = 0.0;
    for (int i = top; i >= lowest; i--) {
        value = value * 4294967296.0 + m[i];
    }

    int k;
    mantissa = std::frexp(value, &k);
    exponent = k + 32 * (lowest - fracLimbs());
    if (isNegative()) {
        mantissa = -mantissa;
    }
}

double BigFixed::toDouble() const {
    double mantissa;
    int exponent;
    toScaled(mantissa, exponent);
    return std::ldexp(mantissa, exponent);
}

std::string BigFixed::toDecimalString(int digits) const {
    std::vector<uint32_t> m = magnitude();
    std::string result = isNegative() ? "-" : "";
    result += std::to_string(m.back()) + ".";
    m.back() = 0;

    // Multiplying the fraction by 10 pushes the next decimal digit out of the top fractional limb.
    for (int d = 0; d < digits; d++) {
        uint64_t carry = 0;
        for (size_t i = 0; i + 1 < m.size(); i++) {
            uint64_t value = (uint64_t) m[i] * 10 + carry;
            m[i] = (uint32_t) value;
            carry = value >> 32;
        }
        result += (char) ('0' + carry);
    }
    return result;
}

void BigFixed::negateInPlace() {
    uint64_t carry = 1;
    for (uint32_t& limb : limbs_) {
        uint64_t value = (uint64_t) (uint32_t) ~limb + carry;
        limb = (uint32_t) value;
        carry = value >> 32;
    }
}

std::vector<uint32_t> BigFixed::magnitude() const {
    if (!isNegative()) {
        return limbs_;
    }
    BigFixed negated = -*this;
    return negated.limbs_;
}

int fracLimbsForStep(double log2Step) {
    double bits = std::max(0.0, -log2Step) + 64.0;
    return (int) std::ceil(bits / 32.0);
}
