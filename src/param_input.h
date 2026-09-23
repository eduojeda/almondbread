#ifndef PARAM_INPUT_H
#define PARAM_INPUT_H

using namespace std;

#include <iostream>
#include <complex>
#include <GLFW/glfw3.h>

#include "big_fixed.h"
#include "float_exp.h"

// Turns a held key into discrete steps: one when it goes down, then repeats after a short delay,
// faster the longer it is held.
struct KeyRepeat {
    bool held = false;
    double pressedAt = 0.0;
    double nextStep = 0.0;

    bool step(bool down, double now);
};

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
    const double INITIAL_QUALITY = 30.0;
    // Each A or D step changes the iteration limit by this factor, and always by at least one iteration.
    const double ITERATION_STEP = 1.02;

    GLFWwindow* window_;
    bool changed_ = true;
    bool mouseDown_ = false;
    bool dragging_ = false;
    BigComplex dragAnchor_;
    complex<double> dragCursor_;
    BigComplex zoomTarget_;
    BigComplex origin_;
    FloatExp range_ = FloatExp(INITIAL_RANGE);
    double quality_ = INITIAL_QUALITY;
    KeyRepeat lowerIterations_;
    KeyRepeat raiseIterations_;

    complex<double> cursorViewOffset();
    complex<double> viewOffsetOf(const BigComplex& point, FloatExp range);
    BigComplex pointAtViewOffset(complex<double> offset, FloatExp range);
    BigComplex offsetFrom(const BigComplex& point, complex<double> offset, FloatExp range);
    void zoomOutAround(complex<double> cursor);
    void dragTo(complex<double> cursor);
    double iterationsPerQuality();
    void stepMaxIterations(int direction);
    void ensurePrecision();
};

#endif
