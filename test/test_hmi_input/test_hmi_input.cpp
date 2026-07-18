#include <unity.h>
#include "modules/hmi_input.h"

void test_gear_maps(void) {
    TEST_ASSERT_TRUE(Gear::N == hmi_compute({0, false, 0}).gear);
    TEST_ASSERT_TRUE(Gear::R == hmi_compute({1, false, 0}).gear);
    TEST_ASSERT_TRUE(Gear::D == hmi_compute({2, false, 0}).gear);
}
void test_gear_out_of_range_defaults_N(void) {
    TEST_ASSERT_TRUE(Gear::N == hmi_compute({9, false, 0}).gear);
}
void test_paddock_passthrough(void) {
    TEST_ASSERT_TRUE(hmi_compute({2, true, 0}).paddock);
    TEST_ASSERT_FALSE(hmi_compute({2, false, 0}).paddock);
}
void test_drive_mode_from_config(void) {
    TEST_ASSERT_EQUAL_UINT8(2, hmi_compute({0, false, 0x02}).drive_mode);
}
void test_vess_defaults_on(void) {
    TEST_ASSERT_TRUE(hmi_compute({0, false, 0}).vess_enabled);
}
void test_vess_passthrough(void) {
    TEST_ASSERT_TRUE(hmi_compute({0, false, 0, true}).vess_enabled);
    TEST_ASSERT_FALSE(hmi_compute({0, false, 0, false}).vess_enabled);
}

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_gear_maps);
    RUN_TEST(test_gear_out_of_range_defaults_N);
    RUN_TEST(test_paddock_passthrough);
    RUN_TEST(test_drive_mode_from_config);
    RUN_TEST(test_vess_defaults_on);
    RUN_TEST(test_vess_passthrough);
    return UNITY_END();
}
