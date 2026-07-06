// Not a correctness test -- a visual aid. Renders the same widget layout as
// src/core/app_wiring.cpp's display_update() to a 24-bit BMP so anyone
// (Windows/macOS/Linux, no image tool needed) can open it and see the
// per-widget allocated space. Run: pio test -e native -f test_render_layout
#include <unity.h>
#include "framebuffer.h"
#include "modules/widgets/widget_speed.h"
#include "modules/widgets/widget_battery.h"
#include "modules/widgets/widget_warnings.h"
#include "modules/widgets/widget_gear.h"
#include <cstdio>
#include <cstdint>
#include <vector>
#include <filesystem>

namespace {

void write_bmp(const char *path, int w, int h, const std::vector<uint8_t> &rgb) {
    int row_stride = w * 3;
    int pad = (4 - (row_stride % 4)) % 4;
    int data_size = (row_stride + pad) * h;
    int file_size = 54 + data_size;

    uint8_t header[54] = {0};
    header[0] = 'B'; header[1] = 'M';
    *(int32_t *)&header[2]  = file_size;
    *(int32_t *)&header[10] = 54;
    *(int32_t *)&header[14] = 40;
    *(int32_t *)&header[18] = w;
    *(int32_t *)&header[22] = h;
    *(int16_t *)&header[26] = 1;
    *(int16_t *)&header[28] = 24;
    *(int32_t *)&header[34] = data_size;

    std::FILE *f = std::fopen(path, "wb");
    std::fwrite(header, 1, 54, f);
    uint8_t padbuf[3] = {0, 0, 0};
    for (int y = h - 1; y >= 0; y--) {
        const uint8_t *row = &rgb[y * row_stride];
        for (int x = 0; x < w; x++) {
            uint8_t px[3] = { row[x * 3 + 2], row[x * 3 + 1], row[x * 3 + 0] };
            std::fwrite(px, 1, 3, f);
        }
        if (pad) std::fwrite(padbuf, 1, pad, f);
    }
    std::fclose(f);
}

}  // namespace

void test_render_layout_writes_bmp(void) {
    FrameBuffer fb;
    fb.clear();

    // Same calls/coordinates as src/core/app_wiring.cpp display_update()
    widget_speed_draw(fb,    10,  10, 87);
    widget_battery_draw(fb,  10, 120, 62);
    widget_warnings_draw(fb, 250, 10, true, true);
    widget_gear_draw(fb,     270, 110, 2 /* D */);

    // Bounding boxes sized to worst-case content per widget.
    struct Box { int x, y, w, h; };
    Box boxes[] = {
        {10,  10,  96, 28},    // speed:    4 digits @ scale4
        {10, 120,  96, 12},    // battery:  bar(60x12) + "100%" @ scale1
        {250, 10,  36, 16},    // warnings: two 16x16 boxes, 20px pitch
        {270, 110, 18, 21},    // gear:     1 char @ scale3
    };
    for (auto &b : boxes) fb_rect(fb, b.x - 2, b.y - 2, b.w + 4, b.h + 4, false, true);

    const int SCALE = 3;
    int W = FB_W * SCALE, H = FB_H * SCALE;
    std::vector<uint8_t> rgb((size_t)W * H * 3);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            bool on = fb.get(x / SCALE, y / SCALE);
            uint8_t v = on ? 0 : 255;
            size_t i = ((size_t)y * W + x) * 3;
            rgb[i] = rgb[i + 1] = rgb[i + 2] = v;
        }
    }

    const char *path = "render_layout.bmp";
    write_bmp(path, W, H, rgb);

    std::FILE *check = std::fopen(path, "rb");
    TEST_ASSERT_NOT_NULL_MESSAGE(check, "failed to write render_layout.bmp");
    if (check) std::fclose(check);

    auto full_path = std::filesystem::absolute(path);
    TEST_MESSAGE(full_path.string().c_str());
}

void setUp(void) {}
void tearDown(void) {}

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_render_layout_writes_bmp);
    return UNITY_END();
}
