"""
ArigiaGaurd - Multi-Channel Sensor & Telemetry Data Visualizer
Generates multi-panel diagnostic plots for kinematics, BCG vitals, pneumatic alternation, and microclimate.
"""

import sys
import argparse
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

def generate_synthetic_telemetry(duration_sec: float = 60.0, fs: float = 100.0):
    """Generates synthetic multi-modal patient sensor data."""
    t = np.linspace(0, duration_sec, int(duration_sec * fs))
    
    # Respiration (0.25 Hz = 15 brpm)
    resp = 0.04 * np.sin(2 * np.pi * 0.25 * t)
    
    # Cardiac BCG J-waves (1.2 Hz = 72 bpm with characteristic rapid systolic recoil)
    cardiac = 0.015 * np.sin(2 * np.pi * 1.2 * t) + 0.008 * np.sin(4 * np.pi * 1.2 * t)
    
    # Baseline gravity + noise
    noise = np.random.normal(0, 0.003, len(t))
    az = 0.98 + resp + cardiac + noise
    ax = 0.02 + np.random.normal(0, 0.002, len(t))
    ay = -0.01 + np.random.normal(0, 0.002, len(t))

    # Add a 30-degree lateral turn at t = 35 s
    turn_mask = t >= 35.0
    ax[turn_mask] = 0.50 + np.random.normal(0, 0.005, np.sum(turn_mask))
    az[turn_mask] = 0.86 + resp[turn_mask] + cardiac[turn_mask] + noise[turn_mask]

    # Pneumatic air-cell pressures (Bank A vs Bank B alternation)
    t_sec = np.linspace(0, duration_sec, int(duration_sec))
    pressure_bank_a = 32.0 + 2.0 * np.sin(2 * np.pi * 0.02 * t_sec)
    pressure_bank_b = 18.0 - 2.0 * np.sin(2 * np.pi * 0.02 * t_sec)

    # Microclimate
    humidity = 48.0 + 12.0 * (t_sec / duration_sec) ** 2  # gradual humidity buildup
    temperature = 34.2 + 1.2 * (t_sec / duration_sec)

    return {
        "t": t, "t_sec": t_sec,
        "ax": ax, "ay": ay, "az": az,
        "cardiac": cardiac, "resp": resp,
        "bank_a": pressure_bank_a, "bank_b": pressure_bank_b,
        "humidity": humidity, "temperature": temperature
    }

def plot_telemetry(data, output_file: str = "docs/telemetry_plot.png"):
    """Creates a high-resolution 4-panel telemetry figure."""
    fig, axes = plt.subplots(4, 1, figsize=(10, 8), sharex=False)
    plt.subplots_adjust(hspace=0.4)

    # Panel 1: Kinematic Accel Axes & Repositioning
    ax1 = axes[0]
    ax1.plot(data["t"], data["az"], color="#1f77b4", label="Accel Z (Vertical / Recoil)")
    ax1.plot(data["t"], data["ax"], color="#ff7f0e", label="Accel X (Lateral Tilt)")
    ax1.axvline(35.0, color="gray", linestyle="--", label="Lateral Turn (30°)")
    ax1.set_ylabel("Accel (g)")
    ax1.set_title("ArigiaGaurd Multi-Modal Sensor Telemetry: Kinematics & Posture", fontsize=11, fontweight="bold")
    ax1.legend(loc="upper right", fontsize=8)
    ax1.grid(True, linestyle=":", alpha=0.6)

    # Panel 2: Contactless BCG Cardiac & Respiratory Excursions
    ax2 = axes[1]
    window = (data["t"] >= 10.0) & (data["t"] <= 25.0)
    ax2.plot(data["t"][window], data["resp"][window], color="#2ca02c", label="Extracted Respiration (15 brpm)")
    ax2.plot(data["t"][window], data["cardiac"][window], color="#d62728", alpha=0.8, label="BCG Cardiac J-Waves (72 bpm)")
    ax2.set_ylabel("Micro-g")
    ax2.set_title("Contactless Ballistocardiography (BCG) Decomposition", fontsize=11, fontweight="bold")
    ax2.legend(loc="upper right", fontsize=8)
    ax2.grid(True, linestyle=":", alpha=0.6)

    # Panel 3: Pneumatic Alternating Pressure Cycles
    ax3 = axes[2]
    ax3.plot(data["t_sec"], data["bank_a"], color="#9467bd", linewidth=1.8, label="Chamber Bank A Pressure")
    ax3.plot(data["t_sec"], data["bank_b"], color="#8c564b", linewidth=1.8, linestyle="--", label="Chamber Bank B Pressure")
    ax3.axhline(32.0, color="red", linestyle=":", label="Capillary Threshold (32 mmHg)")
    ax3.set_ylabel("Pressure (mmHg)")
    ax3.set_title("Closed-Loop Pneumatic Pressure Redistribution (Alternating APAM)", fontsize=11, fontweight="bold")
    ax3.legend(loc="upper right", fontsize=8)
    ax3.grid(True, linestyle=":", alpha=0.6)

    # Panel 4: Sacral Microclimate Surveillance
    ax4 = axes[3]
    ax4_temp = ax4.twinx()
    l1 = ax4.plot(data["t_sec"], data["humidity"], color="#17becf", linewidth=1.8, label="Sacral Humidity (% RH)")
    l2 = ax4_temp.plot(data["t_sec"], data["temperature"], color="#e377c2", linewidth=1.8, linestyle="-.", label="Interface Temp (°C)")
    ax4.axhline(65.0, color="orange", linestyle=":", label="Maceration Threshold (65% RH)")
    ax4.set_xlabel("Time (seconds)")
    ax4.set_ylabel("Humidity (% RH)", color="#17becf")
    ax4_temp.set_ylabel("Temp (°C)", color="#e377c2")
    ax4.set_title("Sacral Microclimate Surveillance (Maceration Prevention)", fontsize=11, fontweight="bold")
    lines = l1 + l2
    labels = [l.get_label() for l in lines]
    ax4.legend(lines, labels, loc="upper left", fontsize=8)
    ax4.grid(True, linestyle=":", alpha=0.6)

    plt.tight_layout()
    plt.savefig(output_file, dpi=200)
    print(f"[SUCCESS] Multi-channel telemetry plot saved to {output_file}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="ArigiaGaurd Data Visualizer")
    parser.add_argument("--output", type=str, default="docs/telemetry_plot.png", help="Path to save output figure")
    parser.add_argument("--duration", type=float, default=60.0, help="Duration of simulation in seconds")
    args = parser.parse_args()

    data = generate_synthetic_telemetry(duration_sec=args.duration)
    plot_telemetry(data, output_file=args.output)
