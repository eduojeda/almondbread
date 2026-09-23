#ifndef HUD_H
#define HUD_H

#include <deque>
#include <string>
#include <vector>

// glad has to come before GLFW, which param_input.h includes.
#include "text_overlay.h"
#include "param_input.h"

// The frame rate, zoom level, iteration limit and the coordinates under the cursor, in the top-left corner.
class Hud {
public:
    Hud(GLFWwindow* window);

    void recordFrame(double renderSeconds);
    void draw(ParamInput& input);

private:
    GLFWwindow* window_;
    TextOverlay overlay_;
    std::deque<double> frameTimes_;
    double lastRenderSeconds_ = 0.0;

    std::vector<std::string> lines(ParamInput& input);
    std::string frameRateLine();
};

#endif
