// [FILL-IN] Edit this file. Implement the *_compute() function below.
#include "modules/indicators.h"

uint16_t indicators_compute(const IndicatorInput &in) {
    uint16_t bits = 0;
    if (in.error1 != 0) bits |= 0x01;   // bit0: any fault
    if (in.hv_active)   bits |= 0x02;   // bit1: HV active
    if (in.brake)       bits |= 0x04;   // bit2: brake
    return bits;
}
