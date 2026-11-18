# ArigiaGaurd: Bluetooth Low Energy (BLE) Protocol Specification

## 1. Overview
ArigiaGaurd exposes a Bluetooth Low Energy (BLE 5.0) GATT Server profile enabling real-time telemetry streaming, clinical compliance logging, and priority alarm notifications to hospital caregiver tablets and nurse station monitors.

- **Device Name**: `ArigiaGaurd`
- **Primary Service UUID**: `0000FE40-CC7A-482A-984A-7F2ED5B3E58F`

---

## 2. GATT Characteristic Architecture

### 2.1 Posture & Vitals Telemetry Characteristic
- **UUID**: `0000FE41-CC7A-482A-984A-7F2ED5B3E58F`
- **Properties**: `READ`, `NOTIFY`
- **Update Frequency**: $1\ \text{Hz}$
- **Payload Structure (8 Bytes, Little-Endian)**:

| Byte Offset | Field Name | Data Type | Units / Scaling | Description |
|---|---|---|---|---|
| `0` | `posture_id` | `uint8_t` | Enumeration (0–4) | 0: Supine, 1: Right 30°, 2: Left 30°, 3: Fowler, 4: Prone |
| `1` | `confidence` | `uint8_t` | % (0–100) | Edge Impulse model classification confidence |
| `2-3` | `heart_rate` | `uint16_t` | $\text{bpm} \times 10$ | BCG $J$-wave estimated heart rate |
| `4-5` | `resp_rate` | `uint16_t` | $\text{brpm} \times 10$ | BCG low-frequency respiratory rate |
| `6` | `rmssd` | `uint8_t` | ms (0–255) | HRV metric for sleep-state determination |
| `7` | `motion_flag` | `uint8_t` | Boolean (0/1) | 1 = Body motion artifact active; 0 = Quiescent |

### 2.2 Mattress Pneumatic & Microclimate Characteristic
- **UUID**: `0000FE42-CC7A-482A-984A-7F2ED5B3E58F`
- **Properties**: `READ`, `NOTIFY`
- **Update Frequency**: $1\ \text{Hz}$
- **Payload Structure (10 Bytes, Little-Endian)**:

| Byte Offset | Field Name | Data Type | Units / Scaling | Description |
|---|---|---|---|---|
| `0-1` | `manifold_pressure` | `uint16_t` | $\text{mmHg} \times 10$ | BMP180 chamber pressure ($0$–$100\ \text{mmHg}$) |
| `2-3` | `cumulative_pti` | `uint16_t` | $\text{mmHg}\cdot\text{s}$ | Accumulated Pressure-Time Integral |
| `4-5` | `elapsed_turn_time` | `uint16_t` | seconds | Duration in current posture since last offloading |
| `6` | `active_bank` | `uint8_t` | 0 = Bank A, 1 = Bank B | Currently inflated alternating cell bank |
| `7` | `humidity_rh` | `uint8_t` | % RH (0–100) | SI7021 sacral interface relative humidity |
| `8-9` | `temperature` | `int16_t` | $^\circ\text{C} \times 10$ | SI7021 sacral interface temperature |

### 2.3 Priority Alerts & System Interlocks Characteristic
- **UUID**: `0000FE43-CC7A-482A-984A-7F2ED5B3E58F`
- **Properties**: `READ`, `NOTIFY`
- **Update Mode**: Asynchronous interrupt on alarm condition
- **Payload Structure (2 Bytes, Little-Endian)**:

| Byte Offset | Field Name | Data Type | Bitmask / Value Definitions |
|---|---|---|---|
| `0` | `alert_code` | `uint8_t` | `0x00`: Normal Status<br>`0x01`: Overdue Turn Deadline (> 2h in static posture)<br>`0x02`: Elevated Ischemic PTI (> 1800 mmHg·s)<br>`0x03`: Bottoming-Out Hazard (< 15 mmHg)<br>`0x04`: Maceration Warning (> 65% RH)<br>`0x05`: Severe Fowler Shear Prolonged (> 60 min) |
| `1` | `severity_level` | `uint8_t` | `0`: Nominal (Green)<br>`1`: Advisory Notice (Yellow)<br>`2`: High-Priority Nurse Intervention Required (Red) |

### 2.4 Caregiver Control & Command Characteristic
- **UUID**: `0000FE44-CC7A-482A-984A-7F2ED5B3E58F`
- **Properties**: `WRITE`
- **Payload Structure (2 Bytes)**:
  - `Byte 0`: Command ID (`0x01`: Force Pneumatic Bank Alternation, `0x02`: Reset PTI Counter, `0x03`: Silence Alarm, `0x04`: Initiate Zero-Offset Sensor Calibration)
  - `Byte 1`: Parameter / Checksum
