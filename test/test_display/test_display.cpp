#include <unity.h>
#include "modules/display.h"

void test_rounds_and_picks_max_temp(void) {
    DisplayModel m = display_compute({1234.6f, 0.55f, 40, 70, 0, true});
    TEST_ASSERT_EQUAL_INT(1235, m.speed);
    TEST_ASSERT_EQUAL_INT(55, m.soc_pct);
    TEST_ASSERT_EQUAL_INT(70, m.temp_max);
    TEST_ASSERT_FALSE(m.fault);
    TEST_ASSERT_TRUE(m.hv);
}
void test_fault_flag(void) {
    DisplayModel m = display_compute({0.0f, 0.0f, 0, 0, 0x08, false});
    TEST_ASSERT_TRUE(m.fault);
}

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_rounds_and_picks_max_temp);
    RUN_TEST(test_fault_flag);
    return UNITY_END();
}
