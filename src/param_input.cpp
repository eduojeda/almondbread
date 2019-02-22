#include "param_input.h"

ParamInput::ParamInput(GLFWwindow* window) {
    window_ = window;
}

void ParamInput::update() {
    const double originStep = range_ / 100.0;
    const double rangeStep = 0.95; // const? pull out?

    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }

    if (glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS) {
        changed_ = true; // do this with some sort of meta magic?
        originIm_ += originStep;
    }

    if (glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS) {
        changed_ = true;
        originIm_ -= originStep;
    }

    if (glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        changed_ = true;
        originRe_ += originStep;
    }

    if (glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS) {
        changed_ = true;
        originRe_ -= originStep;
    }

    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        changed_ = true;
        range_ *= rangeStep;
    }

    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        changed_ = true;
        range_ /= rangeStep;
    }
}

double ParamInput::getRange() {
    std::cout << "range: " << range_ << std::endl;
    return range_;
}

double ParamInput::getOriginRe() {
    std::cout << "originRe: " << originRe_ << std::endl;
    return originRe_;
}

double ParamInput::getOriginIm() {
    std::cout << "originIm: " << originIm_ << std::endl;
    return originIm_;
}

bool ParamInput::hasChanged() {
    if (changed_) {
        changed_ = false;
        return true;
    }

    return false;
}