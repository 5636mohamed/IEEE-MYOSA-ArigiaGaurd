# DecubiGuard: 3D CAD Enclosure & Mechanical Mounts

## Overview
This directory contains the physical enclosure designs for housing the DecubiGuard sensor pod and pneumatic manifold bracket onto standard hospital bed frames.

## Files
1. `sensor_pod_enclosure_top.stl`: Top snap-fit shell for the MPU6050 and SI7021 sub-assembly.
2. `sensor_pod_enclosure_bottom.stl`: Base plate with silicone dampening grommets and strap loops.
3. `manifold_mount_bracket.stl`: Bed-rail clip bracket for securing the pneumatic Luer-lock manifold and BMP180 barometric chamber.

## 3D Printing & Fabrication Parameters
- **Material**: Medical-grade Biocompatible PETG or ABS (autoclavable / wipeable with 70% isopropyl alcohol).
- **Infill**: 40% gyroid infill for structural rigidity under patient weight.
- **Layer Height**: 0.16 mm – 0.20 mm.
- **Supports**: Minimal build-plate supports required for snap-fit latches.

> **[HARDWARE DEPLOYMENT NOTICE]**  
> Physical STL files represent physical mechanical artifacts intended for 3D stereolithography / FDM fabrication. These mechanical components require physical bench fabrication and cannot be simulated in software.
