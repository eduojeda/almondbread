#include "param_input.h"

ParamInput::ParamInput(GLFWwindow* window) {
    window_ = window;
    GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    glfwSetCursor(window_, cursor);
}

complex<double> ParamInput::screenToComplex(int x, int y, int width, int height) {
    complex<double> start = origin_ - complex<double>(range_ / 2.0, range_ / 2.0);
    complex<double> delta = complex<double>(range_ / width, range_ / height);

    y = height - y; // screen coords are upside down
    return complex<double>(start.real() + delta.real() * x, start.imag() + delta.imag() * y);
}

void ParamInput::update() {
    int width, height;
    const double panUnit = range_ / 5.0;
    complex<double> panVector(0.0, 0.0);

    glfwGetWindowSize(window_, &width, &height);

    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!mouseDown_) {
            double x, y;
            glfwGetCursorPos(window_, &x, &y);
            zoomTarget_ = screenToComplex(x, y, width, height);

            glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            mouseDown_ = true;
        }

        panVector = zoomTarget_ - origin_;
        range_ *= ZOOM_FACTOR;
        changed_ = true;
    }

    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        mouseDown_ = false;
    }

    if (glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS) {
        panVector += complex<double>(0.0, panUnit);
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS) {
        panVector -= complex<double>(0.0, panUnit);
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        panVector += complex<double>(panUnit, 0.0);
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS) {
        panVector -= complex<double>(panUnit, 0.0);
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        range_ *= ZOOM_FACTOR;
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        range_ /= ZOOM_FACTOR;
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
        quality_--;
        if (quality_ < 1) {
            quality_ = 1;
        }
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) {
        quality_++;
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }

    if (glfwGetKey(window_, GLFW_KEY_TAB) == GLFW_PRESS) {
        logParams();
    }

    if (abs(panVector) > 0.0) {
        origin_ += panVector * PAN_FACTOR;
    }
}

void ParamInput::logParams() {
    cout << "range: " << range_ << endl;
    cout << "zoom: " << INITIAL_RANGE / range_ << endl;
    cout << "origin: " << origin_ << endl;
    cout << "maxIters: " << getMaxIters() << endl;
}

complex<double> ParamInput::getOrigin() {
    return origin_;
}

double ParamInput::getRange() {
    return range_;
}

int ParamInput::getMaxIters() {
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    return quality_ * pow(log10(width / range_), 1.25);
}

bool ParamInput::hasChanged() {
    if (changed_) {
        changed_ = false;
        return true;
    }

    return false;
}