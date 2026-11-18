/**
 * @file tinyml_inference.h
 * @brief Edge Impulse quantized 1D-CNN posture classifier wrapper for ArigiaGaurd.
 * @author Mohamed Elhariry
 */

#ifndef TINYML_INFERENCE_H
#define TINYML_INFERENCE_H

#include "config.h"
#include "sensor_acquisition.h"

class TinyMLInference {
public:
    TinyMLInference();
    bool begin();
    
    // Ingests a new 6-axis IMU sample into the circular feature buffer
    void addSample(const ImuSample& sample);
    
    // Returns true when enough samples (IMU_WINDOW_SIZE) are accumulated for inference
    bool isWindowReady() const;
    
    // Executes on-device quantized INT8 inference, populating confidence and returning posture
    PostureClass runInference(float& confidenceOut);
    
    // Helper to get printable string label
    static const char* getPostureName(PostureClass posture);

    // Get individual class probability (0 to 4)
    float getClassProbability(uint8_t classIdx) const;

private:
    float featureBuffer[IMU_WINDOW_SIZE][6];
    uint16_t bufferIndex;
    bool bufferFilled;
    float classProbabilities[5];
    PostureClass lastPosture;
};

#endif // TINYML_INFERENCE_H
