#pragma once
#include <cstdint>
// [FILL-IN] Pure: decide WHAT to show. The locked U8g2 renderer draws this model.

struct DisplayInput { float speed_rpm; float soc; int controller_temp; int motor_temp; uint8_t error1; bool hv_active; };
struct DisplayModel { int speed; int soc_pct; int temp_max; bool fault; bool hv; };

DisplayModel display_compute(const DisplayInput &in);
