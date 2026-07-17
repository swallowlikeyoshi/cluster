// [FILL-IN] Edit this file. Draw your widget into the framebuffer.
#include "modules/widgets/widget_battery.h"

void widget_battery_draw(FrameBuffer &fb, int x, int y, int soc_pct) {
    constexpr int gauge_w = 28;
    constexpr int gauge_h = 128;
    const int gy = y + 14;

    if (soc_pct < 0) {
        fb_text(fb, x, y, "--%", 1);
        fb_rect(fb, x, gy, gauge_w, gauge_h, false, true);
        return;
    }

    if (soc_pct > 100) soc_pct = 100;

    fb_number(fb, x, y, soc_pct, 1);
    fb_text(fb, x + (soc_pct >= 100 ? 24 : soc_pct >= 10 ? 18 : 12), y, "%", 1);

    fb_rect(fb, x, gy, gauge_w, gauge_h, false, true);
    const int fillh = (gauge_h - 2) * soc_pct / 100;
    if (fillh > 0) {
        fb_rect(fb, x + 1, gy + gauge_h - 1 - fillh, gauge_w - 2, fillh, true, true);
    }
}
