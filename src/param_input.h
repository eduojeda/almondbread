#ifndef PARAM_INPUT_H
#define PARAM_INPUT_H

#include <iostream>
#include <GLFW/glfw3.h>

class ParamInput {
public:
    ParamInput(GLFWwindow* window);
    void update();
    double getRange();
    double getOriginRe();
    double getOriginIm();
    int getMaxIters();
    bool hasChanged();
    void logParams();

private:
    GLFWwindow* window_;
    double range_ = 4.0;
    double originRe_ = 0.0;
    double originIm_ = 0.0;
    int maxIters_ = 30;
    bool changed_ = true;
};

#endif