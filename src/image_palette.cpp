#include "image_palette.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

ImagePalette::ImagePalette(const char* path): Palette(0, 0) {
    int width, height, channels;
    data_ = stbi_load(path, &width, &height, &channels, 0);
}

ImagePalette::~ImagePalette() {
    stbi_image_free(data_);
}