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

void widget_best_lap_draw(FrameBuffer &fb, int x, int y, uint8_t lap_count,
                          uint32_t lap_ms) {
    const unsigned lap = lap_count > 99 ? 99 : lap_count;
    char label_buf[8];
    if (lap_ms == 0 || lap == 0) {
        std::snprintf(label_buf, sizeof(label_buf), "TOP--");
        fb_text(fb, x, y, label_buf, 1);
        fb_text(fb, x, y + 12, "--:--.--", 2);
        return;
    }

    const unsigned minutes = (unsigned)((lap_ms / 60000UL) % 100UL);
    const unsigned seconds = (unsigned)((lap_ms / 1000UL) % 60UL);
    const unsigned centis = (unsigned)((lap_ms / 10UL) % 100UL);

    std::snprintf(label_buf, sizeof(label_buf), "TOP%02u", lap);
    fb_text(fb, x, y, label_buf, 1);

    char time_buf[12];
    std::snprintf(time_buf, sizeof(time_buf), "%02u:%02u.%02u",
                  minutes, seconds, centis);
    fb_text(fb, x, y + 12, time_buf, 2);
}
