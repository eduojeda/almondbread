#include "image_palette.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

// Uses the image's first row, converted to RGB.
ImagePalette::ImagePalette(const char* path): Palette(0, 0) {
    int width, height, channels;
    free(data_);
    data_ = stbi_load(path, &width, &height, &channels, 3);
    size_ = data_ ? width : 0;
    depth_ = 3;
}

ImagePalette::~ImagePalette() {
    stbi_image_free(data_);
    // The base destructor frees data_ too.
    data_ = NULL;
}
