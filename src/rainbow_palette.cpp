#include "rainbow_palette.h"

RainbowPalette::RainbowPalette(int size, int depth): Palette(size, depth) {
    for (int i = 0; i < size_ ; i += depth_) {
        memset(paletteData_+i, i, depth_);
    }
}

unsigned char* RainbowPalette::getPaletteData() {
    return paletteData_;
}