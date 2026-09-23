#include "param_input.h"

#include <algorithm>
#include <cmath>

ParamInput::ParamInput(GLFWwindow* window) {
    window_ = window;
    ensurePrecision();
    GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    glfwSetCursor(window_, cursor);
}

void ParamInput::update() {
    // Pans are measured in units of the view size at the start of this update.
    const FloatExp range = range_;
    const double panUnit = 1.0 / 5.0;
    complex<double> panVector(0.0, 0.0);

    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!mouseDown_) {
            zoomTarget_ = pointAtViewOffset(cursorViewOffset(), range);

            glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            mouseDown_ = true;
        }

        panVector = viewOffsetOf(zoomTarget_, range);
        range_ = range_ * ZOOM_FACTOR;
        changed_ = true;
    }

    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        mouseDown_ = false;
    }

    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        zoomOutAround(cursorViewOffset());
        changed_ = true;
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
        range_ = range_ * ZOOM_FACTOR;
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        range_ = range_ / ZOOM_FACTOR;
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        quality_--;
        if (quality_ < 1) {
            quality_ = 1;
        }
        changed_ = true;
    }

    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
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
        origin_ = pointAtViewOffset(panVector * PAN_FACTOR, range);
    }

    // Dragging with the middle button keeps the grabbed point under the cursor. It runs last so it
    // also holds while zooming. The view is only marked changed when something actually moved,
    // because holding still over a deep view must not re-render it every frame.
    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        complex<double> cursor = cursorViewOffset();
        if (!dragging_) {
            dragAnchor_ = pointAtViewOffset(cursor, range_);
            dragCursor_ = cursor;
            dragging_ = true;
        }
        bool zoomed = range_.mantissa != range.mantissa || range_.exponent != range.exponent;
        if (cursor != dragCursor_ || zoomed) {
            dragTo(cursor);
            dragCursor_ = cursor;
            changed_ = true;
        }
    } else {
        dragging_ = false;
    }

    ensurePrecision();
}

void ParamInput::logParams() {
    // Enough decimals to pin the view center down to well below a pixel.
    int digits = max(10, (int) (-range_.log2() * log10(2.0)) + 8);
    cout << "range: " << range_.toString() << endl;
    cout << "zoom: " << (FloatExp(INITIAL_RANGE) / range_).toString() << endl;
    cout << "origin.re: " << origin_.re.toDecimalString(digits) << endl;
    cout << "origin.im: " << origin_.im.toDecimalString(digits) << endl;
    cout << "quality: " << quality_ << endl;
    cout << "maxIters: " << getMaxIters() << endl;
}

const BigComplex& ParamInput::getOrigin() {
    return origin_;
}

complex<double> ParamInput::getCursorCoords() {
    complex<double> offset = cursorViewOffset();
    double range = range_.toDouble();
    return complex<double>(origin_.re.toDouble(), origin_.im.toDouble()) + offset * range;
}

BigComplex ParamInput::getCursorPoint() {
    return pointAtViewOffset(cursorViewOffset(), range_);
}

bool ParamInput::isCursorInWindow() {
    return glfwGetWindowAttrib(window_, GLFW_HOVERED) != 0;
}

FloatExp ParamInput::getZoom() {
    return FloatExp(INITIAL_RANGE) / range_;
}

int ParamInput::getCoordinateDecimals() {
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    double log10PixelSize = (range_.log2() - log2((double) max(width, height))) * log10(2.0);
    return max(1, (int) ceil(-log10PixelSize) + 1);
}

FloatExp ParamInput::getRange() {
    return range_;
}

int ParamInput::getMaxIters() {
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    double zoomDigits = log10((double) width) - range_.log2() * log10(2.0);
    return max(1, (int) (quality_ * pow(max(zoomDigits, 0.0), 1.25)));
}

bool ParamInput::hasChanged() {
    if (changed_) {
        changed_ = false;
        return true;
    }

    return false;
}

// Cursor position relative to the view center, in units of the view size.
complex<double> ParamInput::cursorViewOffset() {
    double x, y;
    int width, height;

    glfwGetWindowSize(window_, &width, &height);
    glfwGetCursorPos(window_, &x, &y);

    // screen coords are upside down
    return complex<double>(x / width - 0.5, (height - y) / height - 0.5);
}

complex<double> ParamInput::viewOffsetOf(const BigComplex& point, FloatExp range) {
    BigComplex offset = point - origin_;
    return complex<double>((FloatExp(offset.re) / range).toDouble(), (FloatExp(offset.im) / range).toDouble());
}

BigComplex ParamInput::pointAtViewOffset(complex<double> offset, FloatExp range) {
    return offsetFrom(origin_, offset, range);
}

BigComplex ParamInput::offsetFrom(const BigComplex& point, complex<double> offset, FloatExp range) {
    int fracLimbs = point.fracLimbs();
    return BigComplex(
        point.re + (range * offset.real()).toBigFixed(fracLimbs),
        point.im + (range * offset.imag()).toBigFixed(fracLimbs));
}

// Zooms out one step while the point under the cursor stays where it is on screen.
void ParamInput::zoomOutAround(complex<double> cursor) {
    BigComplex pinned = pointAtViewOffset(cursor, range_);
    range_ = range_ / ZOOM_FACTOR;
    origin_ = offsetFrom(pinned, -cursor, range_);
}

void ParamInput::dragTo(complex<double> cursor) {
    origin_ = offsetFrom(dragAnchor_, -cursor, range_);
}

// The view center needs finer resolution than a pixel as the view shrinks. 2^-12 of the view is
// finer than a pixel on any window up to 4096 pixels wide.
void ParamInput::ensurePrecision() {
    int fracLimbs = fracLimbsForStep(range_.log2() - 12.0);
    if (fracLimbs > origin_.fracLimbs()) {
        origin_ = origin_.withFracLimbs(fracLimbs);
        zoomTarget_ = zoomTarget_.withFracLimbs(fracLimbs);
        dragAnchor_ = dragAnchor_.withFracLimbs(fracLimbs);
    }
}
