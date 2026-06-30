#include <unity.h>
#include "can_protocol.h"

// shared torque scaling (from VCU base)
void test_torque_offset(void) { TEST_ASSERT_EQUAL_UINT16(32000, torque_to_raw(0.0f)); }
// Cluster additions
void test_cluster_cmd_id(void) { TEST_ASSERT_EQUAL_HEX32(0x1801D0C0, CAN_ID_CLUSTER_CMD); }
void test_feedback_ids(void) {
    TEST_ASSERT_EQUAL_HEX32(0x1801D0EF, CAN_ID_FB1_L);
    TEST_ASSERT_EQUAL_HEX32(0x1802D0EF, CAN_ID_FB2_L);
}
void test_decode_voltage(void) { TEST_ASSERT_FLOAT_WITHIN(0.01f, 48.0f, raw_to_voltage(480)); }   // 0.1V/bit
void test_decode_current(void) { TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, raw_to_current(32000)); }    // 0.1A/bit, -3200
void test_decode_temp(void)    { TEST_ASSERT_EQUAL_INT(25, raw_to_temp(65)); }                       // 1C/bit, -40
void test_decode_speed(void)   { TEST_ASSERT_EQUAL_INT(0, raw_to_speed(32000)); }                    // 1rpm/bit, -32000

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_torque_offset);
    RUN_TEST(test_cluster_cmd_id);
    RUN_TEST(test_feedback_ids);
    RUN_TEST(test_decode_voltage);
    RUN_TEST(test_decode_current);
    RUN_TEST(test_decode_temp);
    RUN_TEST(test_decode_speed);
    return UNITY_END();
}
