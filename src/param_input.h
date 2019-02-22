#ifndef PARAM_INPUT_H
#define PARAM_INPUT_H

#include <GLFW/glfw3.h>
#include <iostream>

class ParamInput {
private:
    GLFWwindow* window_;
    double range_ = 4.0;
    double originRe_ = 0.0;
    double originIm_ = 0.0;
    bool changed_ = true;

public:
    ParamInput(GLFWwindow* window);
    void update();
    double getRange();
    double getOriginRe();
    double getOriginIm();
    bool hasChanged();
};

#endif