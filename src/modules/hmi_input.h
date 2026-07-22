#pragma once
#include <cstdint>
#include "cluster_command.h"
// [FILL-IN] Pure: physical switch states -> a semantic ClusterCommand.
// The locked core reads the switches and encodes the command to CAN.

struct HmiSwitches {
    uint8_t  gear_raw;      // 0=N, 1=R, 2=D (other -> N)
    bool     paddock;       // paddock-mode switch
    uint16_t config_bits;   // other configurable switches (drive mode in low 2 bits)
};

ClusterCommand hmi_compute(const HmiSwitches &in);
