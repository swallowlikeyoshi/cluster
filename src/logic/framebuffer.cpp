// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#include "framebuffer.h"

void FrameBuffer::clear() { for (int i = 0; i < FB_STRIDE * FB_H; i++) bits[i] = 0; }

void FrameBuffer::pixel(int x, int y, bool on) {
    if (x < 0 || x >= FB_W || y < 0 || y >= FB_H) return;   // clip
    uint8_t &b = bits[y * FB_STRIDE + (x >> 3)];
    uint8_t m = 0x80 >> (x & 7);
    if (on) b |= m; else b &= (uint8_t)~m;
}

bool FrameBuffer::get(int x, int y) const {
    if (x < 0 || x >= FB_W || y < 0 || y >= FB_H) return false;
    return (bits[y * FB_STRIDE + (x >> 3)] & (0x80 >> (x & 7))) != 0;
}

void fb_hline(FrameBuffer &fb, int x, int y, int w, bool on) { for (int i = 0; i < w; i++) fb.pixel(x + i, y, on); }
void fb_vline(FrameBuffer &fb, int x, int y, int h, bool on) { for (int i = 0; i < h; i++) fb.pixel(x, y + i, on); }

void fb_rect(FrameBuffer &fb, int x, int y, int w, int h, bool fill, bool on) {
    if (fill) { for (int j = 0; j < h; j++) fb_hline(fb, x, y + j, w, on); }
    else {
        fb_hline(fb, x, y, w, on); fb_hline(fb, x, y + h - 1, w, on);
        fb_vline(fb, x, y, h, on); fb_vline(fb, x + w - 1, y, h, on);
    }
}

void fb_bar(FrameBuffer &fb, int x, int y, int w, int h, int pct) {
    if (pct < 0) pct = 0; if (pct > 100) pct = 100;
    fb_rect(fb, x, y, w, h, false, true);          // outline
    int fillw = (w - 2) * pct / 100;
    if (fillw > 0) fb_rect(fb, x + 1, y + 1, fillw, h - 2, true, true);
}
