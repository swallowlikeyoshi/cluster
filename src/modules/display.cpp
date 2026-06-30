// [FILL-IN] Edit this file. Implement the *_compute() function below.
#include "modules/display.h"

DisplayModel display_compute(const DisplayInput &in) {
    DisplayModel m;
    m.speed    = (int)(in.speed_rpm + 0.5f);
    m.soc_pct  = (int)(in.soc * 100.0f + 0.5f);
    m.temp_max = in.controller_temp > in.motor_temp ? in.controller_temp : in.motor_temp;
    m.fault    = in.error1 != 0;
    m.hv       = in.hv_active;
    return m;
}
