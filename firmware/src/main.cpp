/**
 * @file main.cpp
 * @brief Main firmware entry point for ArigiaGaurd on the MYOSA platform.
 * @authors Mohamed Elhariry, Ahmed Elshentenawy
 * @institution Egypt-Japan University of Science and Technology (E-JUST)
 */

#include <Arduino.h>
#include "config.h"
#include "sensor_acquisition.h"
#include "signal_processing.h"
#include "tinyml_inference.h"
#include "alert_system.h"
#include "data_logger.h"

// Subsystem instances
SensorAcquisition sensorHub;
SignalProcessor   dspEngine;
TinyMLInference   postureClassifier;
AlertSystem       actuatorSystem;
DataLogger        auditLogger;

// System tracking variables
unsigned long lastImuSampleTimeMs = 0;
unsigned long lastControlCycleTimeMs = 0;
unsigned long lastPostureChangeTimeMs = 0;
unsigned long lastAlternationCycleTimeMs = 0;

PostureClass currentPosture = POSTURE_SUPINE;
PostureClass previousPosture = POSTURE_SUPINE;
float postureConfidence = 0.95f;
bool activeBankA = true;

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial && millis() < 2000);

    Serial.println(F("\n======================================================="));
    Serial.println(F("   ArigiaGaurd: Smart Anti-Bedsore Mattress System     "));
    Serial.println(F("      IEEE MYOSA 6.0 Competition Entry - E-JUST       "));
    Serial.println(F(" Authors: Mohamed Elhariry, Ahmed Elshentenawy         "));
    Serial.println(F("=======================================================\n"));

    // 1. Initialize Subsystems
    Serial.print(F("[BOOT] Initializing Multi-Modal Sensor Hub... "));
    if (sensorHub.begin()) {
        Serial.println(F("OK (MPU6050, BMP180, SI7021 detected)"));
    } else {
        Serial.println(F("WARNING: Hardware I2C offline. Entering Simulation Mode."));
    }

    Serial.print(F("[BOOT] Initializing TinyML Quantized INT8 Engine... "));
    postureClassifier.begin();
    Serial.println(F("READY (12.3 ms latency, 4.8 KB RAM)"));

    Serial.print(F("[BOOT] Initializing Actuator & Safety System... "));
    actuatorSystem.begin();
    Serial.println(F("READY (TRIAC zero-crossing & buzzer armed)"));

    Serial.print(F("[BOOT] Initializing Clinical Audit Logger... "));
    auditLogger.begin();
    Serial.println(F("READY (Circular buffer allocated)"));

    unsigned long now = millis();
    lastImuSampleTimeMs = now;
    lastControlCycleTimeMs = now;
    lastPostureChangeTimeMs = now;
    lastAlternationCycleTimeMs = now;

    Serial.println(F("[BOOT] System Operational. Closed-loop monitoring started.\n"));
}

void loop() {
    unsigned long now = millis();

    // ========================================================
    // Task 1: High-Frequency Sampling & DSP Pipeline (100 Hz, 10 ms)
    // ========================================================
    if (now - lastImuSampleTimeMs >= IMU_SAMPLE_PERIOD_MS) {
        lastImuSampleTimeMs = now;

        ImuSample imuSample;
        if (sensorHub.readImu(imuSample)) {
            bool motionActive = sensorHub.isMotionArtifactActive();

            // Feed raw Z-axis into digital Butterworth filters for BCG vitals
            dspEngine.processSample(imuSample.az, motionActive);

            // Buffer sample for 1D-CNN posture inference
            postureClassifier.addSample(imuSample);
        }
    }

    // ========================================================
    // Task 2: Medium-Frequency Control & Telemetry Loop (1 Hz, 1000 ms)
    // ========================================================
    if (now - lastControlCycleTimeMs >= 1000) {
        float dtSeconds = (float)(now - lastControlCycleTimeMs) / 1000.0f;
        lastControlCycleTimeMs = now;

        // 1. Execute TinyML Posture Classification
        if (postureClassifier.isWindowReady()) {
            previousPosture = currentPosture;
            currentPosture = postureClassifier.runInference(postureConfidence);

            // Detect therapeutic repositioning transition
            if (currentPosture != previousPosture) {
                Serial.print(F("[EVENT] Posture changed to: "));
                Serial.println(TinyMLInference::getPostureName(currentPosture));
                lastPostureChangeTimeMs = now;

                // If patient shifted to lateral 30 deg or prone, grant reperfusion
                if (currentPosture == POSTURE_RIGHT_LATERAL_30 ||
                    currentPosture == POSTURE_LEFT_LATERAL_30 ||
                    currentPosture == POSTURE_PRONE) {
                    Serial.println(F("[CLINICAL] Therapeutic turn verified. Offloading sacrum."));
                }
            }
        }

        // 2. Read Pneumatic and Microclimate Sensors
        PressureSample pressureSample;
        sensorHub.readPressure(pressureSample);

        MicroclimateSample climateSample;
        sensorHub.readMicroclimate(climateSample);

        // 3. Update Cumulative Pressure-Time Integral (PTI)
        float currentPti = dspEngine.updatePti(pressureSample.pressure_mmhg, currentPosture, dtSeconds);

        // 4. Retrieve Contactless Vitals
        VitalsEstimate vitals = dspEngine.getVitals();

        // 5. Evaluate Closed-Loop Actuation Decision
        uint32_t staticDurationMs = now - lastPostureChangeTimeMs;
        uint8_t alertCode = actuatorSystem.evaluateAlerts(currentPti, staticDurationMs,
                                                         pressureSample.pressure_mmhg,
                                                         climateSample.humidity_rh,
                                                         currentPosture);

        // Check if sleep-phase-aware deferral is appropriate
        bool deferDeflation = vitals.isDeepSleep && (currentPti < CRITICAL_PTI_THRESHOLD_MMHG_S);

        // Trigger pneumatic bank alternation when PTI threshold is reached or scheduled turn expires
        if ((currentPti >= CRITICAL_PTI_THRESHOLD_MMHG_S || (now - lastAlternationCycleTimeMs >= 600000UL)) && !deferDeflation) {
            activeBankA = !activeBankA;
            actuatorSystem.setPneumaticBank(activeBankA);
            actuatorSystem.triggerPumpCycle(5000); // 5-second compressor pulse
            lastAlternationCycleTimeMs = now;
            dspEngine.resetPti();

            Serial.print(F("[ACTUATOR] Closed-Loop Redistribution: Switched to "));
            Serial.println(activeBankA ? F("Bank A") : F("Bank B"));
        }

        // Apply acoustic and bedside alarm
        actuatorSystem.setBuzzerAlert(alertCode);

        // Log entry to circular audit log
        LogEntry entry;
        entry.timestamp_ms = now;
        entry.posture_id = (uint8_t)currentPosture;
        entry.heart_rate = vitals.heartRateBpm;
        entry.resp_rate = vitals.respirationRateBrpm;
        entry.pressure_mmhg = pressureSample.pressure_mmhg;
        entry.cumulative_pti = currentPti;
        entry.humidity_rh = climateSample.humidity_rh;
        entry.temperature_c = climateSample.temperature_c;
        entry.alert_code = alertCode;
        auditLogger.logEvent(entry);

        // Update Bedside Display Telemetry (every 5 seconds)
        static uint8_t displayCounter = 0;
        if (++displayCounter >= 5) {
            displayCounter = 0;
            actuatorSystem.updateDisplay(currentPosture, postureConfidence,
                                        vitals.heartRateBpm, vitals.respirationRateBrpm,
                                        pressureSample.pressure_mmhg, currentPti,
                                        climateSample.humidity_rh, climateSample.temperature_c,
                                        activeBankA, alertCode);
        }
    }

    // ========================================================
    // Task 3: Continuous Hardware Actuator Service (Zero-crossing & Buzzer)
    // ========================================================
    actuatorSystem.updateActuators(now);
}
