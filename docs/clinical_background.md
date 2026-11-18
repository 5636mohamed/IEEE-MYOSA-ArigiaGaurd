# ArigiaGaurd: Clinical Background & Pathophysiology

## 1. Pathophysiology of Pressure Injuries
Pressure injuries (decubitus ulcers, bedsores) are localized damage to the skin and underlying soft tissue, typically over bony prominences (sacrum, ischial tuberosities, greater trochanters, heels). They arise from sustained mechanical compression, shear forces, and friction interacting with microclimate factors.

### The Ischemic Threshold & Capillary Closing Pressure
- **Microvascular Occlusion**: Landmark physiology establishes that arteriolar capillary closing pressure ranges between **$30$ to $32\ \text{mmHg}$** ($4.0$–$4.3\ \text{kPa}$).
- **Tissue Loading**: When interface pressure exceeds capillary closing pressure, capillary microvessels collapse, halting microvascular blood flow and producing local tissue ischemia.
- **Pressure-Time Relationship**: Deep tissue injury does not follow a simple static threshold; tissue tolerance is governed by a non-linear inverse sigmoid curve. Extreme pressure causes rapid cell necrosis within minutes, whereas moderate supra-capillary pressure ($>32\ \text{mmHg}$) causes cumulative ischemic hypoxia and reperfusion injury over 1 to 2 hours.
- **The Pressure-Time Integral (PTI)**:
  $$\text{PTI}(t) = \int_0^t \max(0, P(\tau) - P_{\text{cap}})\, d\tau$$
  Quantifying cumulative ischemic insult via PTI allows patient-tailored pressure relief rather than relying on uncalibrated static timers.

## 2. Shear Stress & Elevated Fowler's Angle
When a patient is positioned in Fowler's posture (head-of-bed elevated $\ge 30^\circ$–$45^\circ$), gravitational force causes the torso to slide downward. The deep skeletal frame moves downward while the superficial epidermal layers adhere to the mattress surface. This differential displacement creates severe mechanical **shear stress** at the sacral-fascial plane, stretching and angulating perforator arteries and reducing ischemic tolerance time by over 50%.

## 3. Microclimate Aggravation & Maceration
The local interface microclimate—characterized by temperature ($T$) and relative humidity ($\text{RH}$)—dramatically modulates skin durability:
- **Perspiration & Incontinence**: Elevated relative humidity ($>65\%\ \text{RH}$) weakens the stratum corneum through overhydration (maceration), increasing the friction coefficient by up to $3\times$.
- **Metabolic Demand**: Elevated interface temperature increases local tissue metabolic demand ($Q_{10}$ effect: $\approx 10\%$ increase in metabolic oxygen demand per $1^\circ\text{C}$ rise). When blood flow is concurrently occluded, this exacerbated metabolic demand accelerates tissue death.

## 4. Contactless Ballistocardiography (BCG) & Sleep-Aware Control
Standard alternating-pressure air mattresses (APAMs) cycle pneumatically every 10–12 minutes regardless of patient status. The resulting cyclic inflation and deflation oscillations frequently fragment sleep and awaken vulnerable ICU or palliative patients.
- **Ballistocardiography (BCG)** measures the micro-mechanical recoils of aortic blood ejection without skin electrodes.
- **Sleep Phase Gating**: During deep slow-wave (non-REM) sleep, characterized by elevated heart-rate variability ($RMSSD$) and regularized respiratory rate, the ArigiaGaurd closed-loop controller safely defers non-urgent air-cell deflation cycles as long as the cumulative PTI remains below the critical ischemic safety threshold. This preserves vital restorative sleep architecture while strictly guaranteeing tissue safety.

## 5. Clinical Guidelines Concordance
ArigiaGaurd directly aligns with the **International Guideline on Pressure Ulcer Prevention and Treatment** published jointly by the European Pressure Ulcer Advisory Panel (EPUAP), National Pressure Injury Advisory Panel (NPIAP), and Pan Pacific Pressure Injury Alliance (PPPIA):
- Automated repositioning tracking verifies adherence to the recommended $30^\circ$ lateral tilt turning schedule.
- Microclimate surveillance alerts against moisture-associated skin damage (MASD).
- Contactless vitals provide non-intrusive cardiopulmonary surveillance without disturbing frail, fragile skin.
