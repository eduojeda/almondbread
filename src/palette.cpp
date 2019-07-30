#include "palette.h"

Palette::Palette(int size, short depth): size_(size), depth_(depth) {
    paletteData_ = (unsigned char*)malloc(sizeof(unsigned char) * size_ * depth_);
    memset(paletteData_, 0x66, sizeof(unsigned char) * size_ * depth_);
}

Palette::~Palette() {
    free(paletteData_);
}