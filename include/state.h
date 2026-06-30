// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#pragma once
#include <cstdint>
// [LOCKED] Cluster shared state bus. ONLY core/app_wiring.cpp may include this.
// Module files (src/modules/*) must never include state.h.

struct ClusterState {
    // received vehicle state (from CAN)
    float    speed_rpm    = 0.0f;
    float    bus_voltage  = 0.0f;   // V
    float    bus_current  = 0.0f;   // A
    float    soc          = 0.0f;   // 0..1
    int      controller_temp = 0;   // C
    int      motor_temp      = 0;   // C
    uint8_t  error1 = 0, error2 = 0, error3 = 0;   // raw error bitmaps
    uint8_t  gear   = 0;            // 0..7
    bool     brake     = false;
    bool     hv_active = false;
    bool     handshaked = false;
    // HMI outputs (sent to VCU)
    uint8_t  drive_mode = 0;        // 0 Normal, 1 Efficiency, 2 Sport
    bool     reset_req  = false;
};

extern ClusterState state;   // defined in core/app_wiring.cpp
