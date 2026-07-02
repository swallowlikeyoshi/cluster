// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#include "core/wiring.h"
#include "state.h"
#include "core/can_bus.h"
#include "core/display_blit.h"
#include "framebuffer.h"
#include "modules/hmi_input.h"
#include "modules/widgets/widget_speed.h"
#include "modules/widgets/widget_battery.h"
#include "modules/widgets/widget_warnings.h"
#include "modules/widgets/widget_gear.h"
#include <Arduino.h>

// [LOCKED] The ONLY translation unit that touches `state`.
ClusterState state;

namespace {
    // Input pins (direct GPIO; if pin count runs short, an io_expander can be
    // reintroduced HERE only, without touching any module).
    constexpr int PIN_GEAR_R  = 32;
    constexpr int PIN_GEAR_D  = 33;
    constexpr int PIN_PADDOCK = 26;
    FrameBuffer fb;

    // Map an EZkontrol gear code (state.gear, 0..7) to the widget's 0=N/1=R/2=D.
    int gear_code(uint8_t ez) { return ez == 1 ? 1 : ez == 3 ? 2 : 0; }  // 1=R, 3=D1 -> D, else N
}

static void hmi_update() {
    HmiSwitches sw;
    sw.gear_raw    = digitalRead(PIN_GEAR_R) == LOW ? 1 : digitalRead(PIN_GEAR_D) == LOW ? 2 : 0;
    sw.paddock     = digitalRead(PIN_PADDOCK) == LOW;
    sw.config_bits = 0;
    ClusterCommand cmd = hmi_compute(sw);
    state.drive_mode = cmd.drive_mode;
    state.reset_req  = false;
    can_bus::send_command(cmd);
}

static void can_rx_update() { can_bus::poll_rx(); }

static void display_update() {
    fb.clear();
    widget_speed_draw(fb,    10,  10, (int)state.speed_rpm);
    widget_battery_draw(fb,  10, 120, (int)(state.soc * 100.0f));
    widget_warnings_draw(fb, 250, 10, state.error1 != 0, state.hv_active);
    widget_gear_draw(fb,     270, 110, gear_code(state.gear));
    display_blit::show(fb);
}

Task g_tasks[] = {
    { can_rx_update,   5, 0 },   // 200 Hz drain
    { hmi_update,     20, 0 },   // 50 Hz
    { display_update, 66, 0 },   // ~15 Hz
};
const int G_TASK_COUNT = sizeof(g_tasks) / sizeof(g_tasks[0]);

void modules_init() {
    pinMode(PIN_GEAR_R,  INPUT_PULLUP);
    pinMode(PIN_GEAR_D,  INPUT_PULLUP);
    pinMode(PIN_PADDOCK, INPUT_PULLUP);
    can_bus::begin();
    display_blit::begin();
}
