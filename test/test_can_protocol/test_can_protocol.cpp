#include <unity.h>
#include "can_protocol.h"
#include "cluster_command.h"

// shared torque scaling (from VCU base)
void test_torque_offset(void) { TEST_ASSERT_EQUAL_UINT16(32000, torque_to_raw(0.0f)); }
// Cluster additions
void test_cluster_cmd_id(void) { TEST_ASSERT_EQUAL_HEX32(0x1801D0C0, CAN_ID_CLUSTER_CMD); }
void test_vcu_cluster_status_id(void) { TEST_ASSERT_EQUAL_HEX32(0x1801C0D0, CAN_ID_VCU_CLUSTER_STATUS); }
void test_feedback_ids(void) {
    TEST_ASSERT_EQUAL_HEX32(0x1801D0EF, CAN_ID_FB1_L);
    TEST_ASSERT_EQUAL_HEX32(0x1802D0EF, CAN_ID_FB2_L);
    TEST_ASSERT_EQUAL_HEX32(0x1801D0F0, CAN_ID_FB1_R);
    TEST_ASSERT_EQUAL_HEX32(0x1802D0F0, CAN_ID_FB2_R);
}
// guards against a copy-paste mistake reusing the same ID for L and R
void test_feedback_ids_lr_distinct(void) {
    TEST_ASSERT_NOT_EQUAL(CAN_ID_FB1_L, CAN_ID_FB1_R);
    TEST_ASSERT_NOT_EQUAL(CAN_ID_FB2_L, CAN_ID_FB2_R);
    TEST_ASSERT_NOT_EQUAL(CAN_ID_FB1_L, CAN_ID_FB2_L);
    TEST_ASSERT_NOT_EQUAL(CAN_ID_FB1_R, CAN_ID_FB2_R);
}
void test_decode_voltage(void) { TEST_ASSERT_FLOAT_WITHIN(0.01f, 48.0f, raw_to_voltage(480)); }   // 0.1V/bit
void test_decode_current(void) { TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, raw_to_current(32000)); }    // 0.1A/bit, -3200
void test_decode_temp(void)    { TEST_ASSERT_EQUAL_INT(25, raw_to_temp(65)); }                       // 1C/bit, -40
void test_decode_speed(void)   { TEST_ASSERT_EQUAL_INT(0, raw_to_speed(32000)); }                    // 1rpm/bit, -32000

void test_encode_gear_and_mode(void) {
    uint8_t out[8];
    encode_cluster_command({Gear::D, 2, false}, out);
    TEST_ASSERT_EQUAL_UINT8(2, out[0]);   // gear D = 2
    TEST_ASSERT_EQUAL_UINT8(2, out[1]);   // drive_mode
    TEST_ASSERT_EQUAL_UINT8(0, out[2] & 0x01);   // paddock off
    TEST_ASSERT_EQUAL_UINT8(0x02, out[2] & 0x02); // VESS defaults on
}

void test_encode_paddock_bit(void) {
    uint8_t out[8];
    encode_cluster_command({Gear::N, 0, true}, out);
    TEST_ASSERT_EQUAL_UINT8(1, out[2] & 0x01);   // paddock on
}
void test_encode_vess_bit(void) {
    uint8_t out[8];
    encode_cluster_command({Gear::N, 0, false, true}, out);
    TEST_ASSERT_EQUAL_UINT8(0x02, out[2] & 0x02); // VESS enabled
    encode_cluster_command({Gear::N, 0, false, false}, out);
    TEST_ASSERT_EQUAL_UINT8(0, out[2] & 0x02);    // VESS off request
}

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_torque_offset);
    RUN_TEST(test_cluster_cmd_id);
    RUN_TEST(test_vcu_cluster_status_id);
    RUN_TEST(test_feedback_ids);
    RUN_TEST(test_feedback_ids_lr_distinct);
    RUN_TEST(test_decode_voltage);
    RUN_TEST(test_decode_current);
    RUN_TEST(test_decode_temp);
    RUN_TEST(test_decode_speed);
    RUN_TEST(test_encode_gear_and_mode);
    RUN_TEST(test_encode_paddock_bit);
    RUN_TEST(test_encode_vess_bit);
    return UNITY_END();
}
