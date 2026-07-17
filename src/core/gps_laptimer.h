#pragma once

namespace gps_laptimer {
void begin();
void poll();
bool start_at_current_fix();
void stop();
}
