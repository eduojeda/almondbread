#ifndef BILINEAR_APPROXIMATION_H
#define BILINEAR_APPROXIMATION_H

#include <vector>

// Bilinear approximations (Zhuoran, 2021) of runs of perturbation iterations along a reference orbit.
// While a pixel's offset dz is small enough, 2^k iterations starting at reference index m collapse to
//     dz_{m + 2^k} = A dz_m + B dc
// which holds to the requested relative error while |dz_m| < R. The runs form a binary tree: level k
// holds the runs of length 2^k that start at reference indices 1 + j * 2^k.
class BilinearApproximation {
public:
    // orbitTexels is the orbit as packed by ReferenceOrbit, log2MaxDc an upper bound on log2 |dc| over
    // every pixel, and epsilon the relative error allowed per iteration. Epsilon 0 disables the table.
    void build(const std::vector<float>& orbitTexels, int lastIndex, double log2MaxDc, double epsilon);

    int getLevelCount() const;
    const std::vector<int>& getLevelStarts() const;
    // Two RGBA texels per node: (A mantissa, A exponent, log2 R + A exponent) and (B mantissa, B exponent, 0).
    // R is stored relative to A's exponent because log2 R alone can be too large for float to hold precisely.
    const std::vector<float>& getTexels() const;

private:
    std::vector<int> levelStarts_;
    std::vector<float> texels_;
};

#endif
