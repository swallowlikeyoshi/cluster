#include <unity.h>
#include "state.h"

void test_state_defaults_safe(void) {
    ClusterState s;
    TEST_ASSERT_EQUAL_FLOAT(0.0f, s.speed_rpm);
    TEST_ASSERT_FALSE(s.hv_active);
    TEST_ASSERT_FALSE(s.handshaked);
    TEST_ASSERT_EQUAL_UINT8(0, s.error1);
}

void setUp(void) {}
void tearDown(void) {}
int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_state_defaults_safe);
    return UNITY_END();
}
