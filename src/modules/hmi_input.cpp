// [FILL-IN] Edit this file. Implement the *_compute() function below.
#include "modules/hmi_input.h"

static bool rising(uint16_t cur, uint16_t prev, uint16_t mask) {
    return (cur & mask) && !(prev & mask);
}

HmiOutput hmi_compute(const HmiInput &in) {
    HmiOutput o;
    o.drive_mode = in.cur_drive_mode;
    o.reset_req  = false;
    if (rising(in.buttons, in.prev_buttons, 0x01))   // button0: cycle drive mode
        o.drive_mode = (uint8_t)((in.cur_drive_mode + 1) % 3);
    if (rising(in.buttons, in.prev_buttons, 0x02))   // button1: reset
        o.reset_req = true;
    return o;
}
