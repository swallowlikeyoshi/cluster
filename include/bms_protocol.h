#pragma once
#include <cstdint>

struct BmsSummary {
    float    pack_voltage_v = 0.0f;
    float    current_a = 0.0f;
    int      temp_c = 0;
    uint32_t remaining_mah = 0;
    uint8_t  soc_pct = 0;
    uint8_t  soh_pct = 0;
    uint16_t cycles = 0;
};

int bms_expected_frame_len(uint8_t id);
bool bms_decode_summary_frame(const uint8_t *frame, int len, BmsSummary &out);
