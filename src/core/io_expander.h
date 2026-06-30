// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#pragma once
#include <cstdint>
// [LOCKED] MCP23017 I/O expander: Port A = lamp outputs, Port B = button inputs.

namespace io_expander {
    void     begin();
    void     write_outputs(uint16_t lamp_bits);   // bit0..7 -> Port A pins
    uint16_t read_inputs();                        // Port B pins -> bit0.. (active-high)
}
