# DecubiGuard: Interactive Demonstration Guide

This guide details the 4-stage operational demonstration protocol for the DecubiGuard smart anti-bedsore mattress system during IEEE MYOSA 6.0 competition evaluations and live audience exhibitions.

---

## Equipment & Setup Checklist
- [x] MYOSA Motherboard running `DecubiGuard_Firmware`
- [x] MYOSA IMU Board (MPU6050) installed in sacral sensor pod
- [x] MYOSA Pressure Board (BMP180) connected to pneumatic manifold
- [x] MYOSA Microclimate Board (SI7021) mounted at mattress-sheet interface
- [x] MYOSA OLED (SSD1306) and Piezo Buzzer
- [x] Alternating pressure mattress pad with dual-bank air cells
- [x] Optocoupled zero-crossing TRIAC module and pneumatic air compressor
- [x] Laptop running `scripts/serial_monitor.py` and `scripts/data_visualizer.py`
- [x] BLE client mobile application or Generic GATT browser (nRF Connect)

---

## Demonstration Protocol

| Stage | Demonstration Title | Action / Procedure | Expected Output & Telemetry |
|---|---|---|---|
| **1** | **Contactless Vitals & Baseline Supine Classification** | Volunteer lies motionless in the supine position on the mattress. | OLED displays: `Posture: Supine`, `HR: 71 bpm`, `RR: 15 brpm`. Laptop plot displays real-time filtered BCG $J$-wave micro-vibrations and sinusoidal respiratory excursions. |
| **2** | **Dynamic 30° Lateral Turn & PTI Timer Reset** | Volunteer shifts from supine into a $30^\circ$ right-lateral therapeutic tilt using a bolster wedge. | TinyML model detects posture transition to `right_lateral_30` within 1.2 s. OLED confirms turn compliance (`Turn OK ✓`), and the cumulative sacral Pressure-Time Integral (PTI) counter auto-resets. |
| **3** | **Closed-Loop TRIAC Alternation & Sleep Deferral** | System simulates sustained supra-capillary loading ($\text{PTI} \ge 1800\ \text{mmHg}\cdot\text{s}$). | Optoisolated zero-crossing TRIAC triggers compressor pump. Bank A deflates to offload sacrum while Bank B inflates. In sleep mode ($RMSSD > 45\ \text{ms}$), cycle smoothly defers until safe interval. |
| **4** | **Microclimate Moisture Spike & Caregiver Alarm** | Introduce elevated humidity (>65% RH) at the sensor interface. | SI7021 registers moisture spike; OLED displays `WARNING: MACERATION RISK`; buzzer emits audible warning; BLE characteristic pushes high-priority alarm packet to smartphone. |

---

## Step-by-Step Execution Guide

### Stage 1 — Contactless Vitals & Baseline Posture
1. Launch the serial monitor:
   ```bash
   python scripts/serial_monitor.py --port COM3 --baud 115200
   ```
2. Observe initialization sequence:
   ```
   [BOOT] DecubiGuard System Initializing...
   [I2C] MPU6050: OK (0x68), BMP180: OK (0x77), SI7021: OK (0x40), OLED: OK (0x3C)
   [AI] Loading Quantized INT8 1D-CNN Posture Model... Ready (12.3 ms latency)
   [BLE] Advertising as 'DecubiGuard-Caregiver'
   ```
3. Volunteer lies supine. Within 10–15 seconds, cardiac $J$-waves and respiratory envelope stabilize.

### Stage 2 — Therapeutic Repositioning Detection
1. Subject transitions to a $30^\circ$ right-lateral tilt with therapeutic wedge support.
2. The 1D-CNN classifies the posture:
   ```
   [TINYML] Inference: right_lateral_30 (Confidence: 96.2%)
   [PTI] Sacral offloading detected. Reperfusion timer started. PTI reset to 0.
   ```
3. Bedside OLED highlights `Turn Verified ✓`.

### Stage 3 — Closed-Loop Pressure Redistribution
1. Allow cumulative ischemic exposure to rise or press test button to simulate elapsed turn deadline.
2. The closed-loop controller triggers the TRIAC gate:
   ```
   [ACTUATOR] PTI threshold exceeded (1820 mmHg·s). Triggering TRIAC pump cycle...
   [VALVE] Deflating Bank A (18 mmHg) -> Inflating Bank B (34 mmHg)
   [BMP180] Manifold pressure verified at 32.4 mmHg. Bottoming-out safety interlock: CLEAR.
   ```

### Stage 4 — Microclimate Alert Simulation
1. Expose the SI7021 to a warm humidified airflow.
2. Interface humidity rises above $65\%\ \text{RH}$:
   ```
   [MICROCLIMATE] RH: 71.4%, Temp: 35.8 C -> THRESHOLD EXCEEDED
   [ALERT] Buzzing active alarm! BLE notification sent: ALERT_MACERATION_RISK
   ```
3. The companion mobile dashboard triggers a visual red banner alert for caregiver intervention.
