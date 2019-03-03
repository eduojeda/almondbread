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
    const double INITIAL_RANGE = 4.0;

    GLFWwindow* window_;
    double range_ = INITIAL_RANGE;
    complex<double> origin_;
    int maxIters_ = 30;
    bool changed_ = true;
};

#endif