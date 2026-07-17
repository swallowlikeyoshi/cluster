// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#include "core/wiring.h"
#include "state.h"
#include "core/can_bus.h"
#include "core/display_blit.h"
#include "core/bms_ble.h"
#include "core/gps_laptimer.h"
#include "framebuffer.h"
#include "modules/hmi_input.h"
#include "modules/widgets/widget_speed.h"
#include "modules/widgets/widget_battery.h"
#include "modules/widgets/widget_warnings.h"
#include "modules/widgets/widget_gear.h"
#include "modules/widgets/widget_laptime.h"
#include <Arduino.h>

// [LOCKED] The ONLY translation unit that touches `state`.
ClusterState state;

namespace {
    // Input pins (direct GPIO; if pin count runs short, an io_expander can be
    // reintroduced HERE only, without touching any module).
    constexpr int PIN_GEAR_R  = 32;
    constexpr int PIN_GEAR_D  = 33;
    constexpr int PIN_PADDOCK = 26;
    constexpr int PIN_LCD_ACTION = 27;
    constexpr uint32_t CAN_STARTUP_GRACE_MS = 3000;
    constexpr uint32_t CONTROLLER_FRAME_TIMEOUT_MS = 300;
    constexpr uint32_t VCU_STATUS_TIMEOUT_MS = 300;
    FrameBuffer fb;
    bool warning_detail_page = false;
    bool warning_button_down = false;
    uint32_t warning_button_last_ms = 0;

    int gear_code(uint8_t gear) { return gear <= 3 ? gear : 0; }

    float absf(float v) { return v < 0.0f ? -v : v; }

    bool frame_fresh(uint32_t last_ms, uint32_t now, uint32_t timeout_ms) {
        return last_ms != 0 && (now - last_ms) <= timeout_ms;
    }

    bool frame_stale(uint32_t last_ms, uint32_t now, uint32_t timeout_ms) {
        return !frame_fresh(last_ms, now, timeout_ms);
    }

    uint8_t command_gear_code(Gear gear) {
        switch (gear) {
            case Gear::R: return 1;
            case Gear::D: return 2;
            default: return 0;
        }
    }

    bool controller_fault_active() {
        return state.error1 || state.error2 || state.error3 ||
               state.error1_r || state.error2_r || state.error3_r;
    }

    bool controller_feedback_stale(uint32_t now) {
        if (now < CAN_STARTUP_GRACE_MS) return false;
        return frame_stale(state.controller_l_fb1_last_ms, now, CONTROLLER_FRAME_TIMEOUT_MS) ||
               frame_stale(state.controller_l_fb2_last_ms, now, CONTROLLER_FRAME_TIMEOUT_MS) ||
               frame_stale(state.controller_r_fb1_last_ms, now, CONTROLLER_FRAME_TIMEOUT_MS) ||
               frame_stale(state.controller_r_fb2_last_ms, now, CONTROLLER_FRAME_TIMEOUT_MS);
    }

    bool vcu_status_stale(uint32_t now) {
        return state.vcu_cluster_status_last_ms != 0 &&
               frame_stale(state.vcu_cluster_status_last_ms, now, VCU_STATUS_TIMEOUT_MS);
    }

    bool can_link_warning_active(uint32_t now) {
        return controller_feedback_stale(now) || vcu_status_stale(now);
    }

    bool warning_active() {
        const uint32_t now = millis();
        return controller_fault_active() || can_link_warning_active(now);
    }

    bool bit_any(uint8_t left, uint8_t right, uint8_t bit) {
        return (left & (1u << bit)) || (right & (1u << bit));
    }

    void add_warning(const char *labels[], int &count, const char *label) {
        if (count < 8) labels[count++] = label;
    }

    void warning_line(int &y, const char *label, int scale, int step) {
        fb_text(fb, 18, y, label, scale);
        y += step;
    }

    void draw_warning_detail() {
        const char *labels[8];
        int count = 0;
        const uint32_t now = millis();

        if (bit_any(state.error1, state.error1_r, 2)) add_warning(labels, count, "OVER VOLT");
        if (bit_any(state.error1, state.error1_r, 3)) add_warning(labels, count, "LOW VOLT");
        if (bit_any(state.error1, state.error1_r, 4)) add_warning(labels, count, "CTRL HOT");
        if (bit_any(state.error1, state.error1_r, 5)) add_warning(labels, count, "MOTOR HOT");
        if (state.error1 || state.error2 || state.error3) add_warning(labels, count, "LEFT FAULT");
        if (state.error1_r || state.error2_r || state.error3_r) add_warning(labels, count, "RIGHT FAULT");
        if (bit_any(state.error3, state.error3_r, 4) || can_link_warning_active(now)) {
            add_warning(labels, count, "CAN ERR");
        }
        if (count == 0) add_warning(labels, count, "FAULT");

        fb_text(fb, 18, 14, "WARNING", 5);

        int scale = 5;
        int step = 48;
        int y = count == 1 ? 112 : 78;
        if (count >= 4) {
            scale = 4;
            step = 39;
            y = 76;
        }
        if (count >= 6) {
            scale = 3;
            step = 30;
            y = 74;
        }

        for (int i = 0; i < count; ++i) warning_line(y, labels[i], scale, step);
    }

    void lcd_action_update() {
        const bool warn = warning_active();
        if (warn) {
            gps_laptimer::stop();
        } else {
            warning_detail_page = false;
        }

        const bool down = digitalRead(PIN_LCD_ACTION) == LOW;
        const uint32_t now = millis();
        if (down != warning_button_down && now - warning_button_last_ms >= 50) {
            warning_button_down = down;
            warning_button_last_ms = now;
            if (down) {
                if (warn) {
                    warning_detail_page = !warning_detail_page;
                } else {
                    gps_laptimer::start_at_current_fix();
                }
            }
        }
    }

    void refresh_can_timeouts() {
        const uint32_t now = millis();
        const bool left_speed_fresh =
            frame_fresh(state.controller_l_fb1_last_ms, now, CONTROLLER_FRAME_TIMEOUT_MS);
        const bool right_speed_fresh =
            frame_fresh(state.controller_r_fb1_last_ms, now, CONTROLLER_FRAME_TIMEOUT_MS);

        if (now >= CAN_STARTUP_GRACE_MS) {
            if (left_speed_fresh && right_speed_fresh) {
                state.speed_rpm = (absf(state.speed_rpm_l) + absf(state.speed_rpm_r)) * 0.5f;
            } else if (left_speed_fresh) {
                state.speed_rpm = absf(state.speed_rpm_l);
            } else if (right_speed_fresh) {
                state.speed_rpm = absf(state.speed_rpm_r);
            } else {
                state.speed_rpm = 0.0f;
            }
        }

        if (state.gear_from_can && vcu_status_stale(now)) {
            state.gear_from_can = false;
            state.brake = false;
            state.hv_active = false;
        }
    }
}

static void hmi_update() {
    refresh_can_timeouts();
    lcd_action_update();

    HmiSwitches sw;
    sw.gear_raw    = digitalRead(PIN_GEAR_R) == LOW ? 1 : digitalRead(PIN_GEAR_D) == LOW ? 2 : 0;
    sw.paddock     = digitalRead(PIN_PADDOCK) == LOW;
    sw.config_bits = 0;
    ClusterCommand cmd = hmi_compute(sw);
    if (!state.gear_from_can) {
        state.gear = command_gear_code(cmd.gear);
    }
    state.drive_mode = cmd.drive_mode;
    state.reset_req  = false;
    can_bus::send_command(cmd);
}

static void can_rx_update() { can_bus::poll_rx(); }
static void gps_update() { gps_laptimer::poll(); }
static void bms_update() { bms_ble::poll(); }

static void display_update() {
    fb.clear();
    const bool warn = warning_active();
    if (warn && warning_detail_page) {
        draw_warning_detail();
    } else {
        widget_speed_draw(fb,    10,  10, (int)state.speed_rpm);
        widget_gear_draw(fb,     289,  16, gear_code(state.gear));
        const int soc_pct = state.soc_valid ? (int)(state.soc * 100.0f + 0.5f) : -1;
        widget_battery_draw(fb, 285,  48, soc_pct);
        widget_laptime_draw(fb,  10, 145, state.lap_count,
                            state.current_lap_ms, state.gps_fix_ok);
        widget_warnings_draw(fb, 220, 188, warn, state.hv_active);
    }
    display_blit::show(fb, warn);
}

Task g_tasks[] = {
    { can_rx_update,   5, 0 },   // 200 Hz drain
    { gps_update,     20, 0 },   // 50 Hz UART drain
    { bms_update,    100, 0 },   // 10 Hz BLE BMS state machine
    { hmi_update,     20, 0 },   // 50 Hz
    { display_update, 66, 0 },   // ~15 Hz
};
const int G_TASK_COUNT = sizeof(g_tasks) / sizeof(g_tasks[0]);

void modules_init() {
    pinMode(PIN_GEAR_R,  INPUT_PULLUP);
    pinMode(PIN_GEAR_D,  INPUT_PULLUP);
    pinMode(PIN_PADDOCK, INPUT_PULLUP);
    pinMode(PIN_LCD_ACTION, INPUT_PULLUP);
    can_bus::begin();
    gps_laptimer::begin();
    bms_ble::begin();
    display_blit::begin();
}
