#ifndef RAINBOW_PALETTE_H
#define RAINBOW_PALETTE_H

#include "palette.h"

using namespace std;

class RainbowPalette: public Palette {
public:
    RainbowPalette(int size, int depth);

private:
    int calculateChannelValue(int step, int size, float channelPhase);
};

#endif