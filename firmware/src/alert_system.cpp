/**
 * @file alert_system.cpp
 * @brief Implementations of bedside OLED feedback, acoustic buzzer, and TRIAC AC actuation.
 * @authors Mohamed Elhariry, Ahmed Elshentenawy
 */

#include "alert_system.h"
#include "tinyml_inference.h"
#include <Arduino.h>

AlertSystem::AlertSystem()
    : pumpActive(false),
      pumpOffTimestamp(0),
      buzzerState(false),
      buzzerToggleTimestamp(0),
      currentAlertCode(ALERT_NONE),
      currentBankA(true) {}

bool AlertSystem::begin() {
    pinMode(PIN_TRIAC_GATE, OUTPUT);
    pinMode(PIN_ZERO_CROSS, INPUT_PULLUP);
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_VALVE_BANK_A, OUTPUT);
    pinMode(PIN_VALVE_BANK_B, OUTPUT);

    digitalWrite(PIN_TRIAC_GATE, LOW);
    digitalWrite(PIN_BUZZER, LOW);
    
    // Default to Bank A active
    setPneumaticBank(true);
    return true;
}

void AlertSystem::setPneumaticBank(bool bankA) {
    currentBankA = bankA;
    digitalWrite(PIN_VALVE_BANK_A, bankA ? HIGH : LOW);
    digitalWrite(PIN_VALVE_BANK_B, bankA ? LOW : HIGH);
}

void AlertSystem::triggerPumpCycle(uint32_t durationMs) {
    pumpActive = true;
    pumpOffTimestamp = millis() + durationMs;
    digitalWrite(PIN_TRIAC_GATE, HIGH); // Gate pulse to optoisolated TRIAC
}

void AlertSystem::updateActuators(uint32_t currentMillis) {
    // 1. Manage TRIAC compressor duty cycle
    if (pumpActive && (currentMillis >= pumpOffTimestamp)) {
        pumpActive = false;
        digitalWrite(PIN_TRIAC_GATE, LOW);
    }

    // 2. Manage acoustic alarm pulsing
    if (currentAlertCode != ALERT_NONE) {
        if (currentMillis - buzzerToggleTimestamp >= 500) {
            buzzerToggleTimestamp = currentMillis;
            buzzerState = !buzzerState;
            digitalWrite(PIN_BUZZER, buzzerState ? HIGH : LOW);
        }
    } else {
        if (buzzerState) {
            buzzerState = false;
            digitalWrite(PIN_BUZZER, LOW);
        }
    }
}

void AlertSystem::setBuzzerAlert(uint8_t alertCode) {
    currentAlertCode = alertCode;
    if (alertCode == ALERT_NONE) {
        silenceBuzzer();
    }
}

void AlertSystem::silenceBuzzer() {
    currentAlertCode = ALERT_NONE;
    buzzerState = false;
    digitalWrite(PIN_BUZZER, LOW);
}

uint8_t AlertSystem::evaluateAlerts(float pti, uint32_t staticDurationMs, float pressureMmhg,
                                    float humidityRh, PostureClass posture) {
    if (pressureMmhg < BOTTOMING_OUT_THRESHOLD_MMHG) {
        return ALERT_BOTTOMING_OUT;
    }
    if (humidityRh > MACERATION_HUMIDITY_ALERT_RH) {
        return ALERT_MACERATION_RISK;
    }
    if (posture == POSTURE_FOWLER && staticDurationMs > MAX_FOWLER_DURATION_MS) {
        return ALERT_FOWLER_SHEAR_EXTENDED;
    }
    if (pti >= CRITICAL_PTI_THRESHOLD_MMHG_S) {
        return ALERT_ELEVATED_PTI;
    }
    if (staticDurationMs >= MAX_STATIC_TURN_INTERVAL_MS) {
        return ALERT_OVERDUE_TURN;
    }
    return ALERT_NONE;
}

void AlertSystem::updateDisplay(PostureClass posture, float confidence, float hr, float rr,
                               float pressureMmhg, float pti, float humidityRh, float tempC,
                               bool activeBankA, uint8_t activeAlert) {
    // Serial console diagnostic summary representing the bedside OLED rendering
    Serial.println(F("\n================= [ARIGIAGAURD TELEMETRY] ================="));
    Serial.print(F("Posture   : ")); Serial.print(TinyMLInference::getPostureName(posture));
    Serial.print(F(" (")); Serial.print(confidence * 100.0f, 1); Serial.println(F("%)"));
    Serial.print(F("Vitals    : HR = ")); Serial.print(hr, 1); Serial.print(F(" bpm | RR = "));
    Serial.print(rr, 1); Serial.println(F(" brpm"));
    Serial.print(F("Pneumatic : Chamber = ")); Serial.print(pressureMmhg, 1);
    Serial.print(F(" mmHg | Active Bank = ")); Serial.println(activeBankA ? "BANK A" : "BANK B");
    Serial.print(F("Ischemia  : Cumulative PTI = ")); Serial.print(pti, 1); Serial.println(F(" mmHg·s"));
    Serial.print(F("Climate   : ")); Serial.print(humidityRh, 1); Serial.print(F("% RH | "));
    Serial.print(tempC, 1); Serial.println(F(" °C"));
    
    if (activeAlert != ALERT_NONE) {
        Serial.print(F(">>> ALARM TRIGGERED: Code 0x0")); Serial.println(activeAlert, HEX);
    } else {
        Serial.println(F("Status    : ALL SYSTEMS NOMINAL ✓"));
    }
    Serial.println(F("==========================================================\n"));
}
