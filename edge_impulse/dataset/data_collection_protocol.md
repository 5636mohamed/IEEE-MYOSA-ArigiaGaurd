# DecubiGuard: In-Bed Posture & Ballistocardiography Data Collection Protocol

## 1. Ethical Governance & Volunteer Consent
All data collection procedures conform to the Declaration of Helsinki and institutional ethical guidelines established by the Institutional Review Board (IRB) of the Egypt-Japan University of Science and Technology (E-JUST). Prior to experimentation, all volunteers provided written informed consent.

## 2. Volunteer Cohort Demographics
Data was acquired from $N = 12$ healthy human subjects (7 males, 5 females; age: $24.8 \pm 3.2$ years; BMI: $23.4 \pm 2.8\ \text{kg/m}^2$; height: $172 \pm 9\ \text{cm}$; mass: $68.5 \pm 11.2\ \text{kg}$) across diverse somatotypes (ectomorph, mesomorph, endomorph) to ensure kinematic generalization.

## 3. Sensor Deployment & Mounting Configuration
1. **MYOSA IMU Board (MPU6050)**:
   - Positioned in a rigid 3D-printed enclosure affixed along the central longitudinal axis of the mattress topper directly beneath the sacral loading zone ($S1$–$S3$ anatomical projection).
   - Configured for $\pm 2g$ accelerometer full-scale range and $\pm 250^\circ/\text{s}$ gyroscope full-scale range.
   - Continuous sampling at $f_s = 100\ \text{Hz}$ via hardware timer interrupt over I2C ($400\ \text{kHz}$ Fast Mode).
2. **Pneumatic Pressure Sensor (BMP180)**:
   - Pneumatically coupled to the central manifold of the alternating air cells via medical-grade Luer-lock silicon tubing (inner diameter: $3.2\ \text{mm}$).
   - Operating range: $300$–$1100\ \text{hPa}$, factory-calibrated for internal air-cell pressure tracking ($1\ \text{Hz}$).
3. **Microclimate Sensor (SI7021)**:
   - Positioned at the mattress-sacral interface beneath a breathable, vapor-permeable hospital sheet.
   - Measures relative humidity ($0$–$100\%\ \text{RH}$, $\pm 3\%$) and temperature ($-10$ to $+85^\circ\text{C}$, $\pm 0.4^\circ\text{C}$).
4. **Gold-Standard Reference Instrumentation**:
   - Reference heart rate verified with simultaneous Lead-II ECG monitoring ($1000\ \text{Hz}$).
   - Reference respiratory rate monitored via a thoracic piezoelectric respiratory effort belt.

## 4. Posture Recording Protocol
Each subject underwent a structured 60-minute experimental protocol across five standardized in-bed postures:

| Class ID | Posture Label | Clinical Description | Support Device | Duration / Subject |
|---|---|---|---|---|
| **0** | `supine` | Horizontal dorsal decubitus position ($0^\circ$ incline) | Standard hospital pillow | 15 minutes |
| **1** | `right_lateral_30` | Right-tilted $30^\circ$ lateral recumbent position | $30^\circ$ viscoelastic foam wedge behind back | 12 minutes |
| **2** | `left_lateral_30` | Left-tilted $30^\circ$ lateral recumbent position | $30^\circ$ viscoelastic foam wedge behind back | 12 minutes |
| **3** | `fowler` | Head-of-bed elevation ($30^\circ$ to $45^\circ$) | Articulating hospital bed frame | 12 minutes |
| **4** | `prone` | Ventral decubitus position ($0^\circ$ incline, head turned sideways) | Flat mattress | 9 minutes |

### Dynamic Repositioning Transitions
Between static posture blocks, subjects executed active and passive repositioning transitions (turn from supine to lateral, elevation of head-of-bed, shifting) lasting 10–30 seconds. Transition kinematics were logged to benchmark transient artifact rejection.

## 5. Ballistocardiographic (BCG) Recording Criteria
- **Cardiac Micro-vibrations**: During static postures, minute mechanical recoils of aortic blood ejection ($J$-waves, amplitude $5$–$25\ \text{mg}$) propagate through the mattress foam to the MPU6050 $Z$-axis.
- **Respiratory Excursions**: Low-frequency chest wall displacement ($0.1$–$0.5\ \text{Hz}$, amplitude $30$–$100\ \text{mg}$) couples into the accelerometer axes.
- **Motion Artifact Gating**: Any segment exhibiting aggregate acceleration variance $\sigma^2 > 0.05\ g^2$ or angular velocity $|\omega| > 15^\circ/\text{s}$ is flagged as gross body movement; BCG vitals extraction is paused during the transient window to prevent false cardiac peak detection.

## 6. Data Formatting & Edge Impulse Ingestion
Data is recorded in CSV format with continuous 6-axis kinematic features sampled at $100\ \text{Hz}$:
```csv
timestamp_ms,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,posture_label
0,0.012,-0.008,0.985,0.12,-0.05,0.02,supine
10,0.015,-0.007,0.984,0.10,-0.04,0.01,supine
...
```
Windows of $2000\ \text{ms}$ (200 samples) with $1000\ \text{ms}$ step size are segmented, zero-mean normalized, and ingested into Edge Impulse Studio for feature extraction and model compilation.
