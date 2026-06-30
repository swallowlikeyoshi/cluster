// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#include "core/io_expander.h"
#include <Arduino.h>
#include <Adafruit_MCP23X17.h>

namespace {
    Adafruit_MCP23X17 mcp_;
    constexpr uint8_t MCP_ADDR = 0x20;
}

namespace io_expander {

void begin() {
    Wire.begin(21, 22);
    mcp_.begin_I2C(MCP_ADDR);
    for (int p = 0; p < 8; p++)  mcp_.pinMode(p, OUTPUT);        // Port A: lamps
    for (int p = 8; p < 16; p++) mcp_.pinMode(p, INPUT_PULLUP);  // Port B: buttons
}

void write_outputs(uint16_t lamp_bits) {
    for (int p = 0; p < 8; p++)
        mcp_.digitalWrite(p, (lamp_bits >> p) & 0x01 ? HIGH : LOW);
}

uint16_t read_inputs() {
    uint16_t bits = 0;
    for (int p = 8; p < 16; p++)
        if (mcp_.digitalRead(p) == LOW)   // active-low button -> active-high bit
            bits |= (1u << (p - 8));
    return bits;
}

} // namespace io_expander
