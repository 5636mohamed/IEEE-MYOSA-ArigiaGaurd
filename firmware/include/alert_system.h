/**
 * @file alert_system.h
 * @brief Bedside OLED display, acoustic buzzer, and optoisolated TRIAC compressor actuation.
 * @authors Mohamed Elhariry, Ahmed Elshentenawy
 */

#ifndef ALERT_SYSTEM_H
#define ALERT_SYSTEM_H

#include "config.h"

enum AlertCode {
    ALERT_NONE                  = 0x00,
    ALERT_OVERDUE_TURN          = 0x01,
    ALERT_ELEVATED_PTI          = 0x02,
    ALERT_BOTTOMING_OUT         = 0x03,
    ALERT_MACERATION_RISK       = 0x04,
    ALERT_FOWLER_SHEAR_EXTENDED = 0x05
};

class AlertSystem {
public:
    AlertSystem();
    bool begin();
    
    // Updates OLED screen with current system vitals and status
    void updateDisplay(PostureClass posture, float confidence, float hr, float rr,
                       float pressureMmhg, float pti, float humidityRh, float tempC,
                       bool activeBankA, uint8_t activeAlert);

    // Actuation control
    void setPneumaticBank(bool bankA);
    void triggerPumpCycle(uint32_t durationMs);
    void updateActuators(uint32_t currentMillis);

    // Acoustic alarm control
    void setBuzzerAlert(uint8_t alertCode);
    void silenceBuzzer();

    // Decision arbitration
    uint8_t evaluateAlerts(float pti, uint32_t staticDurationMs, float pressureMmhg,
                           float humidityRh, PostureClass posture);

private:
    bool pumpActive;
    uint32_t pumpOffTimestamp;
    bool buzzerState;
    uint32_t buzzerToggleTimestamp;
    uint8_t currentAlertCode;
    bool currentBankA;
};

#endif // ALERT_SYSTEM_H
