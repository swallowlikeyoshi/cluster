// [FILL-IN] Edit this file. Draw your widget into the framebuffer.
#include "modules/widgets/widget_warnings.h"

void widget_warnings_draw(FrameBuffer &fb, int x, int y, bool fault, bool hv) {
    if (fault) fb_rect(fb, x,      y, 16, 16, true,  true);   // filled box = fault
    if (hv)    fb_rect(fb, x + 20, y, 16, 16, false, true);   // outline box = HV present
}
