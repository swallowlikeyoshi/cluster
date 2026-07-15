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
    // received vehicle state (from CAN, Controller_L — sniffed off VCU feedback frames)
    float    speed_rpm    = 0.0f;
    float    bus_voltage  = 0.0f;   // V
    float    bus_current  = 0.0f;   // A
    float    soc          = 0.0f;   // 0..1
    int      controller_temp = 0;   // C
    int      motor_temp      = 0;   // C
    uint8_t  error1 = 0, error2 = 0, error3 = 0;   // raw error bitmaps
    // Controller_R mirror of the above (dual motor — see CAN_PROTOCOL.md §7)
    float    speed_rpm_r    = 0.0f;
    float    bus_voltage_r  = 0.0f;
    float    bus_current_r  = 0.0f;
    int      controller_temp_r = 0;
    int      motor_temp_r      = 0;
    uint8_t  error1_r = 0, error2_r = 0, error3_r = 0;
    uint8_t  gear   = 0;            // 0..7
    bool     brake     = false;
    bool     hv_active = false;
    bool     handshaked = false;
    // HMI outputs (sent to VCU)
    uint8_t  drive_mode = 0;        // 0 Normal, 1 Efficiency, 2 Sport
    bool     reset_req  = false;
};

extern ClusterState state;   // defined in core/app_wiring.cpp
