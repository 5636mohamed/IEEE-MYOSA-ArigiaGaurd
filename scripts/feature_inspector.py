"""
ArigiaGaurd - TinyML Feature Inspector & Distribution Visualizer
Extracts and compares time- and frequency-domain kinematic features across the 5 sleep postures.
"""

import sys
import argparse
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

CLASSES = ["supine", "right_lateral_30", "left_lateral_30", "fowler", "prone"]

def generate_posture_windows(n_windows_per_class: int = 50, window_len: int = 200):
    """Generates synthetic 6-axis IMU windows for each posture class."""
    dataset = {}
    for c in CLASSES:
        windows = []
        for _ in range(n_windows_per_class):
            if c == "supine":
                ax = np.random.normal(0.0, 0.05, window_len)
                ay = np.random.normal(0.0, 0.05, window_len)
                az = np.random.normal(0.98, 0.04, window_len)
            elif c == "right_lateral_30":
                ax = np.random.normal(0.50, 0.05, window_len)
                ay = np.random.normal(0.0, 0.05, window_len)
                az = np.random.normal(0.86, 0.04, window_len)
            elif c == "left_lateral_30":
                ax = np.random.normal(-0.50, 0.05, window_len)
                ay = np.random.normal(0.0, 0.05, window_len)
                az = np.random.normal(0.86, 0.04, window_len)
            elif c == "fowler":
                ax = np.random.normal(0.0, 0.05, window_len)
                ay = np.random.normal(0.58, 0.06, window_len)
                az = np.random.normal(0.80, 0.05, window_len)
            elif c == "prone":
                ax = np.random.normal(0.0, 0.05, window_len)
                ay = np.random.normal(0.0, 0.05, window_len)
                az = np.random.normal(-0.95, 0.05, window_len)
            
            # Gyroscope noise
            gx = np.random.normal(0.0, 0.5, window_len)
            gy = np.random.normal(0.0, 0.5, window_len)
            gz = np.random.normal(0.0, 0.5, window_len)
            windows.append(np.stack([ax, ay, az, gx, gy, gz], axis=-1))
        dataset[c] = np.array(windows)
    return dataset

def extract_features(window: np.ndarray, fs: float = 100.0) -> dict:
    """Computes time and frequency domain features for a single 6-axis window."""
    ax, ay, az = window[:, 0], window[:, 1], window[:, 2]
    
    # Time domain
    rms_ax = np.sqrt(np.mean(ax**2))
    rms_az = np.sqrt(np.mean(az**2))
    mean_ax = np.mean(ax)
    mean_ay = np.mean(ay)
    mean_az = np.mean(az)

    # Roll and pitch angles
    roll = np.arctan2(mean_ay, mean_az) * 180.0 / np.pi
    pitch = np.arctan2(-mean_ax, np.sqrt(mean_ay**2 + mean_az**2)) * 180.0 / np.pi

    # Frequency domain (FFT on Az)
    fft_vals = np.abs(np.fft.rfft(az - mean_az))
    freqs = np.fft.rfftfreq(len(az), 1.0 / fs)
    total_energy = np.sum(fft_vals**2)
    spectral_centroid = np.sum(freqs * fft_vals) / (np.sum(fft_vals) + 1e-8)

    return {
        "mean_ax": mean_ax,
        "mean_ay": mean_ay,
        "mean_az": mean_az,
        "rms_az": rms_az,
        "roll": roll,
        "pitch": pitch,
        "spectral_centroid": spectral_centroid,
        "total_energy": total_energy
    }

def inspect_dataset(output_file: str = "results/feature_distribution.png"):
    """Extracts features across classes and generates distribution box plots."""
    dataset = generate_posture_windows()
    
    roll_per_class = []
    pitch_per_class = []
    az_per_class = []

    print("\n=== ArigiaGaurd Feature Inspection Summary ===")
    print(f"{'Class':<20} | {'Mean Roll (°)':<15} | {'Mean Pitch (°)':<15} | {'Mean Az (g)':<12}")
    print("-" * 70)

    for c in CLASSES:
        rolls = []
        pitches = []
        azs = []
        for win in dataset[c]:
            feats = extract_features(win)
            rolls.append(feats["roll"])
            pitches.append(feats["pitch"])
            azs.append(feats["mean_az"])
        
        roll_per_class.append(rolls)
        pitch_per_class.append(pitches)
        az_per_class.append(azs)

        print(f"{c:<20} | {np.mean(rolls):>13.1f} ± {np.std(rolls):<4.1f} | {np.mean(pitches):>13.1f} ± {np.std(pitches):<4.1f} | {np.mean(azs):>10.2f} ± {np.std(azs):<4.2f}")

    # Plot box plots
    fig, axes = plt.subplots(1, 3, figsize=(12, 4))
    
    axes[0].boxplot(roll_per_class, tick_labels=CLASSES)
    axes[0].set_title("Roll Angle Distribution (°)", fontweight="bold")
    axes[0].tick_params(axis='x', rotation=30)
    axes[0].grid(True, linestyle=":", alpha=0.6)

    axes[1].boxplot(pitch_per_class, tick_labels=CLASSES)
    axes[1].set_title("Pitch Angle Distribution (°)", fontweight="bold")
    axes[1].tick_params(axis='x', rotation=30)
    axes[1].grid(True, linestyle=":", alpha=0.6)

    axes[2].boxplot(az_per_class, tick_labels=CLASSES)
    axes[2].set_title("Vertical Accel Az (g)", fontweight="bold")
    axes[2].tick_params(axis='x', rotation=30)
    axes[2].grid(True, linestyle=":", alpha=0.6)

    plt.tight_layout()
    plt.savefig(output_file, dpi=200)
    print(f"\n[SUCCESS] Feature distributions plotted to {output_file}\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="ArigiaGaurd Feature Inspector")
    parser.add_argument("--output", type=str, default="results/feature_distribution.png", help="Path to save distribution plot")
    args = parser.parse_args()

    inspect_dataset(output_file=args.output)
