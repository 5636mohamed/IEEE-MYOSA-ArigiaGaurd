/**
 * @file config.h
 * @brief Global configuration constants, pinouts, and clinical thresholds for DecubiGuard.
 * @author Mohamed Elhariry
 * @institution Egypt-Japan University of Science and Technology (E-JUST)
 */

#ifndef DECUBIGUARD_CONFIG_H
#define DECUBIGUARD_CONFIG_H

#include <stdint.h>

// ==========================================
// System Hardware & Bus Configuration
// ==========================================
#define SERIAL_BAUD_RATE            115200
#define I2C_SDA_PIN                 21
#define I2C_SCL_PIN                 22
#define I2C_CLOCK_SPEED_HZ          400000

// I2C 7-bit Device Addresses
#define MPU6050_I2C_ADDR            0x68
#define BMP180_I2C_ADDR             0x77
#define SI7021_I2C_ADDR             0x40
#define OLED_SSD1306_I2C_ADDR       0x3C

// Actuation & Peripheral GPIO Pins
#define PIN_TRIAC_GATE              4       // MOC3041 Optoisolator Trigger
#define PIN_ZERO_CROSS              2       // AC Zero-Crossing Detection Interrupt
#define PIN_BUZZER                  5       // Active Piezo Alarm Buzzer
#define PIN_VALVE_BANK_A            18      // Solenoid Valve Air-Cell Bank A
#define PIN_VALVE_BANK_B            19      // Solenoid Valve Air-Cell Bank B

// ==========================================
// Sampling Rates & Windowing
// ==========================================
#define IMU_SAMPLE_RATE_HZ          100     // Kinematics and BCG micro-vibrations
#define IMU_SAMPLE_PERIOD_MS        10      // 10 ms interval
#define IMU_WINDOW_SIZE             200     // 2000 ms sliding window (200 samples)
#define IMU_STEP_SIZE               100     // 1000 ms stride (50% overlap)

#define BCG_FILTER_ORDER            4       // 4th-order digital Butterworth
#define BCG_CARDIAC_HP_HZ           0.8f    // Cardiac bandpass lower cutoff
#define BCG_CARDIAC_LP_HZ           3.5f    // Cardiac bandpass upper cutoff
#define BCG_RESP_HP_HZ              0.1f    // Respiration bandpass lower cutoff
#define BCG_RESP_LP_HZ              0.5f    // Respiration bandpass upper cutoff

// ==========================================
// Clinical Ischemia & Operational Thresholds
// ==========================================
#define CAPILLARY_CLOSING_PRESSURE_MMHG   32.0f   // Landmark microvascular collapse point
#define CRITICAL_PTI_THRESHOLD_MMHG_S     1800.0f // Ischemic pressure-time integral limit
#define MAX_STATIC_TURN_INTERVAL_MS       7200000UL // 2 hours mandated clinical repositioning
#define MAX_FOWLER_DURATION_MS            3600000UL // 1 hour shear stress limit

#define BOTTOMING_OUT_THRESHOLD_MMHG      15.0f   // Air-cell deflation hazard limit
#define NOMINAL_CELL_PRESSURE_MMHG        32.0f   // Target operating chamber pressure
#define MAX_OVERPRESSURE_MMHG             55.0f   // Over-inflation safety limit

#define MACERATION_HUMIDITY_ALERT_RH      65.0f   // Stratum corneum overhydration limit
#define TISSUE_TEMPERATURE_ALERT_C        35.5f   // Elevated metabolic demand threshold

#define DEEP_SLEEP_RMSSD_THRESHOLD_MS     45.0f   // Elevated HRV indicating non-REM sleep
#define MOTION_ARTIFACT_THRESHOLD_G2      0.05f   // IMU variance threshold for motion gating

// ==========================================
// Posture Class Enumeration
// ==========================================
enum PostureClass {
    POSTURE_SUPINE           = 0,
    POSTURE_RIGHT_LATERAL_30 = 1,
    POSTURE_LEFT_LATERAL_30  = 2,
    POSTURE_FOWLER           = 3,
    POSTURE_PRONE            = 4,
    POSTURE_UNKNOWN          = 255
};

// ==========================================
// BLE Telemetry UUID Definitions
// ==========================================
#define BLE_DEVICE_NAME             "DecubiGuard"
#define BLE_SERVICE_UUID            "0000FE40-CC7A-482A-984A-7F2ED5B3E58F"
#define BLE_CHAR_VITALS_UUID        "0000FE41-CC7A-482A-984A-7F2ED5B3E58F"
#define BLE_CHAR_PRESSURE_UUID      "0000FE42-CC7A-482A-984A-7F2ED5B3E58F"
#define BLE_CHAR_ALERTS_UUID        "0000FE43-CC7A-482A-984A-7F2ED5B3E58F"
#define BLE_CHAR_CONTROL_UUID       "0000FE44-CC7A-482A-984A-7F2ED5B3E58F"

#endif // DECUBIGUARD_CONFIG_H
