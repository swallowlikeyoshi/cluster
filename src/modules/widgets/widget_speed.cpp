// [FILL-IN] Edit this file. Draw your widget into the framebuffer.
#include "modules/widgets/widget_speed.h"

namespace {
constexpr float WHEEL_DIAMETER_M = 0.4597f;   // 45.97 cm
constexpr float GEAR_RATIO = 3.72f;
constexpr float PI_F = 3.14159265f;
constexpr int MAX_SPEED_KPH = 80;

int digit_count(int value) {
    if (value < 0) value = -value;
    int digits = 1;
    while (value >= 10) {
        value /= 10;
        ++digits;
    }
    return digits;
}

int rpm_to_kph(int rpm) {
    if (rpm < 0) rpm = -rpm;
    const float wheel_circumference_m = WHEEL_DIAMETER_M * PI_F;
    const float kph = ((float)rpm / GEAR_RATIO) * wheel_circumference_m * 0.06f;
    return (int)(kph + 0.5f);
}

void speed_bar(FrameBuffer &fb, int x, int y, int w, int h, int kph) {
    int pct = kph * 100 / MAX_SPEED_KPH;
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    fb_rect(fb, x, y, w, h, false, true);
    const int fillw = (w - 2) * pct / 100;
    if (fillw > 0) fb_rect(fb, x + 1, y + 1, fillw, h - 2, true, true);
}
}

void widget_speed_draw(FrameBuffer &fb, int x, int y, int rpm) {
    const int kph = rpm_to_kph(rpm);
    constexpr int number_scale = 6;
    constexpr int unit_scale = 2;
    const int unit_x = x + digit_count(kph) * 6 * number_scale + 10;

    fb_number(fb, x, y, kph, number_scale);
    fb_text(fb, unit_x, y + 28, "KM/H", unit_scale);
    speed_bar(fb, x, y + 62, 240, 24, kph);
}
