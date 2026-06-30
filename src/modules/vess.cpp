// [FILL-IN] Edit this file. Implement the *_compute() function below.
#include "modules/vess.h"

int vess_compute(const VessInput &in) {
    if (!in.active) return 0;
    int hz = 200 + (int)(in.speed_rpm * 0.1f);
    if (hz < 200)  hz = 200;
    if (hz > 2000) hz = 2000;
    return hz;
}
