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
    double getRange();
    int getMaxIters();
    bool hasChanged();

private:
    const double ZOOM_FACTOR = 0.99;
    const double PAN_FACTOR = 0.02;
    const double INITIAL_RANGE = 4.0;
    const double INITIAL_MAX_ITERS = 30;

    GLFWwindow* window_;
    complex<double> origin_;
    complex<double> zoomTarget_;
    double range_ = INITIAL_RANGE;
    int maxIters_ = INITIAL_MAX_ITERS;
    bool changed_ = true;
    bool mouseDown_ = false;

    complex<double> screenToComplex(int x, int y, int width, int height);
};

#endif