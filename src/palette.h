#ifndef PALETTE_H
#define PALETTE_H

#include <cstdlib>
#include <cstring>

using namespace std;

class Palette {
public:
    Palette(int size, short depth);
    ~Palette();
    virtual unsigned char* getPaletteData() = 0;

protected:
    unsigned char* paletteData_;
    int size_;
    short depth_;
};

#endif