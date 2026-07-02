// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#include "core/display_blit.h"

namespace display_blit {

void begin() {
    // TODO(core): init the chosen panel (SPI/I2C) once hardware is selected.
}

void show(const FrameBuffer &fb) {
    // TODO(core): push fb.bits (1bpp, FB_STRIDE*FB_H bytes) to the panel.
    // Until then this is a no-op so the firmware builds and runs.
    (void)fb;
}

} // namespace display_blit
