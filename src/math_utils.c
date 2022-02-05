#include "math_utils.h"
#include "math.h"

float approach(float t, float target, float delta) {
    return t < target ? fminf(t + delta, target) : fmaxf(t - delta, target);
}
