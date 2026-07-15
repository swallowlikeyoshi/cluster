// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#include "core/can_bus.h"
#include <Arduino.h>
#include "driver/twai.h"
#include "can_protocol.h"
#include "state.h"

namespace can_bus {

void begin() {
    twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NORMAL);
    twai_timing_config_t  t = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t  f = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_driver_install(&g, &t, &f);
    twai_start();
}

namespace {
    uint16_t u16le(const uint8_t *d) { return (uint16_t)(d[0] | (d[1] << 8)); }

    // Part I: bytes 0-1 voltage, 2-3 bus current, 4-5 phase current (unused), 6-7 speed.
    void decode_fb1(const uint8_t *d, float &voltage, float &current, float &speed) {
        voltage = raw_to_voltage(u16le(d + 0));
        current = raw_to_current(u16le(d + 2));
        speed   = (float)raw_to_speed(u16le(d + 6));
    }

    // Part II: byte 0 controller temp, 1 motor temp, 2 status, 3-5 error bitmaps.
    void decode_fb2(const uint8_t *d, int &ctrl_temp, int &motor_temp,
                     uint8_t &err1, uint8_t &err2, uint8_t &err3) {
        ctrl_temp  = raw_to_temp(d[0]);
        motor_temp = raw_to_temp(d[1]);
        err1 = d[3];
        err2 = d[4];
        err3 = d[5];
    }
}

void poll_rx() {
    twai_message_t m;
    while (twai_receive(&m, 0) == ESP_OK) {
        if (!m.extd || m.data_length_code < 8) continue;
        switch (m.identifier) {
            case CAN_ID_FB1_L:
                decode_fb1(m.data, state.bus_voltage, state.bus_current, state.speed_rpm);
                break;
            case CAN_ID_FB1_R:
                decode_fb1(m.data, state.bus_voltage_r, state.bus_current_r, state.speed_rpm_r);
                break;
            case CAN_ID_FB2_L:
                decode_fb2(m.data, state.controller_temp, state.motor_temp,
                           state.error1, state.error2, state.error3);
                break;
            case CAN_ID_FB2_R:
                decode_fb2(m.data, state.controller_temp_r, state.motor_temp_r,
                           state.error1_r, state.error2_r, state.error3_r);
                break;
            default:
                break;
        }
    }
}

void send_command(const ClusterCommand &cmd) {
    twai_message_t m = {};
    m.identifier = CAN_ID_CLUSTER_CMD; m.extd = 1; m.data_length_code = 8;
    encode_cluster_command(cmd, m.data);
    twai_transmit(&m, pdMS_TO_TICKS(5));
}

} // namespace can_bus
