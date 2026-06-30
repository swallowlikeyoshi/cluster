// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#include "core/wiring.h"
#include "state.h"
#include "core/can_bus.h"
#include "core/io_expander.h"
#include "core/display_render.h"
#include "modules/indicators.h"
#include "modules/vess.h"
#include "modules/hmi_input.h"
#include "modules/display.h"
#include <Arduino.h>

// [LOCKED] The ONLY translation unit that touches `state`.
ClusterState state;

namespace {
    constexpr int PIN_VESS = 25;          // buzzer PWM pin
    uint16_t prev_buttons = 0;
}

static void can_rx_update() { can_bus::poll_rx(); }

static void hmi_update() {
    uint16_t buttons = io_expander::read_inputs();
    HmiOutput o = hmi_compute({ buttons, prev_buttons, state.drive_mode });
    prev_buttons = buttons;
    if (o.drive_mode != state.drive_mode || o.reset_req) {
        state.drive_mode = o.drive_mode;
        state.reset_req  = o.reset_req;
        can_bus::send_command(o.reset_req ? 2 : 1, o.drive_mode);  // 2=reset,1=set mode
        state.reset_req = false;
    }
}

static void indicators_update() {
    uint16_t bits = indicators_compute({ state.error1, state.hv_active, state.brake });
    io_expander::write_outputs(bits);
}

static void vess_update() {
    int hz = vess_compute({ state.speed_rpm, state.hv_active });
    if (hz > 0) ledcWriteTone(0, hz); else ledcWrite(0, 0);
}

static void display_update() {
    DisplayModel m = display_compute({
        state.speed_rpm, state.soc, state.controller_temp, state.motor_temp,
        state.error1, state.hv_active });
    display::render(m);
}

Task g_tasks[] = {
    { can_rx_update,     5, 0 },   // 200 Hz drain
    { hmi_update,       20, 0 },   // 50 Hz
    { indicators_update,50, 0 },   // 20 Hz
    { vess_update,      20, 0 },   // 50 Hz
    { display_update,  100, 0 },   // 10 Hz
};
const int G_TASK_COUNT = sizeof(g_tasks) / sizeof(g_tasks[0]);

void modules_init() {
    can_bus::begin();
    io_expander::begin();
    display::begin();
    ledcSetup(0, 2000, 8);     // buzzer PWM channel 0
    ledcAttachPin(PIN_VESS, 0);
}
