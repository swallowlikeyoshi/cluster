// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#pragma once
#include <cstdint>
// [LOCKED] 1bpp framebuffer + pure drawing helpers. Widgets draw into this;
// the locked display_blit pushes it to the physical panel.

constexpr int FB_W = 320;
constexpr int FB_H = 160;
constexpr int FB_STRIDE = FB_W / 8;   // 40 bytes per row

struct FrameBuffer {
    uint8_t bits[FB_STRIDE * FB_H];
    void clear();
    void pixel(int x, int y, bool on);
    bool get(int x, int y) const;
};

// Primitives (implemented in framebuffer.cpp)
void fb_hline(FrameBuffer &fb, int x, int y, int w, bool on);
void fb_vline(FrameBuffer &fb, int x, int y, int h, bool on);
void fb_rect(FrameBuffer &fb, int x, int y, int w, int h, bool fill, bool on);
void fb_bar(FrameBuffer &fb, int x, int y, int w, int h, int pct);   // outline + proportional fill

// Text (font5x7.cpp + framebuffer.cpp; implemented in Task 2)
const uint8_t *font_glyph(char c);   // 7 rows, low 5 bits (bit4 = leftmost col), or nullptr
void fb_text(FrameBuffer &fb, int x, int y, const char *s, int scale);
void fb_number(FrameBuffer &fb, int x, int y, int value, int scale);
