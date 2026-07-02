#include <unity.h>
#include "framebuffer.h"
#include "modules/widgets/widget_warnings.h"

static int lit_in(FrameBuffer &fb, int x0, int y0, int w, int h) {
    int n = 0; for (int y = y0; y < y0 + h; y++) for (int x = x0; x < x0 + w; x++) if (fb.get(x, y)) n++;
    return n;
}
void test_clear_draws_nothing(void) {
    FrameBuffer fb; fb.clear(); widget_warnings_draw(fb, 0, 0, false, false);
    TEST_ASSERT_EQUAL_INT(0, lit_in(fb, 0, 0, 40, 20));
}
void test_fault_lights_left_icon(void) {
    FrameBuffer fb; fb.clear(); widget_warnings_draw(fb, 0, 0, true, false);
    TEST_ASSERT_TRUE(lit_in(fb, 0, 0, 16, 16) > 0);
}
void test_hv_lights_right_icon(void) {
    FrameBuffer fb; fb.clear(); widget_warnings_draw(fb, 0, 0, false, true);
    TEST_ASSERT_TRUE(lit_in(fb, 20, 0, 16, 16) > 0);
}

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_clear_draws_nothing);
    RUN_TEST(test_fault_lights_left_icon);
    RUN_TEST(test_hv_lights_right_icon);
    return UNITY_END();
}
