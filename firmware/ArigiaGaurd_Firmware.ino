#include <Wire.h>
#include "include/config.h"

// State variables
unsigned long lastTurnTimestamp = 0;
float currentCellPressure = 0.0f;
float currentHumidity = 0.0f;
float currentTemp = 0.0f;
float estimatedHeartRate = 72.0f;
float estimatedRespRate = 16.0f;
bool activeCellBank = false; // false = Bank A, true = Bank B

void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(PIN_TRIAC_GATE, OUTPUT);
    pinMode(PIN_ZERO_CROSS, INPUT_PULLUP);
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_VALVE_CELL_A, OUTPUT);
    pinMode(PIN_VALVE_CELL_B, OUTPUT);

    digitalWrite(PIN_TRIAC_GATE, LOW);
    digitalWrite(PIN_BUZZER, LOW);
    
    lastTurnTimestamp = millis();
    Serial.println(F("[ArigiaGaurd] System Initialized Successfully."));
}

void loop() {
    unsigned long now = millis();

    // 1. Check patient turn interval (mandated 2-hour window)
    if (now - lastTurnTimestamp >= TURN_INTERVAL_MS) {
        digitalWrite(PIN_BUZZER, HIGH);
        Serial.println(F("[ALERT] Turn interval expired: Reposition patient."));
    }

    // 2. Closed-loop pressure regulation & bank alternation
    static unsigned long lastCycleSwitch = 0;
    if (now - lastCycleSwitch >= 600000UL) { // 10 minute alternation cycle
        activeCellBank = !activeCellBank;
        digitalWrite(PIN_VALVE_CELL_A, activeCellBank ? HIGH : LOW);
        digitalWrite(PIN_VALVE_CELL_B, activeCellBank ? LOW : HIGH);
        lastCycleSwitch = now;
        Serial.print(F("[INFO] Switched air cell bank to: "));
        Serial.println(activeCellBank ? "Bank B" : "Bank A");
    }

    delay(20);
}
