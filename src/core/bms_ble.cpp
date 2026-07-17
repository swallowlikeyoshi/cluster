#include "core/bms_ble.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "state.h"

namespace bms_ble {
namespace {
constexpr char BMS_NAME[] = "LWS-1608";
constexpr uint32_t SCAN_RETRY_MS = 15000;
constexpr uint32_t SUMMARY_POLL_MS = 1000;
constexpr uint32_t STALE_MS = 5000;
constexpr int SCAN_SECONDS = 1;

NimBLEUUID SVC("FFE0");
NimBLEUUID CH_WR("FFE1");
NimBLEUUID CH_NFY("FFE2");

NimBLEClient *client = nullptr;
NimBLERemoteCharacteristic *write_ch = nullptr;
uint32_t last_scan_ms = 0;
uint32_t last_summary_poll_ms = 0;
bool initialized = false;

uint8_t frame_buf[64];
int frame_len_used = 0;

uint16_t u16le(const uint8_t *d) {
    return (uint16_t)(d[0] | ((uint16_t)d[1] << 8));
}

bool checksum_ok(const uint8_t *f, int len) {
    if (len < 8) return false;
    uint16_t sum = 0;
    for (int i = 1; i < len - 4; ++i) {
        sum = (uint16_t)(sum + f[i]);
    }
    return u16le(f + len - 4) == sum;
}

int expected_frame_len(uint8_t id) {
    switch (id) {
        case 0x24: return 36;  // 14 cell voltages
        case 0x2A: return 32;  // summary: V/I/temp/capacity/SOC
        case 0x25: return 32;
        case 0x2B: return 32;
        case 0x10: return 15;
        default: return 0;
    }
}

void mark_disconnected() {
    write_ch = nullptr;
    frame_len_used = 0;
    state.bms_ble_connected = false;
    if (millis() - state.bms_last_rx_ms > STALE_MS) {
        state.soc_valid = false;
    }
}

void parse_summary(const uint8_t *f) {
    const uint16_t pack_mv = u16le(f + 8);
    const int16_t current_ma = (int16_t)u16le(f + 10);
    const uint16_t remain_mah = u16le(f + 16);
    uint8_t soc_pct = f[24];
    if (soc_pct > 100) soc_pct = 100;

    state.bms_pack_voltage = (float)pack_mv * 0.001f;
    state.bms_current = (float)current_ma * 0.001f;
    state.bms_temp_c = f[12];
    state.bms_remaining_mah = remain_mah;
    state.soc = (float)soc_pct * 0.01f;
    state.soc_valid = true;
    state.bms_soh = f[25];
    state.bms_cycles = u16le(f + 26);
    state.bms_last_rx_ms = millis();
    state.bms_ble_connected = true;
}

void handle_frame(const uint8_t *f) {
    if (f[2] == 0x2A) parse_summary(f);
}

void feed_byte(uint8_t b) {
    if (frame_len_used == 0 && b != 0x3A) return;
    if (frame_len_used == 1 && b != 0x16) {
        frame_len_used = 0;
        return;
    }

    if (frame_len_used >= (int)sizeof(frame_buf)) {
        frame_len_used = 0;
        return;
    }
    frame_buf[frame_len_used++] = b;

    if (frame_len_used < 3) return;
    const int want = expected_frame_len(frame_buf[2]);
    if (want == 0) {
        frame_len_used = 0;
        return;
    }
    if (frame_len_used < want) return;

    if (frame_buf[want - 2] == 0x0D && frame_buf[want - 1] == 0x0A &&
        checksum_ok(frame_buf, want)) {
        handle_frame(frame_buf);
    }
    frame_len_used = 0;
}

void on_notify(NimBLERemoteCharacteristic*, uint8_t *data, size_t len, bool) {
    for (size_t i = 0; i < len; ++i) feed_byte(data[i]);
}

void write_raw(const uint8_t *data, size_t len) {
    if (write_ch) write_ch->writeValue((uint8_t*)data, len, false);
}

void send_handshake() {
    const uint8_t h1[] = { 0x00, 0xA1, 0xA2, 0xA3 };
    write_raw(h1, sizeof(h1));
    delay(80);

    const uint8_t h2[] = {
        0xAA, 0x55, 0xF3, 0x01, 0x77, 0x77, 0x7C, 0x3F, 0x5A, 0xF3
    };
    write_raw(h2, sizeof(h2));
    delay(80);
}

void poll_frame(uint8_t id) {
    const uint8_t checksum = (uint8_t)((0x16 + id) & 0xFF);
    const uint8_t frame[] = { 0x3A, 0x16, id, 0x00, checksum, 0x00, 0x0D, 0x0A };
    write_raw(frame, sizeof(frame));
}

bool matches_bms(NimBLEAdvertisedDevice &dev) {
    return dev.haveName() && dev.getName() == BMS_NAME;
}

bool connect_device(NimBLEAdvertisedDevice &dev) {
    if (!client) client = NimBLEDevice::createClient();
    if (!client) return false;

    if (client->isConnected()) client->disconnect();
    if (!client->connect(&dev)) {
        mark_disconnected();
        return false;
    }

    NimBLERemoteService *svc = client->getService(SVC);
    if (!svc) {
        client->disconnect();
        mark_disconnected();
        return false;
    }

    NimBLERemoteCharacteristic *notify_ch = svc->getCharacteristic(CH_NFY);
    write_ch = svc->getCharacteristic(CH_WR);
    if (!notify_ch || !write_ch || !notify_ch->canNotify()) {
        client->disconnect();
        mark_disconnected();
        return false;
    }

    if (!notify_ch->subscribe(true, on_notify)) {
        client->disconnect();
        mark_disconnected();
        return false;
    }

    state.bms_ble_connected = false;
    frame_len_used = 0;
    send_handshake();
    poll_frame(0x2A);
    last_summary_poll_ms = millis();
    return true;
}

bool scan_and_connect() {
    NimBLEScan *scan = NimBLEDevice::getScan();
    if (!scan) return false;

    scan->setActiveScan(true);
    NimBLEScanResults results = scan->start(SCAN_SECONDS, false);
    for (int i = 0; i < results.getCount(); ++i) {
        NimBLEAdvertisedDevice dev = results.getDevice(i);
        if (matches_bms(dev)) {
            const bool ok = connect_device(dev);
            scan->clearResults();
            return ok;
        }
    }

    scan->clearResults();
    mark_disconnected();
    return false;
}

bool connected() {
    return client && client->isConnected() && write_ch;
}
}

void begin() {
    NimBLEDevice::init("");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    initialized = true;
    last_scan_ms = millis() - SCAN_RETRY_MS;
}

void poll() {
    if (!initialized) return;

    const uint32_t now = millis();
    if (!connected()) {
        mark_disconnected();
        if (now - last_scan_ms >= SCAN_RETRY_MS) {
            last_scan_ms = now;
            scan_and_connect();
        }
        return;
    }

    const bool summary_fresh =
        state.bms_last_rx_ms != 0 && now - state.bms_last_rx_ms <= STALE_MS;
    state.bms_ble_connected = summary_fresh;
    if (!summary_fresh) {
        state.soc_valid = false;
    }

    if (now - last_summary_poll_ms >= SUMMARY_POLL_MS) {
        last_summary_poll_ms = now;
        poll_frame(0x2A);
    }
}
}
