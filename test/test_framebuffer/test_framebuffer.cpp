#include <unity.h>
#include "framebuffer.h"

void test_pixel_set_get(void) {
    FrameBuffer fb; fb.clear();
    TEST_ASSERT_FALSE(fb.get(10, 10));
    fb.pixel(10, 10, true);
    TEST_ASSERT_TRUE(fb.get(10, 10));
    fb.pixel(10, 10, false);
    TEST_ASSERT_FALSE(fb.get(10, 10));
}
void test_clip_out_of_bounds(void) {
    FrameBuffer fb; fb.clear();
    fb.pixel(-1, 0, true);      // must not crash / must be ignored
    fb.pixel(FB_W, 0, true);
    fb.pixel(0, FB_H, true);
    TEST_ASSERT_FALSE(fb.get(-1, 0));
    TEST_ASSERT_FALSE(fb.get(0, FB_H));
}
void test_rect_outline(void) {
    FrameBuffer fb; fb.clear();
    fb_rect(fb, 5, 5, 4, 4, false, true);       // outline
    TEST_ASSERT_TRUE(fb.get(5, 5));             // corner on
    TEST_ASSERT_TRUE(fb.get(8, 8));             // opposite corner
    TEST_ASSERT_FALSE(fb.get(6, 6));            // interior off (outline only)
}
void test_bar_fills_proportional(void) {
    FrameBuffer fb; fb.clear();
    fb_bar(fb, 0, 0, 12, 6, 50);                // 50% of inner width (10) = 5
    TEST_ASSERT_TRUE(fb.get(1, 1));             // filled near left
    TEST_ASSERT_FALSE(fb.get(9, 3));            // right part empty at 50%
}

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_pixel_set_get);
    RUN_TEST(test_clip_out_of_bounds);
    RUN_TEST(test_rect_outline);
    RUN_TEST(test_bar_fills_proportional);
    return UNITY_END();
}
