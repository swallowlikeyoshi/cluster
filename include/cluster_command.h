// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#pragma once
#include <cstdint>
// [LOCKED] Contract between the HMI module and the CAN layer: the "meaning"
// the Cluster sends to the VCU. The CAN byte encoding lives in can_protocol.

enum class Gear : uint8_t { N = 0, R = 1, D = 2 };

struct ClusterCommand {
    Gear    gear       = Gear::N;
    uint8_t drive_mode = 0;      // 0 Normal, 1 Efficiency, 2 Sport ...
    bool    paddock    = false;  // request VCU speed limit
    bool    vess_enabled = true; // fail-open default: VESS stays on unless disabled
};
