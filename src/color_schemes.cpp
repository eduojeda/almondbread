#include "color_schemes.h"

#include <iostream>

#include "gradient_palette.h"
#include "image_palette.h"
#include "rainbow_palette.h"

namespace {

const int PALETTE_SIZE = 256;

ColorScheme fromPalette(const std::string& name, Palette& palette, unsigned char r, unsigned char g, unsigned char b) {
    ColorScheme scheme;
    scheme.name = name;
    scheme.colors.assign(palette.getData(), palette.getData() + palette.getSize() * 3);
    scheme.interior[0] = r;
    scheme.interior[1] = g;
    scheme.interior[2] = b;
    return scheme;
}

ColorScheme gradient(const std::string& name, const std::vector<GradientPalette::Stop>& stops) {
    GradientPalette palette(PALETTE_SIZE, stops);
    return fromPalette(name, palette, 0, 0, 0);
}

std::vector<ColorScheme> buildSchemes() {
    std::vector<ColorScheme> schemes;

    // The interior keeps the rainbow's last entry, as it always had.
    RainbowPalette rainbow(PALETTE_SIZE, 3);
    const unsigned char* last = rainbow.getData() + (PALETTE_SIZE - 1) * 3;
    schemes.push_back(fromPalette("Rainbow", rainbow, last[0], last[1], last[2]));

    ImagePalette fire("res/palette.png");
    if (fire.getData() && fire.getSize() > 0) {
        schemes.push_back(fromPalette("Fire", fire, 0, 0, 0));
    } else {
        std::cerr << "Failed to load palette from res/palette.png; skipping the Fire color scheme." << std::endl;
    }

    schemes.push_back(gradient("Classic", {
        {0.0f, 0, 7, 100}, {0.16f, 32, 107, 203}, {0.42f, 237, 255, 255}, {0.6425f, 255, 170, 0}, {0.8575f, 0, 2, 0}
    }));
    schemes.push_back(gradient("Ocean", {
        {0.0f, 2, 8, 40}, {0.3f, 8, 64, 150}, {0.55f, 40, 190, 220}, {0.7f, 230, 250, 255}, {0.85f, 40, 140, 200}
    }));
    schemes.push_back(gradient("Grayscale", {
        {0.0f, 0, 0, 0}, {0.5f, 255, 255, 255}
    }));
    return schemes;
}

}

const std::vector<ColorScheme>& colorSchemes() {
    static const std::vector<ColorScheme> schemes = buildSchemes();
    return schemes;
}

const ColorScheme& colorSchemeAt(int index) {
    int count = (int) colorSchemes().size();
    return colorSchemes()[((index % count) + count) % count];
}
