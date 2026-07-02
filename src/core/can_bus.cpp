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

void poll_rx() {
    twai_message_t m;
    while (twai_receive(&m, 0) == ESP_OK) {
        // TODO(core): parse vehicle-state frames into `state`. Receive path is
        // an open decision (docs/CAN_PROTOCOL.md §7). Decoders: raw_to_voltage/
        // raw_to_current/raw_to_temp/raw_to_speed.
        (void)m;
    }
}

void send_command(const ClusterCommand &cmd) {
    twai_message_t m = {};
    m.identifier = CAN_ID_CLUSTER_CMD; m.extd = 1; m.data_length_code = 8;
    encode_cluster_command(cmd, m.data);
    twai_transmit(&m, pdMS_TO_TICKS(5));
}

} // namespace can_bus
