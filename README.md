# DecubiGuard: Smart Anti-Bedsore Mattress System 🛏️

**A MYOSA-Based Smart Anti-Bedsore Mattress System with Contactless Vitals, TinyML Posture Classification, and Closed-Loop Pressure Redistribution**

> IEEE MYOSA Competition Entry  
> Department of Biomedical and Mechatronics Engineering  
> Egypt Japan University of Science & Technology (E-JUST)  
> Authors: **Abdelrahman Hamza, Abdelrahman Wael, Mohamed Elhariry**

---

## Overview

Pressure ulcers (decubitus injuries) affect over **10% of hospitalized patients** and more than **50% of individuals with spinal cord injuries or prolonged immobility**. Standard alternating-pressure air mattresses (APAMs) operate strictly open-loop on uncalibrated mechanical timers without posture, tissue ischemia, or microclimate awareness.

**DecubiGuard** is an autonomous smart mattress system built entirely on the **modular MYOSA Mini IoT platform**, requiring no external microcontrollers or custom PCBs. It integrates:

- 🧠 **On-Device TinyML Posture Classification**: Classifies 5 clinical sleep postures (*Supine, Right-Lateral 30°, Left-Lateral 30°, Prone, Fowler's*) directly on the ESP32 core via Edge Impulse (>94% accuracy).
- 💓 **Contactless Vitals Monitoring**: Extracts real-time ballistocardiographic (BCG) $J$-waves and respiratory excursions via an under-mattress MPU6050 IMU, requiring zero adhesive skin electrodes.
- 🔄 **Dynamic Pressure-Time Integral (PTI) Engine**: Replaces static timers with real-time cumulative ischemia tracking ($\int [P(t) - P_{cap}]^+ dt$), auto-titrating alternating air cell cycles.
- 🌙 **Sleep-Phase-Aware Deflation**: Uses BCG heart-rate variability ($RMSSD$) to detect deep non-REM sleep, deferring disruptive pneumatic cycles when cumulative tissue loading allows.
- 💧 **Sacral Microclimate Surveillance**: Monitors local temperature and relative humidity via an SI7021 sensor to alert against sweat accumulation and tissue maceration before breakdown occurs.
- ⚡ **Optoisolated Mains Safety**: Drives line-voltage 220V/110V pneumatic compressors via an optically isolated zero-crossing TRIAC circuit (MOC3041 + BT136) with 5,000 V RMS galvanic isolation.

---

## System Architecture

The DecubiGuard processing pipeline proceeds across five sequential edge-computing layers:

1. **Multi-Modal Sensing**: MPU6050 (BCG & kinematics), BMP180 (manifold pressure), SI7021 (sacral microclimate).
2. **Preprocessing & Filtering**: Dual 4th-order digital Butterworth filters (0.1–0.5 Hz for respiration; 0.8–3.5 Hz for cardiac $J$-waves).
3. **TinyML Inference**: Quantized INT8 1D-CNN running on the MYOSA ESP32 microcontroller.
4. **Closed-Loop Decision Logic**: Dynamic PTI accumulation, bottoming-out safety interlocks, and maceration alarm arbitration.
5. **Actuation & Telemetry**: Zero-crossing TRIAC pump switching, bedside OLED/buzzer alerts, and BLE 5.0 wireless telemetry to caregiver mobile devices.

---

## Hardware Modules

| Module | Interface | Clinical Role |
|---|---|---|
| **MYOSA Motherboard** | ESP32 Core | Central coordinator: sensor acquisition, TinyML inference, PTI logic, BLE GATT server |
| **MYOSA IMU Board** | I2C (`0x68`) | 6-axis MPU6050 captures repositioning kinematics and ballistocardiographic micro-vibrations |
| **MYOSA Pressure Board** | I2C (`0x77`) | BMP180 barometric sensor pneumatically coupled to alternating air cell manifold |
| **MYOSA Microclimate Board** | I2C (`0x40`) | SI7021 digital sensor tracking sacral interface relative humidity and temperature |
| **MYOSA TRIAC Module** | Opto GPIO | Optoisolated zero-crossing AC driver (MOC3041 + BT136) controlling compressor pump |
| **MYOSA OLED & Buzzer** | I2C / PWM | 0.96" SSD1306 bedside display and active acoustic alarm transducer |
| **BLE Radio (on-board)** | 2.4 GHz | Transmits telemetry packets, compliance logs, and alarms to mobile dashboard |

---

## Repository Structure

```
├── docs/
│   ├── DecubiGuard_Research_Paper.pdf    # Full 5-page IEEE EMBS MYOSA research paper
│   ├── main.tex                          # Complete calibrated LaTeX source code
│   └── demo_guide.md                     # 4-stage conference live demonstration protocol
├── firmware/
│   ├── DecubiGuard_Firmware.ino          # Arduino / PlatformIO embedded application
│   ├── include/
│   │   └── config.h                      # Hardware pins, I2C addresses, and clinical thresholds
│   └── src/
├── hardware/
│   └── schematics_and_bom.md             # Complete BOM, pinouts, and mains safety schematics
├── mobile_app/
│   └── ble_protocol.md                   # Custom BLE GATT service and characteristic specification
├── scripts/
│   └── bcg_signal_processor.py           # Offline BCG bandpass filter and J-wave peak extraction
├── tests/
│   └── test_signal_processing.py         # Automated unit tests for signal extraction
├── requirements.txt                      # Python dependencies
└── README.md
```

---

## Live Demonstration Stages

- **Stage 1 -- Contactless Vitals**: Real-time BCG heart rate and respiration rate displayed on OLED within 12 seconds.
- **Stage 2 -- TinyML Posture Recognition**: Autonomous classification of 5 clinical postures and automatic turn timer reset upon voluntary rotation.
- **Stage 3 -- Closed-Loop Pressure Relief**: Active cell bank alternation and immediate (<3.5 s) failsafe response to simulated air leaks.
- **Stage 4 -- Microclimate & Mains Safety**: Sacral moisture alert triggering and oscilloscope verification of zero-crossing transient-free AC switching.

---

## References

1. J. Kottner, J. Cuddigan, K. Carville, and E. Haesler, "A closer look at the 2019 International Guideline on the prevention and treatment of pressure ulcers/injuries," *Journal of Tissue Viability*, vol. 29, no. 4, pp. 228–231, 2020. [doi:10.1016/j.jtv.2020.09.003](https://doi.org/10.1016/j.jtv.2020.09.003)
2. A. Gefen, D. M. Brienza, J. Cuddigan, E. Haesler, and J. Kottner, "Our contemporary understanding of the aetiology of pressure ulcers/pressure injuries," *International Wound Journal*, vol. 19, no. 3, pp. 692–704, 2022. [doi:10.1111/iwj.13667](https://doi.org/10.1111/iwj.13667)
3. C. Shi, J. C. Dumville, N. Cullum, S. Rhodes, A. Jammali-Blasi, and E. McInnes, "Alternating pressure (active) air surfaces for preventing pressure ulcers," *Cochrane Database of Systematic Reviews*, no. 5, Art. no. CD013620, 2021. [doi:10.1002/14651858.CD013620.pub2](https://doi.org/10.1002/14651858.CD013620.pub2)
4. P. Avsar, Z. Moore, D. Patton, T. O'Connor, A. M. Budri, and L. Nugent, "Repositioning for preventing pressure ulcers: A systematic review and meta-analysis," *Journal of Wound Care*, vol. 29, no. 9, pp. 496–508, 2020. [doi:10.12968/jowc.2020.29.9.496](https://doi.org/10.12968/jowc.2020.29.9.496)
5. J. Kottner, J. Black, E. Call, A. Gefen, and N. Santamaria, "Microclimate: A critical review in the context of pressure ulcer prevention," *Clinical Biomechanics*, vol. 59, pp. 62–70, 2018. [doi:10.1016/j.clinbiomech.2018.09.010](https://doi.org/10.1016/j.clinbiomech.2018.09.010)
6. O. T. Inan, P.-F. Migeotte, K.-S. Park, M. Etemadi, K. Tavakolian, R. Casanella, J. Zanetti, J. Tank, I. Funtova, G. K. Prisk, and M. Di Rienzo, "Ballistocardiography and seismocardiography: A review of recent advances," *IEEE Journal of Biomedical and Health Informatics*, vol. 19, no. 4, pp. 1414–1427, 2015. [doi:10.1109/JBHI.2014.2361732](https://doi.org/10.1109/JBHI.2014.2361732)
7. C. Brüser, K. Stadlthanner, S. de Waele, and S. Leonhardt, "Adaptive heartbeat modeling for beat-to-beat heart rate measurement in ballistocardiograms," *IEEE Journal of Biomedical and Health Informatics*, vol. 19, no. 2, pp. 454–463, 2015. [doi:10.1109/JBHI.2014.2314144](https://doi.org/10.1109/JBHI.2014.2314144)
8. G. Matar, J.-M. Lina, and G. Kaddoum, "Artificial neural network for in-bed posture classification using bed-sheet pressure sensors," *IEEE Journal of Biomedical and Health Informatics*, vol. 24, no. 1, pp. 125–136, 2020. [doi:10.1109/JBHI.2019.2899070](https://doi.org/10.1109/JBHI.2019.2899070)
9. G. Peruzzi, A. Galli, G. Giorgi, and A. Pozzebon, "Sleep posture detection via embedded machine learning on a reduced set of pressure sensors," *Sensors*, vol. 25, no. 2, p. 458, 2025. [doi:10.3390/s25020458](https://doi.org/10.3390/s25020458)
10. P. Warden and D. Situnayake, *TinyML: Machine Learning with TensorFlow Lite on Arduino and Ultra-Low-Power Microcontrollers*. Sebastopol, CA: O'Reilly Media, 2019.
11. M. J. G. Pezzolesi, D. L. Bader, and C. W. J. Oomens, "Deformation-induced damage in skeletal muscle tissue: An in vitro model for deep tissue injury," *Journal of Biomechanics*, vol. 42, no. 14, pp. 2221–2228, 2009. [doi:10.1016/j.jbiomech.2009.06.050](https://doi.org/10.1016/j.jbiomech.2009.06.050)
12. J. L. Lever, K. Constantinescu, and S. Patel, "Wearable and unobtrusive sensor systems for bedridden patient monitoring: A review," *IEEE Reviews in Biomedical Engineering*, vol. 14, pp. 122–135, 2021. [doi:10.1109/RBME.2020.3001234](https://doi.org/10.1109/RBME.2020.3001234)

---

## Authors & Acknowledgments

- **Abdelrahman Hamza**
- **Abdelrahman Wael**
- **Mohamed Elhariry**

*Department of Mechatronics and Robotics Engineering, Egypt Japan University of Science and Technology (E-JUST)*

*Department of Biomedical and Bioinformatics Engineering, Egypt Japan University of Science and Technology (E-JUST)*

*Department of Electronics and Communication Engineering, Egypt Japan University of Science and Technology (E-JUST)*
