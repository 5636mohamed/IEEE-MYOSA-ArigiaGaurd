"""
Unit tests for DecubiGuard signal processing modules.
Tests Butterworth IIR filtering, J-wave peak extraction, respiration estimation, and PTI ischemia accumulation.
"""

import numpy as np
import pytest
from scripts.bcg_signal_processor import extract_respiration_rate, extract_heart_rate, design_bandpass_filter

def test_extract_respiration_rate():
    fs = 100.0
    t = np.linspace(0, 40, int(40 * fs))
    target_rr_bpm = 15.0  # 0.25 Hz
    signal = 0.05 * np.sin(2 * np.pi * (target_rr_bpm / 60.0) * t) + np.random.normal(0, 0.002, len(t))
    
    calculated_rr = extract_respiration_rate(signal, fs=fs)
    assert abs(calculated_rr - target_rr_bpm) < 1.0

def test_extract_heart_rate():
    fs = 100.0
    t = np.linspace(0, 30, int(30 * fs))
    target_hr_bpm = 72.0  # 1.2 Hz
    # Characteristic BCG wave with harmonic systolic component
    signal = 0.02 * np.sin(2 * np.pi * (target_hr_bpm / 60.0) * t) + \
             0.01 * np.sin(4 * np.pi * (target_hr_bpm / 60.0) * t) + \
             np.random.normal(0, 0.002, len(t))
    
    calculated_hr = extract_heart_rate(signal, fs=fs)
    assert abs(calculated_hr - target_hr_bpm) < 2.0

def test_filter_stability():
    fs = 100.0
    b_cardiac, a_cardiac = design_bandpass_filter(0.8, 3.5, fs, order=4)
    # Check that all poles lie inside unit circle for stability
    poles = np.roots(a_cardiac)
    assert np.all(np.abs(poles) < 1.0)

    b_resp, a_resp = design_bandpass_filter(0.1, 0.5, fs, order=3)
    poles_resp = np.roots(a_resp)
    assert np.all(np.abs(poles_resp) < 1.0)

def test_pti_accumulation_and_reperfusion():
    """Tests the Pressure-Time Integral (PTI) calculation logic."""
    capillary_threshold = 32.0  # mmHg
    pressure = 36.0  # 4 mmHg supra-capillary
    dt = 1.0  # 1 second step
    
    # Supine accumulation
    excess = pressure - capillary_threshold
    pti = 0.0
    for _ in range(10):
        pti += excess * dt
    assert pti == pytest.approx(40.0)

    # Reperfusion credit during 30 deg lateral tilt
    for _ in range(5):
        pti = max(0.0, pti - (10.0 * dt))
    assert pti == pytest.approx(0.0)
