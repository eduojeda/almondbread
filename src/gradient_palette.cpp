#include "gradient_palette.h"

#include <cmath>

GradientPalette::GradientPalette(int size, const std::vector<Stop>& stops): Palette(size, 3) {
    for (int i = 0; i < size; i++) {
        float t = (float) i / size;

        // The last stop at or before t, and the one after it (the first stop again, a cycle later).
        size_t from = stops.size() - 1;
        for (size_t s = 0; s < stops.size(); s++) {
            if (stops[s].position <= t) {
                from = s;
            }
        }
        size_t to = (from + 1) % stops.size();
        float start = stops[from].position > t ? stops[from].position - 1.0f : stops[from].position;
        float end = to == 0 ? stops[to].position + 1.0f : stops[to].position;
        float blend = (t - start) / (end - start);

        const unsigned char* a = &stops[from].r;
        const unsigned char* b = &stops[to].r;
        for (int c = 0; c < 3; c++) {
            data_[i * 3 + c] = (unsigned char) std::lround(a[c] + (b[c] - a[c]) * blend);
        }
    }
}
