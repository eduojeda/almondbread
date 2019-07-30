#ifndef IMAGE_PALETTE_H
#define IMAGE_PALETTE_H

#include "palette.h"

using namespace std;

class ImagePalette: public Palette {
public:
    ImagePalette(const char* path);
    ~ImagePalette();
};

#endif