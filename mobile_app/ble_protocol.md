# DecubiGuard: BLE Telemetry Specification

## Service Definition
- **Service Name**: DecubiGuard Telemetry Service
- **Service UUID**: `0000FE40-CC7A-482A-984A-7F2ED5B3E58F`

## Characteristics

### 1. Vitals Telemetry Characteristic
- **UUID**: `0000FE41-CC7A-482A-984A-7F2ED5B3E58F`
- **Properties**: `READ`, `NOTIFY`
- **Payload Structure (6 Bytes)**:
  - `Byte 0-1`: Heart Rate (uint16_t, bpm * 10)
  - `Byte 2-3`: Respiration Rate (uint16_t, brpm * 10)
  - `Byte 4`: Signal Quality Metric (uint8_t, 0-100%)
  - `Byte 5`: Motion Flag (0 = Still, 1 = Movement detected)

### 2. Mattress Status Characteristic
- **UUID**: `0000FE42-CC7A-482A-984A-7F2ED5B3E58F`
- **Properties**: `READ`, `NOTIFY`
- **Payload Structure (8 Bytes)**:
  - `Byte 0-1`: Elapsed Turn Time (uint16_t, minutes)
  - `Byte 2-3`: Air-Cell Pressure (uint16_t, Pa / 10)
  - `Byte 4`: Active Chamber (0 = Bank A, 1 = Bank B)
  - `Byte 5`: Relative Humidity (uint8_t, % RH)
  - `Byte 6-7`: Temperature (int16_t, degC * 10)

### 3. System Alerts Characteristic
- **UUID**: `0000FE43-CC7A-482A-984A-7F2ED5B3E58F`
- **Properties**: `READ`, `NOTIFY`, `WRITE`
- **Payload Structure (1 Byte)**:
  - `0x00`: Normal Operation
  - `0x01`: Repositioning Due (2h Timer Expired)
  - `0x02`: Bottoming-Out Detected (< 1.5 kPa)
  - `0x03`: Maceration Risk (Elevated Humidity > 75%)
