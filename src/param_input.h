#ifndef PARAM_INPUT_H
#define PARAM_INPUT_H

using namespace std;

#include <iostream>
#include <complex>
#include <GLFW/glfw3.h>

class ParamInput {
public:
    ParamInput(GLFWwindow* window);
    void update();
    void logParams();

    complex<double> getOrigin();
    complex<double> getCursorCoords();
    double getRange();
    int getMaxIters();
    bool hasChanged();

private:
    const complex<double> INITIAL_ORIGIN = complex<double>(0.0, 0.0);
    const double INITIAL_RANGE = 3.3;
    const double ZOOM_FACTOR = 0.98;
    const double PAN_FACTOR = 0.03;
    const int INITIAL_QUALITY = 30;

    GLFWwindow* window_;
    bool changed_ = true;
    bool mouseDown_ = false;
    complex<double> zoomTarget_;
    complex<double> cursorCoords_;
    complex<double> origin_ = INITIAL_ORIGIN;
    double range_ = INITIAL_RANGE;
    int quality_ = INITIAL_QUALITY;

    void updateCursorCoords();
    complex<double> screenToComplex(int x, int y, int width, int height);
};

#endif