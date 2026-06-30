// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#pragma once
#include "modules/display.h"   // for DisplayModel
// [LOCKED] U8g2 128x64 OLED renderer. Draws the pure DisplayModel.

namespace display {
    void begin();
    void render(const DisplayModel &m);
}
