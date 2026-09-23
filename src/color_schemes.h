#ifndef COLOR_SCHEMES_H
#define COLOR_SCHEMES_H

#include <string>
#include <vector>

struct ColorScheme {
    std::string name;
    std::vector<unsigned char> colors; // RGB triples, cycled through by iteration count
    unsigned char interior[3];         // points that never escape
};

// The available schemes, built on first use. Rainbow comes first and keeps the original look.
const std::vector<ColorScheme>& colorSchemes();

// Any integer selects a scheme, wrapping around in both directions.
const ColorScheme& colorSchemeAt(int index);

#endif
