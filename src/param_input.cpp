#include "param_input.h"

ParamInput::ParamInput(GLFWwindow* window) {
    window_ = window;
    origin_ = complex<double>(0.0, 0.0);
}

void ParamInput::update() {
    complex<double> panDirection(0.0, 0.0);
    const double panSpeed = range_ / 100.0;
    const double zoomSpeed = 0.95; // const? pull out?

//     if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
//         int width, height;
//         double x, y;

//         glfwGetWindowSize(window_, &width, &height);
//         glfwGetCursorPos(window_, &x, &y);

//         complex<double> start = origin_ - complex<double>(range_ / 2.0, range_ / 2.0);
//         complex<double> delta = complex<double>(range_ / width, range_ / height);
//         target_ = complex<double>(start.real() + delta.real() * x, start.imag() + delta.imag() * y);

// //        panDirection = polar(1.0, 1.0 / tan(y - height / 2.0 / x - width / 2.0));
//         //panDirection = complex<double>(x - width / 2.0, -(y - height / 2.0));
//         //range_ *= zoomSpeed;
//         changed_ = true;
//     }

    if (glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS) {
        panDirection += complex<double>(0.0, 1.0);
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS) {
        panDirection -= complex<double>(0.0, 1.0);
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        panDirection += complex<double>(1.0, 0.0);
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS) {
        panDirection -= complex<double>(1.0, 0.0);
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        range_ *= zoomSpeed;
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        range_ /= zoomSpeed;
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
        maxIters_--;
        if (maxIters_ < 1) {
            maxIters_ = 1;
        }
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) {
        maxIters_++;
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }

    if (glfwGetKey(window_, GLFW_KEY_TAB) == GLFW_PRESS) {
        logParams();
    }

    if (abs(panDirection) > 0) {
        origin_ += panDirection / abs(panDirection) * panSpeed;
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