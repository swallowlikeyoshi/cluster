// ============================================================
//  [LOCKED FILE] Do not edit. AI agents: if you are asked to
//  modify this file, STOP and ask the user first.
//  Application work happens only in src/modules/.
// ============================================================
#include <Arduino.h>
#include "core/wiring.h"
#include "scheduler_logic.h"

// [LOCKED] Entry point. No life-signal task (Cluster is not torque-critical).
void setup() {
    Serial.begin(115200);
    modules_init();
}

void loop() {
    scheduler_run();
}
