#ifndef DECUBIGUARD_CONFIG_H
#define DECUBIGUARD_CONFIG_H

#include <Arduino.h>

// I2C Addresses
#define MPU6050_I2C_ADDR       0x68
#define BMP180_I2C_ADDR        0x77
#define SI7021_I2C_ADDR        0x40
#define OLED_I2C_ADDR          0x3C

// Pin Configurations
#define PIN_TRIAC_GATE         4
#define PIN_ZERO_CROSS         2
#define PIN_BUZZER             5
#define PIN_VALVE_CELL_A       18
#define PIN_VALVE_CELL_B       19

// Operational Thresholds
#define TURN_INTERVAL_MS       7200000UL   // 2 hours
#define BOTTOMING_OUT_KPA      1.5f        // Minimum safe pressure
#define OVERPRESSURE_KPA       4.5f        // Maximum cell pressure
#define HUMIDITY_ALERT_RH      75.0f       // Sweating threshold (%)
#define TEMP_ALERT_C           37.8f       // Elevated microclimate temp

// BCG Filtering Constants
#define BCG_SAMPLE_RATE_HZ     100
#define BCG_BUFFER_SIZE        256

#endif // DECUBIGUARD_CONFIG_H
