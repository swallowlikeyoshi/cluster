#pragma once
#include "framebuffer.h"
// [FILL-IN] Draws a SOC bar + percentage at (x,y). Pure.

void widget_battery_draw(FrameBuffer &fb, int x, int y, int soc_pct);
