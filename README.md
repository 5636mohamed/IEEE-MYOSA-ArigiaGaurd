# DecubiGuard: Smart Anti-Bedsore Mattress System

An autonomous pressure ulcer prevention and vital signs monitoring system based on the modular MYOSA Mini IoT architecture.

## Overview
DecubiGuard provides continuous pressure redistribution and non-invasive physiological monitoring for bedridden patients. The system combines alternating-pressure air cell control with ballistocardiography (BCG) for contactless heart and respiration rate tracking.

## System Features
- **Repositioning Tracking**: Monitors patient posture and turn intervals via 6-axis IMU.
- **Closed-Loop Pressure Regulation**: Regulates pneumatic cell pressure using barometric sensing and optoisolated TRIAC mains pump control.
- **Contactless Vitals**: Extracts ballistocardiographic recoil signals to estimate heart rate and respiration rate without skin contact.
- **Microclimate Surveillance**: Tracks sacral interface temperature and relative humidity to mitigate maceration risk.
- **Local & Remote Feedback**: Displays real-time metrics on an OLED screen, triggers audio alerts via buzzer, and broadcasts telemetry over Bluetooth Low Energy (BLE).

## Repository Structure
```
├── docs/
│   ├── DecubiGuard_Research_Paper.pdf
│   └── main.tex
├── firmware/
│   ├── DecubiGuard_Firmware.ino
│   ├── include/
│   └── src/
├── hardware/
│   └── schematics_and_bom.md
├── mobile_app/
│   └── ble_protocol.md
├── scripts/
│   ├── serial_monitor.py
│   └── bcg_signal_processor.py
└── tests/
    └── test_signal_processing.py
```

## Authors
- **Abdelrahman Hamza**
- **Abdelrahman Wael**
- **Mohamed Elhariry**

*Department of Biomedical and Mechatronics Engineering, Egypt Japan University of Science and Technology (E-JUST)*
