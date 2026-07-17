#include "modules/widgets/widget_laptime.h"
#include <cstdio>

void widget_laptime_draw(FrameBuffer &fb, int x, int y, uint8_t lap_count,
                         uint32_t lap_ms, bool gps_ok) {
    const unsigned minutes = (unsigned)((lap_ms / 60000UL) % 100UL);
    const unsigned seconds = (unsigned)((lap_ms / 1000UL) % 60UL);
    const unsigned centis = (unsigned)((lap_ms / 10UL) % 100UL);
    const unsigned lap = lap_count > 99 ? 99 : lap_count;

    char lap_buf[8];
    std::snprintf(lap_buf, sizeof(lap_buf), "LAP%02u", lap);
    fb_text(fb, x, y, lap_buf, 3);

    char time_buf[12];
    if (gps_ok) {
        std::snprintf(time_buf, sizeof(time_buf), "%02u:%02u.%02u",
                      minutes, seconds, centis);
    } else {
        std::snprintf(time_buf, sizeof(time_buf), "--:--.--");
    }
    fb_text(fb, x, y + 31, time_buf, 4);
}
