// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#include "core/display_render.h"
#include <Arduino.h>
#include <U8g2lib.h>

// 2.42" 128x64 OLED (SSD1309, I2C). Adjust constructor if the panel differs.
namespace {
    U8G2_SSD1309_128X64_NONAME0_F_HW_I2C oled_(U8G2_R0, U8X8_PIN_NONE);
    char buf_[24];
}

namespace display {

void begin() { oled_.begin(); }

void render(const DisplayModel &m) {
    oled_.clearBuffer();
    oled_.setFont(u8g2_font_logisoso28_tn);
    snprintf(buf_, sizeof(buf_), "%d", m.speed);
    oled_.drawStr(0, 34, buf_);
    oled_.setFont(u8g2_font_6x12_tr);
    oled_.drawStr(90, 12, "rpm");
    snprintf(buf_, sizeof(buf_), "SOC %d%%  %dC", m.soc_pct, m.temp_max);
    oled_.drawStr(0, 52, buf_);
    if (m.hv)    oled_.drawStr(0, 64, "HV");
    if (m.fault) oled_.drawStr(40, 64, "FAULT");
    oled_.sendBuffer();
}

} // namespace display
