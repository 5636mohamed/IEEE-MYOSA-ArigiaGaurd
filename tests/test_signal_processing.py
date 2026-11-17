"""
Unit tests for DecubiGuard signal processing modules.
"""

import numpy as np
import pytest
from scripts.bcg_signal_processor import extract_respiration_rate, extract_heart_rate

def test_extract_respiration_rate():
    fs = 100.0
    t = np.linspace(0, 40, int(40 * fs))
    target_rr_bpm = 15.0 # 0.25 Hz
    signal = np.sin(2 * np.pi * (target_rr_bpm / 60.0) * t)
    
    calculated_rr = extract_respiration_rate(signal, fs=fs)
    assert abs(calculated_rr - target_rr_bpm) < 1.0

def test_extract_heart_rate():
    fs = 100.0
    t = np.linspace(0, 30, int(30 * fs))
    target_hr_bpm = 72.0 # 1.2 Hz
    signal = np.sin(2 * np.pi * (target_hr_bpm / 60.0) * t)
    
    calculated_hr = extract_heart_rate(signal, fs=fs)
    assert abs(calculated_hr - target_hr_bpm) < 2.0
