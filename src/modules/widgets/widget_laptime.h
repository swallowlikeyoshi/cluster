#pragma once
#include "framebuffer.h"
#include <cstdint>

void widget_laptime_draw(FrameBuffer &fb, int x, int y, uint8_t lap_count,
                         uint32_t lap_ms, bool gps_ok);
