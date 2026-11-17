"""
DecubiGuard - Real-Time Serial Monitor & Telemetry Parser
Listens to MYOSA ESP32 UART output or runs interactive simulation if hardware is not connected.
"""

import sys
import time
import argparse
import random

POSTURES = ["Supine", "Right Lateral 30°", "Left Lateral 30°", "Fowler's (Elevated)", "Prone"]

def run_hardware_monitor(port: str, baud: int):
    """Monitors live physical serial port."""
    try:
        import serial
    except ImportError:
        print("[ERROR] pyserial is not installed. Run: pip install pyserial")
        sys.exit(1)

    print(f"[INFO] Connecting to MYOSA device on {port} @ {baud} baud...")
    try:
        ser = serial.Serial(port, baud, timeout=1.0)
        print(f"[SUCCESS] Connected! Streaming DecubiGuard telemetry:\n")
        while True:
            line = ser.readline().decode('utf-8', errors='replace').strip()
            if line:
                print(line)
    except KeyboardInterrupt:
        print("\n[INFO] Monitor stopped by user.")
    except Exception as e:
        print(f"[ERROR] Serial connection failed: {e}")

def run_simulated_monitor():
    """Runs a simulated real-time telemetry stream."""
    print("[INFO] Running in Simulation Mode (Hardware loopback)...")
    print("[INFO] Press Ctrl+C to terminate.\n")
    
    posture_idx = 0
    confidence = 0.96
    hr = 72.0
    rr = 15.0
    pressure = 32.4
    pti = 120.0
    humidity = 49.0
    temp = 34.2
    active_bank = "BANK A"
    step = 0

    try:
        while True:
            step += 1
            # Periodic posture transitions
            if step % 15 == 0:
                posture_idx = (posture_idx + 1) % len(POSTURES)
                if posture_idx in [1, 2, 4]:
                    pti = max(0.0, pti - 80.0) # Reperfusion
                print(f"\n>>> [EVENT] Patient Repositioning Detected: {POSTURES[posture_idx]} <<<\n")

            # Small physiological fluctuations
            hr += random.uniform(-1.0, 1.0)
            rr += random.uniform(-0.3, 0.3)
            humidity += random.uniform(-0.2, 0.4)
            temp += random.uniform(-0.05, 0.05)

            # Accumulate ischemia in supine or fowler
            if posture_idx in [0, 3]:
                pti += random.uniform(1.5, 3.5)
            else:
                pti = max(0.0, pti - 5.0)

            # Alternation trigger
            if pti > 1800.0 or step % 25 == 0:
                active_bank = "BANK B" if active_bank == "BANK A" else "BANK A"
                print(f"[ACTUATOR] Closed-Loop Redistribution: Switched to {active_bank}")

            alert = "ALL SYSTEMS NOMINAL ✓"
            if pti > 1800.0:
                alert = "ALARM: CRITICAL ISCHEMIC PTI EXCEEDED"
            elif humidity > 65.0:
                alert = "WARNING: MACERATION RISK DETECTED"

            print(f"[{time.strftime('%H:%M:%S')}] Posture: {POSTURES[posture_idx]:<20} | HR: {hr:.1f} bpm | RR: {rr:.1f} brpm | Cell: {pressure:.1f} mmHg | PTI: {pti:.1f} | RH: {humidity:.1f}% | {alert}")
            time.sleep(1.0)
    except KeyboardInterrupt:
        print("\n[INFO] Simulation monitor terminated.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="DecubiGuard Serial Monitor")
    parser.add_argument("--port", type=str, default=None, help="Serial port (e.g., COM3, /dev/ttyUSB0)")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate (default: 115200)")
    args = parser.parse_args()

    if args.port:
        run_hardware_monitor(args.port, args.baud)
    else:
        run_simulated_monitor()
