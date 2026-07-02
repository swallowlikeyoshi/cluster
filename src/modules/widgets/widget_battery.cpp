// [FILL-IN] Edit this file. Draw your widget into the framebuffer.
#include "modules/widgets/widget_battery.h"

void widget_battery_draw(FrameBuffer &fb, int x, int y, int soc_pct) {
    fb_bar(fb, x, y, 60, 12, soc_pct);          // SOC gauge
    fb_number(fb, x + 66, y, soc_pct, 1);       // numeric %
    // place "%" right after the digits: 6px/char at scale 1 (1/2/3-digit SOC).
    fb_text(fb, x + 66 + (soc_pct >= 100 ? 24 : soc_pct >= 10 ? 18 : 12), y, "%", 1);
}
