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

void fb_text(FrameBuffer &fb, int x, int y, const char *s, int scale) {
    if (scale < 1) scale = 1;
    int cx = x;
    for (const char *p = s; *p; ++p) {
        const uint8_t *g = font_glyph(*p);
        if (g) {
            for (int r = 0; r < 7; r++)
                for (int c = 0; c < 5; c++)
                    if (g[r] & (0x10 >> c))
                        fb_rect(fb, cx + c * scale, y + r * scale, scale, scale, true, true);
        }
        cx += 6 * scale;   // 5px glyph + 1px spacing
    }
}

void fb_number(FrameBuffer &fb, int x, int y, int value, int scale) {
    char rev[12]; int r = 0;
    bool neg = value < 0;
    unsigned v = neg ? (unsigned)(-(long)value) : (unsigned)value;
    if (v == 0) rev[r++] = '0';
    while (v > 0) { rev[r++] = (char)('0' + v % 10); v /= 10; }
    char buf[13]; int i = 0;
    if (neg) buf[i++] = '-';
    while (r > 0) buf[i++] = rev[--r];
    buf[i] = '\0';
    fb_text(fb, x, y, buf, scale);
}
