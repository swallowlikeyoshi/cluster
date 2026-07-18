// LWS-1608 BMS reader for ESP32 (Arduino / NimBLE-Arduino).
//
// Connects to a BLE BMS advertising as "LWS-1608", performs the handshake,
// polls summary (0x2A) and cell-voltage (0x24) frames, and prints telemetry.
//
// Source protocol/code:
// https://github.com/swallowlikeyoshi/bexel-v2-bluetooth-protocol

#include <Arduino.h>
#include <NimBLEDevice.h>

struct BmsTelemetry {
    uint16_t pack_mv = 0;
    int16_t current_ma = 0;
    uint16_t remain_mah = 0;
    uint16_t cell_mv[14] = {};
    uint8_t soc = 0;
    uint8_t soh = 0;
    uint8_t temp[4] = {};
    uint8_t cycles = 0;
};

static BmsTelemetry g_bms;
static uint8_t g_buf[64];
static int g_len = 0;

static NimBLEUUID SVC("FFE0");
static NimBLEUUID CH_NOTIFY("FFE2");
static NimBLEUUID CH_WRITE("FFE1");

static const NimBLEAdvertisedDevice *g_target = nullptr;
static NimBLERemoteCharacteristic *g_write = nullptr;

static uint16_t u16le(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static int frame_len(uint8_t id) {
    switch (id) {
        case 0x24: return 36;
        case 0x2A: return 32;
        case 0x25: return 32;
        case 0x2B: return 32;
        case 0x10: return 15;
        default: return 0;
    }
}

static void print_summary() {
    Serial.printf(
        "Pack %.2fV  I %dmA  SOC %u%%  SOH %u%%  T %uC  %umAh  cyc %u\n",
        g_bms.pack_mv / 1000.0f,
        g_bms.current_ma,
        g_bms.soc,
        g_bms.soh,
        g_bms.temp[0],
        g_bms.remain_mah,
        g_bms.cycles
    );
}

static void print_cells() {
    Serial.print("Cells:");
    for (int i = 0; i < 14; i++) Serial.printf(" %u", g_bms.cell_mv[i]);
    Serial.println(" mV");
}

static void parse_frame(const uint8_t *f) {
    switch (f[2]) {
        case 0x2A:
            g_bms.pack_mv = u16le(f + 8);
            g_bms.current_ma = (int16_t)u16le(f + 10);
            for (int i = 0; i < 4; i++) g_bms.temp[i] = f[12 + i];
            g_bms.remain_mah = u16le(f + 16);
            g_bms.soc = f[24];
            g_bms.soh = f[25];
            g_bms.cycles = f[26];
            print_summary();
            break;
        case 0x24:
            for (int i = 0; i < 14; i++) g_bms.cell_mv[i] = u16le(f + 4 + i * 2);
            print_cells();
            break;
        default:
            break;
    }
}

static void feed(uint8_t b) {
    if (g_len == 0 && b != 0x3A) return;
    if (g_len == 1 && b != 0x16) {
        g_len = 0;
        return;
    }
    if (g_len >= (int)sizeof(g_buf)) {
        g_len = 0;
        return;
    }

    g_buf[g_len++] = b;
    if (g_len < 3) return;

    int len = frame_len(g_buf[2]);
    if (len == 0) {
        g_len = 0;
        return;
    }
    if (g_len < len) return;

    if (g_buf[len - 2] == 0x0D && g_buf[len - 1] == 0x0A) parse_frame(g_buf);
    g_len = 0;
}

static void on_notify(NimBLERemoteCharacteristic *, uint8_t *data, size_t len, bool) {
    for (size_t i = 0; i < len; i++) feed(data[i]);
}

static void poll_frame(uint8_t id) {
    uint8_t checksum = (uint8_t)((0x16 + id) & 0xff);
    uint8_t frame[] = {0x3A, 0x16, id, 0x00, checksum, 0x00, 0x0D, 0x0A};
    if (g_write) g_write->writeValue(frame, sizeof(frame), false);
}

class ScanCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice *device) override {
        if (device->getName() == "LWS-1608" || device->isAdvertisingService(SVC)) {
            g_target = device;
            NimBLEDevice::getScan()->stop();
        }
    }
};

static bool connect_bms() {
    NimBLEScan *scan = NimBLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(new ScanCallbacks());
    scan->setActiveScan(true);

    Serial.println("Scanning for LWS-1608...");
    scan->start(5, false);
    if (!g_target) {
        Serial.println("LWS-1608 not found");
        return false;
    }

    NimBLEClient *client = NimBLEDevice::createClient();
    if (!client->connect(g_target)) {
        Serial.println("connect failed");
        return false;
    }

    NimBLERemoteService *svc = client->getService(SVC);
    if (!svc) {
        Serial.println("service FFE0 not found");
        return false;
    }

    NimBLERemoteCharacteristic *notify = svc->getCharacteristic(CH_NOTIFY);
    g_write = svc->getCharacteristic(CH_WRITE);
    if (!notify || !g_write) {
        Serial.println("characteristic FFE1/FFE2 not found");
        return false;
    }

    if (!notify->subscribe(true, on_notify)) {
        Serial.println("subscribe FFE2 failed");
        return false;
    }

    uint8_t h1[] = {0x00, 0xA1, 0xA2, 0xA3};
    g_write->writeValue(h1, sizeof(h1), false);
    delay(100);

    uint8_t h2[] = {0xAA, 0x55, 0xF3, 0x01, 0x77, 0x77, 0x7C, 0x3F, 0x5A, 0xF3};
    g_write->writeValue(h2, sizeof(h2), false);
    delay(100);

    Serial.println("connected, polling...");
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(200);
    NimBLEDevice::init("");
    connect_bms();
}

void loop() {
    poll_frame(0x2A);
    delay(200);
    poll_frame(0x24);
    delay(800);
}
