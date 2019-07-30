#include "rainbow_palette.h"

RainbowPalette::RainbowPalette(int size, int depth): Palette(size, depth) {
    for (int i = 0; i < size_ ; i += depth_) {
        memset(data_+i, i, depth_);
    }
}