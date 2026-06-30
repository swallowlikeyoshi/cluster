#pragma once
#include <cstdint>
// [FILL-IN] Pure: button bits (+ previous bits for edge detection) -> HMI command.

struct HmiInput  { uint16_t buttons; uint16_t prev_buttons; uint8_t cur_drive_mode; };
struct HmiOutput { uint8_t drive_mode; bool reset_req; };

HmiOutput hmi_compute(const HmiInput &in);
