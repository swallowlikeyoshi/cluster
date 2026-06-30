#include <unity.h>
#include "modules/hmi_input.h"

void test_no_change_without_press(void) {
    HmiOutput o = hmi_compute({0, 0, 1});
    TEST_ASSERT_EQUAL_UINT8(1, o.drive_mode);
    TEST_ASSERT_FALSE(o.reset_req);
}
void test_mode_cycles_on_rising_edge(void) {
    HmiOutput o = hmi_compute({0x01, 0x00, 0});   // button0 newly pressed
    TEST_ASSERT_EQUAL_UINT8(1, o.drive_mode);
}
void test_mode_wraps(void) {
    HmiOutput o = hmi_compute({0x01, 0x00, 2});
    TEST_ASSERT_EQUAL_UINT8(0, o.drive_mode);
}
void test_no_cycle_when_held(void) {
    HmiOutput o = hmi_compute({0x01, 0x01, 1});   // still held, not a new edge
    TEST_ASSERT_EQUAL_UINT8(1, o.drive_mode);
}
void test_reset_on_rising_edge(void) {
    HmiOutput o = hmi_compute({0x02, 0x00, 0});
    TEST_ASSERT_TRUE(o.reset_req);
}

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_no_change_without_press);
    RUN_TEST(test_mode_cycles_on_rising_edge);
    RUN_TEST(test_mode_wraps);
    RUN_TEST(test_no_cycle_when_held);
    RUN_TEST(test_reset_on_rising_edge);
    return UNITY_END();
}
