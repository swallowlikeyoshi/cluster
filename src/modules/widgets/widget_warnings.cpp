// [FILL-IN] Edit this file. Draw your widget into the framebuffer.
#include "modules/widgets/widget_warnings.h"

namespace {
void indicator_box(FrameBuffer &fb, int x, int y, bool active) {
    fb_rect(fb, x, y, 10, 10, active, true);
}
}

void widget_warnings_draw(FrameBuffer &fb, int x, int y, bool fault, bool hv) {
    fb_text(fb, x, y, "WARN", 1);
    indicator_box(fb, x + 32, y, fault);

    fb_text(fb, x, y + 13, "HV", 1);
    indicator_box(fb, x + 32, y + 13, hv);
}
