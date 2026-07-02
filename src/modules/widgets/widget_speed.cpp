// [FILL-IN] Edit this file. Draw your widget into the framebuffer.
#include "modules/widgets/widget_speed.h"

void widget_speed_draw(FrameBuffer &fb, int x, int y, int speed) {
    if (speed < 0) speed = 0;
    fb_number(fb, x, y, speed, 4);   // big digits
}
