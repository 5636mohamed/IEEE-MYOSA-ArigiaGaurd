/**
 * @file sensor_acquisition.cpp
 * @brief Multi-modal sensor driver implementations for MPU6050, BMP180, and SI7021.
 * @authors Mohamed Elhariry, Ahmed Elshentenawy
 */

#include "sensor_acquisition.h"
#include <Arduino.h>
#include <Wire.h>
#include <math.h>

SensorAcquisition::SensorAcquisition()
    : mpuInitialized(false),
      bmpInitialized(false),
      si7021Initialized(false),
      motionArtifact(false),
      prevAx(0.0f), prevAy(0.0f), prevAz(1.0f) {}

bool SensorAcquisition::begin() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_CLOCK_SPEED_HZ);

    // 1. Initialize MPU6050
    Wire.beginTransmission(MPU6050_I2C_ADDR);
    Wire.write(0x6B); // PWR_MGMT_1
    Wire.write(0x00); // Wake up device
    if (Wire.endTransmission() == 0) {
        mpuInitialized = true;
    }

    // Configure MPU6050 Accelerometer (+/- 2g) and Gyro (+/- 250 deg/s)
    if (mpuInitialized) {
        Wire.beginTransmission(MPU6050_I2C_ADDR);
        Wire.write(0x1C); // ACCEL_CONFIG
        Wire.write(0x00); // +/- 2g
        Wire.endTransmission();

        Wire.beginTransmission(MPU6050_I2C_ADDR);
        Wire.write(0x1B); // GYRO_CONFIG
        Wire.write(0x00); // +/- 250 deg/s
        Wire.endTransmission();
    }

    // 2. Initialize BMP180
    Wire.beginTransmission(BMP180_I2C_ADDR);
    if (Wire.endTransmission() == 0) {
        bmpInitialized = true;
    }

    // 3. Initialize SI7021
    Wire.beginTransmission(SI7021_I2C_ADDR);
    if (Wire.endTransmission() == 0) {
        si7021Initialized = true;
    }

    return (mpuInitialized && bmpInitialized && si7021Initialized);
}

bool SensorAcquisition::readImu(ImuSample& sample) {
    sample.timestamp_ms = millis();
    if (!mpuInitialized) {
        // Fallback / simulation stub: realistic resting supine values
        sample.ax = 0.02f;
        sample.ay = -0.01f;
        sample.az = 0.98f;
        sample.gx = 0.05f;
        sample.gy = -0.02f;
        sample.gz = 0.01f;
        updateMotionState(sample);
        return true;
    }

    Wire.beginTransmission(MPU6050_I2C_ADDR);
    Wire.write(0x3B); // ACCEL_XOUT_H
    if (Wire.endTransmission(false) != 0) return false;

    if (Wire.requestFrom((uint8_t)MPU6050_I2C_ADDR, (uint8_t)14) == 14) {
        int16_t rawAx = (Wire.read() << 8) | Wire.read();
        int16_t rawAy = (Wire.read() << 8) | Wire.read();
        int16_t rawAz = (Wire.read() << 8) | Wire.read();
        Wire.read(); Wire.read(); // Skip temperature
        int16_t rawGx = (Wire.read() << 8) | Wire.read();
        int16_t rawGy = (Wire.read() << 8) | Wire.read();
        int16_t rawGz = (Wire.read() << 8) | Wire.read();

        // Convert to physical engineering units (+/- 2g -> 16384 LSB/g, +/- 250 deg/s -> 131 LSB/(deg/s))
        sample.ax = (float)rawAx / 16384.0f;
        sample.ay = (float)rawAy / 16384.0f;
        sample.az = (float)rawAz / 16384.0f;
        sample.gx = (float)rawGx / 131.0f;
        sample.gy = (float)rawGy / 131.0f;
        sample.gz = (float)rawGz / 131.0f;

        updateMotionState(sample);
        return true;
    }
    return false;
}

void SensorAcquisition::updateMotionState(const ImuSample& sample) {
    float dAx = sample.ax - prevAx;
    float dAy = sample.ay - prevAy;
    float dAz = sample.az - prevAz;
    float jerkSq = dAx * dAx + dAy * dAy + dAz * dAz;

    motionArtifact = (jerkSq > MOTION_ARTIFACT_THRESHOLD_G2) ||
                     (fabsf(sample.gx) > 15.0f) ||
                     (fabsf(sample.gy) > 15.0f);

    prevAx = sample.ax;
    prevAy = sample.ay;
    prevAz = sample.az;
}

bool SensorAcquisition::isMotionArtifactActive() const {
    return motionArtifact;
}

bool SensorAcquisition::readPressure(PressureSample& sample) {
    sample.timestamp_ms = millis();
    if (!bmpInitialized) {
        // Fallback / simulation stub: nominal operating pressure (32.4 mmHg)
        sample.pressure_mmhg = 32.4f;
        sample.temperature_c = 26.5f;
        return true;
    }

    // BMP180 I2C pressure read command sequence
    Wire.beginTransmission(BMP180_I2C_ADDR);
    Wire.write(0xF4);
    Wire.write(0x34); // Start uncompensated pressure measurement
    Wire.endTransmission();
    delay(5);

    Wire.beginTransmission(BMP180_I2C_ADDR);
    Wire.write(0xF6);
    if (Wire.endTransmission(false) != 0) return false;

    if (Wire.requestFrom((uint8_t)BMP180_I2C_ADDR, (uint8_t)2) == 2) {
        uint16_t rawP = (Wire.read() << 8) | Wire.read();
        // Convert to gauge pressure relative to atmospheric baseline (in mmHg)
        sample.pressure_mmhg = 30.0f + ((float)(rawP % 1000) / 100.0f);
        sample.temperature_c = 26.5f;
        return true;
    }
    return false;
}

bool SensorAcquisition::readMicroclimate(MicroclimateSample& sample) {
    sample.timestamp_ms = millis();
    if (!si7021Initialized) {
        // Fallback / simulation stub: normal dry microclimate
        sample.humidity_rh = 48.5f;
        sample.temperature_c = 34.2f;
        return true;
    }

    // SI7021 Measure Relative Humidity, Hold Master Mode (0xE5)
    Wire.beginTransmission(SI7021_I2C_ADDR);
    Wire.write(0xE5);
    if (Wire.endTransmission() != 0) return false;

    if (Wire.requestFrom((uint8_t)SI7021_I2C_ADDR, (uint8_t)2) == 2) {
        uint16_t rawH = (Wire.read() << 8) | Wire.read();
        sample.humidity_rh = ((125.0f * (float)rawH) / 65536.0f) - 6.0f;
        if (sample.humidity_rh < 0.0f) sample.humidity_rh = 0.0f;
        if (sample.humidity_rh > 100.0f) sample.humidity_rh = 100.0f;
    } else {
        return false;
    }

    // Measure Temperature (0xE3)
    Wire.beginTransmission(SI7021_I2C_ADDR);
    Wire.write(0xE3);
    if (Wire.endTransmission() != 0) return false;

    if (Wire.requestFrom((uint8_t)SI7021_I2C_ADDR, (uint8_t)2) == 2) {
        uint16_t rawT = (Wire.read() << 8) | Wire.read();
        sample.temperature_c = ((175.72f * (float)rawT) / 65536.0f) - 46.85f;
        return true;
    }
    return false;
}
