// [FILL-IN] Edit this file. Implement the *_compute() function below.
#include "modules/hmi_input.h"

ClusterCommand hmi_compute(const HmiSwitches &in) {
    ClusterCommand cmd;
    switch (in.gear_raw) {
        case 1:  cmd.gear = Gear::R; break;
        case 2:  cmd.gear = Gear::D; break;
        default: cmd.gear = Gear::N; break;
    }
    cmd.paddock    = in.paddock;
    cmd.drive_mode = (uint8_t)(in.config_bits & 0x03);

    // 코드 수정 예시

    return cmd;
}
