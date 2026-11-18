/**
 * @file signal_processing.cpp
 * @brief Implementations of digital Butterworth filtering, J-wave peak detection, and PTI integration.
 * @authors Mohamed Elhariry, Ahmed Elshentenawy
 */

#include "signal_processing.h"
#include <Arduino.h>
#include <math.h>

float SignalProcessor::Biquad::process(float in) {
    float out = b0 * in + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
    x2 = x1;
    x1 = in;
    y2 = y1;
    y1 = out;
    return out;
}

void SignalProcessor::Biquad::reset() {
    x1 = x2 = y1 = y2 = 0.0f;
}

SignalProcessor::SignalProcessor() {
    // 4th-order Butterworth bandpass (0.8 - 3.5 Hz at fs = 100 Hz), partitioned into 2 biquads
    // Biquad 1:
    cardiacBiquad1.b0 = 0.0829f; cardiacBiquad1.b1 = 0.0f;     cardiacBiquad1.b2 = -0.0829f;
    cardiacBiquad1.a1 = -1.8210f; cardiacBiquad1.a2 = 0.8341f;
    // Biquad 2:
    cardiacBiquad2.b0 = 0.0829f; cardiacBiquad2.b1 = 0.0f;     cardiacBiquad2.b2 = -0.0829f;
    cardiacBiquad2.a1 = -1.8210f; cardiacBiquad2.a2 = 0.8341f;

    // Respiration bandpass (0.1 - 0.5 Hz at fs = 100 Hz), 2 biquads
    // Biquad 1:
    respBiquad1.b0 = 0.0125f; respBiquad1.b1 = 0.0f;     respBiquad1.b2 = -0.0125f;
    respBiquad1.a1 = -1.9740f; respBiquad1.a2 = 0.9750f;
    // Biquad 2:
    respBiquad2.b0 = 0.0125f; respBiquad2.b1 = 0.0f;     respBiquad2.b2 = -0.0125f;
    respBiquad2.a1 = -1.9740f; respBiquad2.a2 = 0.9750f;

    reset();
}

void SignalProcessor::reset() {
    cardiacBiquad1.reset();
    cardiacBiquad2.reset();
    respBiquad1.reset();
    respBiquad2.reset();

    lastCardiacSample = 0.0f;
    lastRespSample = 0.0f;
    peakThreshold = 0.015f; // Initial threshold in g
    lastPeakTimeMs = 0;
    rrIndex = 0;
    rrCount = 0;

    prevRespVal = 0.0f;
    lastZeroCrossTimeMs = 0;
    respIndex = 0;
    respCount = 0;

    cumulativePti = 0.0f;

    latestVitals.heartRateBpm = 72.0f;
    latestVitals.respirationRateBrpm = 16.0f;
    latestVitals.rmssdMs = 38.0f;
    latestVitals.isDeepSleep = false;
    latestVitals.isValid = false;
}

void SignalProcessor::processSample(float rawAz, bool motionActive) {
    if (motionActive) {
        // Discard BCG vitals during transient motion artifact to avoid false peaks
        latestVitals.isValid = false;
        return;
    }

    uint32_t now = millis();

    // 1. Digital Filtering
    float cStage1 = cardiacBiquad1.process(rawAz);
    lastCardiacSample = cardiacBiquad2.process(cStage1);

    float rStage1 = respBiquad1.process(rawAz);
    lastRespSample = respBiquad2.process(rStage1);

    // 2. Cardiac J-wave Peak Detection
    if (lastCardiacSample > peakThreshold && (now - lastPeakTimeMs > 400)) { // Max 150 bpm limit
        if (lastPeakTimeMs > 0) {
            uint32_t interval = now - lastPeakTimeMs;
            if (interval >= 400 && interval <= 1500) { // 40 - 150 bpm
                rrIntervals[rrIndex] = interval;
                rrIndex = (rrIndex + 1) % 8;
                if (rrCount < 8) rrCount++;

                // Calculate mean heart rate
                uint32_t sumInterval = 0;
                for (uint8_t i = 0; i < rrCount; i++) sumInterval += rrIntervals[i];
                float meanInterval = (float)sumInterval / (float)rrCount;
                latestVitals.heartRateBpm = 60000.0f / meanInterval;

                // Calculate RMSSD for HRV
                if (rrCount >= 3) {
                    float sumDiffSq = 0.0f;
                    for (uint8_t i = 1; i < rrCount; i++) {
                        float diff = (float)rrIntervals[i] - (float)rrIntervals[i - 1];
                        sumDiffSq += diff * diff;
                    }
                    latestVitals.rmssdMs = sqrtf(sumDiffSq / (float)(rrCount - 1));
                    latestVitals.isDeepSleep = (latestVitals.rmssdMs > DEEP_SLEEP_RMSSD_THRESHOLD_MS);
                }
                latestVitals.isValid = true;
            }
        }
        lastPeakTimeMs = now;
        peakThreshold = 0.6f * peakThreshold + 0.4f * lastCardiacSample;
        if (peakThreshold < 0.008f) peakThreshold = 0.008f;
    } else {
        peakThreshold *= 0.999f; // Exponential decay
        if (peakThreshold < 0.008f) peakThreshold = 0.008f;
    }

    // 3. Respiration Rate (Zero-Crossing Detection)
    if (prevRespVal < 0.0f && lastRespSample >= 0.0f) { // Positive zero crossing
        if (lastZeroCrossTimeMs > 0) {
            uint32_t rPeriod = now - lastZeroCrossTimeMs;
            if (rPeriod >= 1500 && rPeriod <= 10000) { // 6 - 40 breaths/min
                respIntervals[respIndex] = (float)rPeriod;
                respIndex = (respIndex + 1) % 4;
                if (respCount < 4) respCount++;

                float sumPeriod = 0.0f;
                for (uint8_t i = 0; i < respCount; i++) sumPeriod += respIntervals[i];
                float meanPeriod = sumPeriod / (float)respCount;
                latestVitals.respirationRateBrpm = 60000.0f / meanPeriod;
            }
        }
        lastZeroCrossTimeMs = now;
    }
    prevRespVal = lastRespSample;
}

VitalsEstimate SignalProcessor::getVitals() const {
    return latestVitals;
}

float SignalProcessor::updatePti(float currentPressureMmhg, PostureClass currentPosture, float dtSeconds) {
    // If patient is in an offloaded therapeutic posture (30 deg lateral tilt or prone), reperfusion occurs
    if (currentPosture == POSTURE_RIGHT_LATERAL_30 || currentPosture == POSTURE_LEFT_LATERAL_30 || currentPosture == POSTURE_PRONE) {
        cumulativePti -= (10.0f * dtSeconds); // Reperfusion credit
        if (cumulativePti < 0.0f) cumulativePti = 0.0f;
    } else {
        // Supine or Fowler posture: calculate ischemic pressure excess over capillary threshold
        float excessPressure = currentPressureMmhg - CAPILLARY_CLOSING_PRESSURE_MMHG;
        if (excessPressure > 0.0f) {
            float shearMultiplier = (currentPosture == POSTURE_FOWLER) ? 1.5f : 1.0f; // Shear aggravation in Fowler
            cumulativePti += (excessPressure * shearMultiplier * dtSeconds);
        }
    }
    return cumulativePti;
}

void SignalProcessor::resetPti() {
    cumulativePti = 0.0f;
}

float SignalProcessor::getCumulativePti() const {
    return cumulativePti;
}

float SignalProcessor::getFilteredCardiac() const {
    return lastCardiacSample;
}

float SignalProcessor::getFilteredRespiration() const {
    return lastRespSample;
}
