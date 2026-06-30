// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#pragma once
#include <cstdint>
// [LOCKED] TWAI (ESP32 CAN). Cluster is RX-first: receive vehicle state into
// `state`; also send a Cluster->VCU command frame. NO life-signal task.

namespace can_bus {
    void begin();                                  // install + start TWAI @ 250kbps
    void poll_rx();                                // drain RX -> state (parse is a stub)
    void send_command(uint8_t cmd, uint8_t param); // 0x1801D0C0
}
