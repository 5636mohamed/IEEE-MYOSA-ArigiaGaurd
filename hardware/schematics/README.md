# DecubiGuard: Circuit Schematic & Interfacing Guide

## Overview
This directory contains electrical schematics, pin mappings, and isolation topologies for DecubiGuard.

## Circuit Architecture

```
                                  [220V/110V AC Mains]
                                           |
                                           v
[ESP32 / MYOSA] ---GPIO 25---> [MOC3041 Optocoupler] ---> [BT136 TRIAC] ---> [Pneumatic Pump]
(Low-Voltage DC Domain)        (5,000 Vrms Galvanic       (Zero-Crossing      (AC Motor Load)
                                Optical Barrier)          Switching)
                                                               |
                                                      [RC Snubber: 39Ω + 0.01µF]

[I2C Bus: GPIO 21 (SDA), GPIO 22 (SCL)]
   ├── MPU6050 (0x68) : 6-axis IMU for BCG micro-vibrations & posture
   ├── BMP180  (0x77) : Manifold air-cell pneumatic pressure
   ├── SI7021  (0x40) : Sacral interface temperature & relative humidity
   └── SSD1306 (0x3C) : 0.96" OLED display

[Bedside Alert Peripheral]
   └── Buzzer: GPIO 18 (Active High Piezo Transducer)
```

## Mains Safety & Isolation Specifications
- **Optical Barrier**: Fairchild MOC3041 zero-crossing phototriac providing 5,000 V RMS electrical breakdown protection.
- **Creepage & Clearance**: Minimum 8 mm creepage distance between the AC mains traces and the low-voltage DC logic ground on the PCB.
- **Snubber Protection**: Metal film resistor ($39\ \Omega$, $1\ \text{W}$) in series with a Class X2 suppression capacitor ($0.01\ \mu\text{F}$, $400\ \text{V}$) to suppress inductive voltage kickback from the pump motor coil during turn-off.
- **Fuse Rating**: $2.0\ \text{A}$ fast-acting ceramic cartridge fuse on AC Line input.

> **[HARDWARE IMPLEMENTATION NOTICE]**  
> High-voltage AC mains switching (110V/220V) and optocoupled zero-crossing TRIAC drivers require physical bench hardware deployment and cannot be simulated in software. Always exercise high-voltage safety procedures during physical bench validation.
