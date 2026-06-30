#include <unity.h>
#include "modules/vess.h"

void test_silent_when_inactive(void) { TEST_ASSERT_EQUAL_INT(0, vess_compute({1000.0f, false})); }
void test_base_tone_at_zero(void)    { TEST_ASSERT_EQUAL_INT(200, vess_compute({0.0f, true})); }
void test_rises_with_speed(void)     { TEST_ASSERT_TRUE(vess_compute({1000.0f, true}) > vess_compute({0.0f, true})); }
void test_clamped_high(void)         { TEST_ASSERT_EQUAL_INT(2000, vess_compute({999999.0f, true})); }

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_silent_when_inactive);
    RUN_TEST(test_base_tone_at_zero);
    RUN_TEST(test_rises_with_speed);
    RUN_TEST(test_clamped_high);
    return UNITY_END();
}
