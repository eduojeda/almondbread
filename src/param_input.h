#ifndef PARAM_INPUT_H
#define PARAM_INPUT_H

using namespace std;

#include <iostream>
#include <complex>
#include <GLFW/glfw3.h>

#include "big_fixed.h"
#include "float_exp.h"

class ParamInput {
public:
    ParamInput(GLFWwindow* window);
    void update();
    void logParams();

    const BigComplex& getOrigin();
    complex<double> getCursorCoords();
    BigComplex getCursorPoint();
    bool isCursorInWindow();
    FloatExp getZoom();
    // Decimal places needed to tell neighbouring window pixels apart.
    int getCoordinateDecimals();
    FloatExp getRange();
    int getMaxIters();
    bool hasChanged();

private:
    const double INITIAL_RANGE = 3.3;
    const double ZOOM_FACTOR = 0.98;
    const double PAN_FACTOR = 0.03;
    const int INITIAL_QUALITY = 30;

    GLFWwindow* window_;
    bool changed_ = true;
    bool mouseDown_ = false;
    bool dragging_ = false;
    BigComplex dragAnchor_;
    complex<double> dragCursor_;
    BigComplex zoomTarget_;
    BigComplex origin_;
    FloatExp range_ = FloatExp(INITIAL_RANGE);
    int quality_ = INITIAL_QUALITY;

    complex<double> cursorViewOffset();
    complex<double> viewOffsetOf(const BigComplex& point, FloatExp range);
    BigComplex pointAtViewOffset(complex<double> offset, FloatExp range);
    BigComplex offsetFrom(const BigComplex& point, complex<double> offset, FloatExp range);
    void zoomOutAround(complex<double> cursor);
    void dragTo(complex<double> cursor);
    void ensurePrecision();
};

#endif
