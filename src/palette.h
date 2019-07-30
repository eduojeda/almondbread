#ifndef PALETTE_H
#define PALETTE_H

#include <cstdlib>
#include <cstring>

using namespace std;

class Palette {
public:
    Palette(int size, short depth);
    ~Palette();
    unsigned char* getData();
    int getSize();
    short getDepth();

protected:
    unsigned char* data_;
    int size_;
    short depth_;
};

#endif