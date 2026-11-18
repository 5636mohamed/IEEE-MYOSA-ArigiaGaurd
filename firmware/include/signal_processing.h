/**
 * @file signal_processing.h
 * @brief Digital filtering, BCG vitals extraction, and PTI ischemia accumulation.
 * @author Mohamed Elhariry
 */

#ifndef SIGNAL_PROCESSING_H
#define SIGNAL_PROCESSING_H

#include "config.h"

struct VitalsEstimate {
    float heartRateBpm;
    float respirationRateBrpm;
    float rmssdMs;
    bool isDeepSleep;
    bool isValid;
};

class SignalProcessor {
public:
    SignalProcessor();
    void reset();
    
    // Ingest new 100 Hz accelerometer Z-sample
    void processSample(float rawAz, bool motionActive);
    
    // Get latest vitals metrics
    VitalsEstimate getVitals() const;
    
    // Accumulate PTI based on chamber pressure (called at 1 Hz)
    float updatePti(float currentPressureMmhg, PostureClass currentPosture, float dtSeconds);
    void resetPti();
    float getCumulativePti() const;

    // Filter accessors
    float getFilteredCardiac() const;
    float getFilteredRespiration() const;

private:
    // Second-order section (biquad) filter implementation
    struct Biquad {
        float b0, b1, b2, a1, a2;
        float x1, x2, y1, y2;
        float process(float in);
        void reset();
    };

    Biquad cardiacBiquad1;
    Biquad cardiacBiquad2;
    Biquad respBiquad1;
    Biquad respBiquad2;

    float lastCardiacSample;
    float lastRespSample;

    // Cardiac peak detection state
    float peakThreshold;
    uint32_t lastPeakTimeMs;
    uint32_t rrIntervals[8];
    uint8_t rrIndex;
    uint8_t rrCount;

    // Respiration zero-crossing state
    float prevRespVal;
    uint32_t lastZeroCrossTimeMs;
    float respIntervals[4];
    uint8_t respIndex;
    uint8_t respCount;

    // Cumulative Pressure-Time Integral (PTI)
    float cumulativePti;

    VitalsEstimate latestVitals;
};

#endif // SIGNAL_PROCESSING_H
