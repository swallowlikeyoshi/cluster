#include <unity.h>
#include "framebuffer.h"
#include "modules/widgets/widget_gear.h"

static int lit(FrameBuffer &fb) {
    int n = 0; for (int y = 0; y < 24; y++) for (int x = 0; x < 20; x++) if (fb.get(x, y)) n++;
    return n;
}
void test_each_valid_gear_draws(void) {
    for (int g = 0; g <= 3; g++) {
        FrameBuffer fb; fb.clear(); widget_gear_draw(fb, 0, 0, g);
        TEST_ASSERT_TRUE(lit(fb) > 0);
    }
}
void test_invalid_gear_blank(void) {
    FrameBuffer fb; fb.clear(); widget_gear_draw(fb, 0, 0, 9);
    TEST_ASSERT_EQUAL_INT(0, lit(fb));
}

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_each_valid_gear_draws);
    RUN_TEST(test_invalid_gear_blank);
    return UNITY_END();
}
