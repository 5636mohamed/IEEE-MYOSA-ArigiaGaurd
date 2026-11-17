"""
DecubiGuard - Contactless Ballistocardiography (BCG) Offline Signal Processing
Separates respiration and micro-acceleration recoil vectors from MPU6050 data.
"""

import numpy as np
from scipy.signal import butter, filtfilt, find_peaks

def design_bandpass_filter(lowcut: float, highcut: float, fs: float, order: int = 4):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

def extract_respiration_rate(signal: np.ndarray, fs: float = 100.0) -> float:
    """Extract respiration rate (0.1 to 0.5 Hz -> 6 to 30 breaths/min)."""
    b, a = design_bandpass_filter(0.1, 0.5, fs, order=3)
    resp_signal = filtfilt(b, a, signal)
    peaks, _ = find_peaks(resp_signal, distance=int(fs * 1.5))
    if len(peaks) > 1:
        avg_period_sec = np.mean(np.diff(peaks)) / fs
        return 60.0 / avg_period_sec
    return 0.0

def extract_heart_rate(signal: np.ndarray, fs: float = 100.0) -> float:
    """Extract BCG heart rate via J-wave peaks (0.8 to 3.5 Hz -> 48 to 210 bpm)."""
    b, a = design_bandpass_filter(0.8, 3.5, fs, order=4)
    bcg_signal = filtfilt(b, a, signal)
    peaks, _ = find_peaks(bcg_signal, distance=int(fs * 0.4), prominence=np.std(bcg_signal) * 0.5)
    if len(peaks) > 1:
        avg_period_sec = np.mean(np.diff(peaks)) / fs
        return 60.0 / avg_period_sec
    return 0.0

if __name__ == "__main__":
    t = np.linspace(0, 30, 3000)
    simulated_resp = 0.2 * np.sin(2 * np.pi * 0.25 * t)  # 15 brpm
    simulated_cardiac = 0.05 * np.sin(2 * np.pi * 1.2 * t) # 72 bpm
    noise = 0.02 * np.random.normal(size=len(t))
    raw_signal = simulated_resp + simulated_cardiac + noise

    rr = extract_respiration_rate(raw_signal)
    hr = extract_heart_rate(raw_signal)
    print(f"Estimated Respiration Rate: {rr:.1f} brpm")
    print(f"Estimated Heart Rate: {hr:.1f} bpm")
