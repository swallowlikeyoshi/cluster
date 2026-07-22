#include "bms_protocol.h"

namespace {
uint16_t u16le(const uint8_t *d) {
    return (uint16_t)(d[0] | ((uint16_t)d[1] << 8));
}
}

int bms_expected_frame_len(uint8_t id) {
    switch (id) {
        case 0x24: return 36;  // 14 cell voltages
        case 0x2A: return 32;  // summary: V/I/temp/capacity/SOC
        case 0x25: return 32;
        case 0x2B: return 32;
        case 0x10: return 15;
        default: return 0;
    }
}

bool bms_decode_summary_frame(const uint8_t *frame, int len, BmsSummary &out) {
    if (!frame || len != bms_expected_frame_len(0x2A)) return false;
    if (frame[0] != 0x3A || frame[1] != 0x16 || frame[2] != 0x2A) return false;
    if (frame[len - 2] != 0x0D || frame[len - 1] != 0x0A) return false;

    uint8_t soc_pct = frame[24];
    if (soc_pct > 100) soc_pct = 100;

    out.pack_voltage_v = (float)u16le(frame + 8) * 0.001f;
    out.current_a = (float)(int16_t)u16le(frame + 10) * 0.001f;
    out.temp_c = frame[12];
    out.remaining_mah = u16le(frame + 16);
    out.soc_pct = soc_pct;
    out.soh_pct = frame[25];
    out.cycles = u16le(frame + 26);
    return true;
}
