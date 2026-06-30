#pragma once
// [FILL-IN] Pure: vehicle speed -> buzzer frequency (Hz). Simple tone, no melody
// (regulation 제12조: >=70dB simple tone). Hardware drive is in the locked core.

struct VessInput { float speed_rpm; bool active; };

int vess_compute(const VessInput &in);   // Hz; 0 = silent
