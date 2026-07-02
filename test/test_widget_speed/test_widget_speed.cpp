#include <unity.h>
#include "framebuffer.h"
#include "modules/widgets/widget_speed.h"

void test_draws_something(void) {
    FrameBuffer fb; fb.clear();
    widget_speed_draw(fb, 0, 0, 88);
    // count lit pixels in the top-left area; must be > 0
    int lit = 0;
    for (int yy = 0; yy < 40; yy++) for (int xx = 0; xx < 80; xx++) if (fb.get(xx, yy)) lit++;
    TEST_ASSERT_TRUE(lit > 0);
}
void test_zero_speed_draws_zero(void) {
    FrameBuffer fb; fb.clear();
    widget_speed_draw(fb, 0, 0, 0);
    int lit = 0;
    for (int yy = 0; yy < 40; yy++) for (int xx = 0; xx < 40; xx++) if (fb.get(xx, yy)) lit++;
    TEST_ASSERT_TRUE(lit > 0);   // "0" is drawn, not blank
}

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_draws_something);
    RUN_TEST(test_zero_speed_draws_zero);
    return UNITY_END();
}
