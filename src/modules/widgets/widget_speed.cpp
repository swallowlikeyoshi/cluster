// [FILL-IN] Edit this file. Draw your widget into the framebuffer.
#include "modules/widgets/widget_speed.h"

namespace {
constexpr float WHEEL_DIAMETER_M = 0.4597f;   // 45.97 cm
constexpr float GEAR_RATIO = 3.72f;
constexpr float PI_F = 3.14159265f;

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

int speed_scale(int kph) {
    const int digits = digit_count(kph);
    if (digits <= 1) return 16;
    if (digits == 2) return 15;
    return 12;
}
}

void widget_speed_draw(FrameBuffer &fb, int x, int y, int rpm) {
    int kph = rpm_to_kph(rpm);
    if (kph > 999) kph = 999;
    fb_number(fb, x, y, kph, speed_scale(kph));
}
