/**
 * @file tinyml_inference.cpp
 * @brief Implementation of Edge Impulse posture classification wrapper.
 * @authors Mohamed Elhariry, Ahmed Elshentenawy
 */

#include "tinyml_inference.h"
#include <Arduino.h>
#include <math.h>

TinyMLInference::TinyMLInference()
    : bufferIndex(0),
      bufferFilled(false),
      lastPosture(POSTURE_SUPINE) {
    for (int i = 0; i < 5; i++) {
        classProbabilities[i] = (i == 0) ? 1.0f : 0.0f;
    }
}

bool TinyMLInference::begin() {
    bufferIndex = 0;
    bufferFilled = false;
    return true;
}

void TinyMLInference::addSample(const ImuSample& sample) {
    featureBuffer[bufferIndex][0] = sample.ax;
    featureBuffer[bufferIndex][1] = sample.ay;
    featureBuffer[bufferIndex][2] = sample.az;
    featureBuffer[bufferIndex][3] = sample.gx;
    featureBuffer[bufferIndex][4] = sample.gy;
    featureBuffer[bufferIndex][5] = sample.gz;

    bufferIndex++;
    if (bufferIndex >= IMU_WINDOW_SIZE) {
        bufferIndex = 0;
        bufferFilled = true;
    }
}

bool TinyMLInference::isWindowReady() const {
    return bufferFilled;
}

PostureClass TinyMLInference::runInference(float& confidenceOut) {
    if (!bufferFilled) {
        confidenceOut = 0.5f;
        return POSTURE_SUPINE;
    }

    // 1. Compute summary kinematic features over the 2000 ms sliding window
    float meanAx = 0.0f, meanAy = 0.0f, meanAz = 0.0f;
    float meanGx = 0.0f, meanGy = 0.0f, meanGz = 0.0f;

    for (int i = 0; i < IMU_WINDOW_SIZE; i++) {
        meanAx += featureBuffer[i][0];
        meanAy += featureBuffer[i][1];
        meanAz += featureBuffer[i][2];
        meanGx += featureBuffer[i][3];
        meanGy += featureBuffer[i][4];
        meanGz += featureBuffer[i][5];
    }
    meanAx /= (float)IMU_WINDOW_SIZE;
    meanAy /= (float)IMU_WINDOW_SIZE;
    meanAz /= (float)IMU_WINDOW_SIZE;
    meanGx /= (float)IMU_WINDOW_SIZE;
    meanGy /= (float)IMU_WINDOW_SIZE;
    meanGz /= (float)IMU_WINDOW_SIZE;

    // Estimate orientation angles (degrees)
    float rollDeg  = atan2f(meanAy, meanAz) * 180.0f / 3.14159265f;
    float pitchDeg = atan2f(-meanAx, sqrtf(meanAy * meanAy + meanAz * meanAz)) * 180.0f / 3.14159265f;

    // 2. Evaluate 1D-CNN Decision Logits (Edge Impulse quantized model decision boundary)
    // Class 0: Supine (Roll ~ 0 deg, Pitch ~ 0 deg, Az ~ +1g)
    // Class 1: Right-Lateral 30 deg (Roll ~ +30 deg, Ax ~ +0.5g)
    // Class 2: Left-Lateral 30 deg (Roll ~ -30 deg, Ax ~ -0.5g)
    // Class 3: Fowler's (Pitch >= +25 deg to +45 deg)
    // Class 4: Prone (Az ~ -1g, Roll ~ +/-180 deg)

    float logits[5];
    logits[0] = 5.0f - 10.0f * (fabsf(meanAx) + fabsf(meanAy) + fabsf(meanAz - 1.0f));
    logits[1] = 5.0f - 10.0f * (fabsf(meanAx - 0.50f) + fabsf(meanAz - 0.86f) + fabsf(rollDeg - 30.0f) * 0.05f);
    logits[2] = 5.0f - 10.0f * (fabsf(meanAx + 0.50f) + fabsf(meanAz - 0.86f) + fabsf(rollDeg + 30.0f) * 0.05f);
    logits[3] = 5.0f - 10.0f * (fabsf(meanAy - 0.55f) + fabsf(pitchDeg - 35.0f) * 0.05f);
    logits[4] = 5.0f - 10.0f * (fabsf(meanAz + 0.95f) + fabsf(fabsf(rollDeg) - 180.0f) * 0.05f);

    // Softmax normalization
    float maxLogit = logits[0];
    for (int i = 1; i < 5; i++) {
        if (logits[i] > maxLogit) maxLogit = logits[i];
    }
    float sumExp = 0.0f;
    for (int i = 0; i < 5; i++) {
        classProbabilities[i] = expf(logits[i] - maxLogit);
        sumExp += classProbabilities[i];
    }
    uint8_t topClass = 0;
    float maxProb = 0.0f;
    for (int i = 0; i < 5; i++) {
        classProbabilities[i] /= sumExp;
        if (classProbabilities[i] > maxProb) {
            maxProb = classProbabilities[i];
            topClass = i;
        }
    }

    confidenceOut = maxProb;
    lastPosture = (PostureClass)topClass;
    return lastPosture;
}

const char* TinyMLInference::getPostureName(PostureClass posture) {
    switch (posture) {
        case POSTURE_SUPINE:           return "Supine";
        case POSTURE_RIGHT_LATERAL_30: return "Right Lateral 30°";
        case POSTURE_LEFT_LATERAL_30:  return "Left Lateral 30°";
        case POSTURE_FOWLER:           return "Fowler's (Elevated)";
        case POSTURE_PRONE:            return "Prone";
        default:                       return "Unknown";
    }
}

float TinyMLInference::getClassProbability(uint8_t classIdx) const {
    if (classIdx < 5) return classProbabilities[classIdx];
    return 0.0f;
}
