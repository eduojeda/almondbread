#include "palette.h"

Palette::Palette(int size, short depth): size_(size), depth_(depth) {
    data_ = (unsigned char*)malloc(sizeof(unsigned char) * size_ * depth_);
    memset(data_, 0x66, sizeof(unsigned char) * size_ * depth_);
}

Palette::~Palette() {
    free(data_);
}

unsigned char* Palette::getData() {
    return data_;
}

int Palette::getSize() {
    return size_;
}

short Palette::getDepth() {
    return depth_;
}