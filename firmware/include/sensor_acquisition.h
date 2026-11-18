/**
 * @file sensor_acquisition.h
 * @brief Multi-modal sensor acquisition subsystem for ArigiaGaurd (MPU6050, BMP180, SI7021).
 * @author Mohamed Elhariry
 */

#ifndef SENSOR_ACQUISITION_H
#define SENSOR_ACQUISITION_H

#include "config.h"

struct ImuSample {
    float ax; // g
    float ay; // g
    float az; // g
    float gx; // deg/s
    float gy; // deg/s
    float gz; // deg/s
    uint32_t timestamp_ms;
};

struct PressureSample {
    float pressure_mmhg;
    float temperature_c;
    uint32_t timestamp_ms;
};

struct MicroclimateSample {
    float humidity_rh;
    float temperature_c;
    uint32_t timestamp_ms;
};

class SensorAcquisition {
public:
    SensorAcquisition();
    bool begin();
    bool readImu(ImuSample& sample);
    bool readPressure(PressureSample& sample);
    bool readMicroclimate(MicroclimateSample& sample);
    bool isMotionArtifactActive() const;
    void updateMotionState(const ImuSample& sample);

private:
    bool mpuInitialized;
    bool bmpInitialized;
    bool si7021Initialized;
    bool motionArtifact;
    float prevAx, prevAy, prevAz;
};

#endif // SENSOR_ACQUISITION_H
