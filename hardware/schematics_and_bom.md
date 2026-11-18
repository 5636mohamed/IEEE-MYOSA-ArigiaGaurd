# ArigiaGaurd: Hardware Architecture & Bill of Materials

## Overview
ArigiaGaurd is designed using modular components integrated around the MYOSA Mini IoT platform.

## Bill of Materials (BOM)

| Component | Function | Interface / Specifications |
|---|---|---|
| **MYOSA Motherboard** | Central microcontroller, BLE stack, scheduling | ESP32-based core, I2C, SPI, GPIO |
| **MYOSA IMU Board (MPU6050)** | Contactless BCG vitals & repositioning detection | 3-axis accelerometer, 3-axis gyro (I2C) |
| **MYOSA Pressure Board (BMP180)** | Pneumatic chamber manifold pressure feedback | Piezoresistive pressure sensor (I2C) |
| **MYOSA Humidity Board (SI7021)** | Microclimate sacral relative humidity & temperature | Digital hygrometer & thermometer (I2C) |
| **MYOSA OLED Module** | Bedside caregiver graphical feedback | 0.96" SSD1306 128x64 display (I2C) |
| **MYOSA Buzzer Module** | Acoustic warning for overdue turns / maceration risk | Piezoelectric transducer (Active High) |
| **Optoisolated TRIAC Circuit** | Mains AC alternating-pressure pump driver | MOC3041 Zero-Cross Optocoupler + BT136 TRIAC |
| **Dual Solenoid Manifold** | Pneumatic alternation between Bank A and Bank B | 12V DC latching 3-way pneumatic valves |

## Mains Safety Interfacing
The alternating pneumatic compressor runs from 220V/110V AC mains. To ensure patient and operator safety:
- **Optical Isolation**: An optical barrier provides 5000 Vrms isolation between the MYOSA low-voltage bus and the mains load.
- **Zero-Crossing Detection**: Switching occurs at mains zero-crossing points, minimizing electromagnetic interference (EMI) and current surges.
- **RC Snubber Network**: A 100 nF X2 capacitor with 39 ohm damping resistor absorbs inductive back-EMF transients from the compressor motor.
