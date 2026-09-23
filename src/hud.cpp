#include "hud.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "color_schemes.h"

namespace {

// Once a coordinate has too many decimals for one line, this many are kept at each end: the leading
// ones say where in the set you are, the trailing ones change as the cursor moves.
const int KEPT_DECIMALS = 16;

std::string formatCoordinate(const BigFixed& value, int decimals) {
    std::string text = value.toDecimalString(decimals);
    if (text[0] != '-') {
        text = " " + text;
    }

    int omitted = decimals - 2 * KEPT_DECIMALS;
    if (omitted <= 8) {
        return text;
    }
    size_t point = text.find('.');
    return text.substr(0, point + 1 + KEPT_DECIMALS) + "...(" + std::to_string(omitted) + " digits)..."
        + text.substr(text.size() - KEPT_DECIMALS);
}

std::string withThousandsSeparators(int value) {
    std::string digits = std::to_string(value);
    for (int i = (int) digits.size() - 3; i > 0; i -= 3) {
        digits.insert(i, ",");
    }
    return digits;
}

std::string formatZoom(FloatExp zoom) {
    if (zoom.log2() < 16.0) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.1fx", zoom.toDouble());
        return buffer;
    }
    return zoom.toString(3) + "x";
}

}

Hud::Hud(GLFWwindow* window): window_(window) {}

void Hud::recordFrame(double renderSeconds) {
    // A pause of more than a few display refreshes beyond the frame's own render time starts a new
    // run, so the rate only counts frames rendered back to back.
    double now = glfwGetTime();
    if (!frameTimes_.empty() && now - frameTimes_.back() > renderSeconds + 0.05) {
        frameTimes_.clear();
    }
    frameTimes_.push_back(now);
    lastRenderSeconds_ = renderSeconds;
}

void Hud::draw(ParamInput& input) {
    int framebufferWidth, framebufferHeight;
    float contentScale, unused;
    glfwGetFramebufferSize(window_, &framebufferWidth, &framebufferHeight);
    glfwGetWindowContentScale(window_, &contentScale, &unused);

    // The font is 12 units per line; 1.5 units per screen point keeps it legible without covering much.
    float scale = std::max(1.0f, std::round(1.5f * contentScale));
    overlay_.draw(lines(input), framebufferWidth, framebufferHeight, scale, 8.0f * contentScale);
}

std::vector<std::string> Hud::lines(ParamInput& input) {
    std::vector<std::string> result;
    std::string frameRate = frameRateLine();
    if (!frameRate.empty()) {
        result.push_back(frameRate);
    }
    result.push_back("Zoom " + formatZoom(input.getZoom()));
    result.push_back("Iteration limit " + withThousandsSeparators(input.getMaxIters()));
    result.push_back("Colors " + colorSchemeAt(input.getColorScheme()).name + " (C to change)");

    if (input.isCursorInWindow()) {
        BigComplex point = input.getCursorPoint();
        int decimals = input.getCoordinateDecimals();
        result.push_back("Re " + formatCoordinate(point.re, decimals));
        result.push_back("Im " + formatCoordinate(point.im, decimals));
    } else {
        result.push_back("Cursor outside the window");
    }
    return result;
}

// The fractal only re-renders when the view changes, so the rate is measured over frames rendered
// back to back, such as while zooming. Otherwise the overlay shows how long the last frame took.
std::string Hud::frameRateLine() {
    if (frameTimes_.empty()) {
        return "";
    }

    double now = glfwGetTime();
    double window = std::max(1.0, 3.0 * lastRenderSeconds_);
    while (frameTimes_.size() > 1 && now - frameTimes_.front() > window) {
        frameTimes_.pop_front();
    }

    char buffer[64];
    double milliseconds = lastRenderSeconds_ * 1000.0;
    double span = frameTimes_.back() - frameTimes_.front();
    bool rendering = frameTimes_.size() >= 2 && span > 0.0 && now - frameTimes_.back() < std::max(0.25, 2.0 * lastRenderSeconds_);
    if (rendering) {
        double fps = (frameTimes_.size() - 1) / span;
        snprintf(buffer, sizeof(buffer), "%.*f fps (%.1f ms per frame)", fps < 10.0 ? 1 : 0, fps, milliseconds);
    } else {
        snprintf(buffer, sizeof(buffer), "Idle (last frame %.1f ms)", milliseconds);
    }
    return buffer;
}
