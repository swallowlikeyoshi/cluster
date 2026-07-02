#pragma once
#include "framebuffer.h"
// [FILL-IN] Draws warning icons (fault/HV) at (x,y). Pure. Absorbs the old
// indicators module: warnings now render on the display instead of LEDs.

void widget_warnings_draw(FrameBuffer &fb, int x, int y, bool fault, bool hv);
