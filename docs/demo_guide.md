# DecubiGuard: Interactive Demonstration Guide

This guide outlines the 4-stage operational demonstration protocol for evaluating the DecubiGuard smart anti-bedsore mattress system.

---

## Demonstration Protocol

### Stage 1: Contactless Vitals & Posture Acquisition
1. Power up the MYOSA motherboard via USB-C.
2. Observe the OLED display as it initializes sensor communication with the MPU6050, BMP180, and SI7021.
3. A subject lies on the mattress in the supine position.
4. The system acquires accelerometer micro-vibrations and displays estimated heart rate (bpm) and respiration rate (brpm) within 15 seconds.

### Stage 2: Repositioning Event & Timer Reset
1. Have the subject perform a lateral turn from supine to the right-lateral position.
2. The IMU detects the angular excursion (>30 degrees across the longitudinal axis).
3. The turn counter increments, and the 2-hour elapsed turn timer automatically resets to zero.

### Stage 3: Closed-Loop Pressure Relief & Bank Alternation
1. Observe the alternating pressure manifold.
2. The system triggers the optoisolated TRIAC circuit, activating the compressor.
3. Bank A inflates while Bank B deflates, redistributing contact interface pressures away from bony prominences.
4. The BMP180 confirms chamber pressure reaches nominal operational range (2.5 - 3.5 kPa) without bottoming-out.

### Stage 4: Microclimate Surveillance & Alert Generation
1. Simulate elevated relative humidity near the sacral zone (e.g., using warm moisture source).
2. The SI7021 registers humidity exceeding 75% RH.
3. The MYOSA buzzer emits a pulsed acoustic alert, and the BLE characteristic broadcasts a maceration alert to the caregiver app.
