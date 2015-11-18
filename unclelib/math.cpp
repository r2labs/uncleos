/* -*- mode: c++; c-basic-offset: 4; -*- */

#include "math.hpp"

int32_t clamp(int32_t value, int32_t min, int32_t max) {
    if (value > max) {
        return max;
    } else if (value < min) {
        return min;
    } else {
        return value;
    }
}

int32_t floor(int32_t value, int32_t scaling) {
    return value/scaling*scaling;
}

int32_t ceil(int32_t value, int32_t scaling) {
    return (value + scaling)/scaling*scaling;
}

int32_t max(int32_t v1, int32_t v2) {
    if (v1 > v2) return v1;
    return v2;
}

int32_t abs(int32_t val) {
    if (val > 0) {
        return val;
    }
    return -val;
}

int32_t lerp(int32_t x, int32_t x_min, int32_t x_max, int32_t y_min, int32_t y_max) {
    if (x > x_max) { return y_max; }
    else if (x < x_min) { return y_min; }
    return y_min + (y_max - y_min)*((x - x_min)/(x_max - x_min));
}
