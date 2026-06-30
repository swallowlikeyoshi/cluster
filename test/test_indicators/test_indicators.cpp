#include <unity.h>
#include "modules/indicators.h"

void test_no_lamps_when_clear(void) { TEST_ASSERT_EQUAL_UINT16(0, indicators_compute({0, false, false})); }
void test_fault_lamp(void)          { TEST_ASSERT_TRUE(indicators_compute({0x04, false, false}) & 0x01); }
void test_hv_lamp(void)             { TEST_ASSERT_TRUE(indicators_compute({0, true, false}) & 0x02); }
void test_brake_lamp(void)          { TEST_ASSERT_TRUE(indicators_compute({0, false, true}) & 0x04); }

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_no_lamps_when_clear);
    RUN_TEST(test_fault_lamp);
    RUN_TEST(test_hv_lamp);
    RUN_TEST(test_brake_lamp);
    return UNITY_END();
}
