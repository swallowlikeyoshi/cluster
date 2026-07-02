// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#pragma once
#include "framebuffer.h"
// [LOCKED] Pushes the 1bpp framebuffer to the physical panel. The panel is not
// chosen yet, so show() is a stub; implement it when the OLED/LCD is selected.

namespace display_blit {
    void begin();
    void show(const FrameBuffer &fb);
}
