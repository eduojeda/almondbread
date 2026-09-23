#ifndef GRADIENT_PALETTE_H
#define GRADIENT_PALETTE_H

#include <vector>

#include "palette.h"

// A palette that blends between color stops and wraps from the last stop back to the first, so it
// can repeat without a seam.
class GradientPalette: public Palette {
public:
    struct Stop {
        float position; // in [0, 1)
        unsigned char r, g, b;
    };

    GradientPalette(int size, const std::vector<Stop>& stops);
};

#endif
