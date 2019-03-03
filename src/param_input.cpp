#include "param_input.h"

ParamInput::ParamInput(GLFWwindow* window) {
    window_ = window;
    origin_ = complex<double>(0.0, 0.0);
}

void ParamInput::update() {
    const double originStep = range_ / 100.0;
    const double rangeStep = 0.95; // const? pull out?

    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }

    if (glfwGetKey(window_, GLFW_KEY_TAB) == GLFW_PRESS) {
        logParams();
    }

    if (glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS) {
        changed_ = true; // do this with some sort of meta magic?
        origin_ += complex<double>(0.0, originStep);
    }

    if (glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS) {
        changed_ = true;
        origin_ -= complex<double>(0.0, originStep);
    }

    if (glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        changed_ = true;
        origin_ += complex<double>(originStep, 0.0);
    }

    if (glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS) {
        changed_ = true;
        origin_ -= complex<double>(originStep, 0.0);
    }

    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        changed_ = true;
        range_ *= rangeStep;
    }

    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        changed_ = true;
        range_ /= rangeStep;
    }

    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
        changed_ = true;
        maxIters_--;
        if (maxIters_ < 0) {
            maxIters_ = 0;
        }
    }

    if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) {
        changed_ = true;
        maxIters_++;
    }
}

void ParamInput::logParams() {
    cout << "range: " << range_ << endl;
    cout << "zoom: " << INITIAL_RANGE / range_ << endl;
    cout << "origin: " << origin_ << endl;
    cout << "maxIters: " << maxIters_ << endl;
}

complex<double> ParamInput::getOrigin() {
    return origin_;
}

double ParamInput::getRange() {
    return range_;
}

int ParamInput::getMaxIters() {
    return maxIters_;
}

bool ParamInput::hasChanged() {
    if (changed_) {
        changed_ = false;
        return true;
    }

    return false;
}