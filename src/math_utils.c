#include "math_utils.h"
#include "math.h"

float approach(float t, float target, float delta) {
    return t < target ? fminf(t + delta, target) : fmaxf(t - delta, target);
}

float approach_angle(float t, float target, float delta) {
    if (fabsf(t - target) > M_PI)
        t += (t > 0 ? -1 : 1) * M_PI * 2;
    return approach(t, target, delta);
}
