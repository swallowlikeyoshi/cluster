#pragma once
#include <cstdint>
// [FILL-IN] Pure: vehicle fault/flags -> 16-bit lamp bitmask for the MCP23017.

struct IndicatorInput { uint8_t error1; bool hv_active; bool brake; };

uint16_t indicators_compute(const IndicatorInput &in);
