"""
Unit tests for ArigiaGaurd TinyML feature extraction and classification pipeline.
"""

import numpy as np
import pytest
from scripts.feature_inspector import extract_features, generate_posture_windows

def test_feature_extraction_dimensions():
    window = np.random.normal(0, 1.0, (200, 6))
    feats = extract_features(window, fs=100.0)
    
    assert "mean_ax" in feats
    assert "mean_ay" in feats
    assert "mean_az" in feats
    assert "rms_az" in feats
    assert "roll" in feats
    assert "pitch" in feats
    assert "spectral_centroid" in feats
    assert "total_energy" in feats

def test_supine_kinematics():
    # Supine: Az ~ 1.0g, Ax ~ 0g, Ay ~ 0g
    window = np.zeros((200, 6))
    window[:, 2] = 0.98 # Az
    feats = extract_features(window, fs=100.0)
    
    assert abs(feats["roll"]) < 5.0
    assert abs(feats["pitch"]) < 5.0
    assert abs(feats["mean_az"] - 0.98) < 0.05

def test_lateral_30_kinematics():
    # Right lateral 30 deg: Ax ~ 0.50g, Az ~ 0.86g
    window = np.zeros((200, 6))
    window[:, 0] = 0.50 # Ax
    window[:, 2] = 0.86 # Az
    feats = extract_features(window, fs=100.0)
    
    assert abs(feats["mean_ax"] - 0.50) < 0.05
    assert feats["pitch"] < 0 # -arctan2(0.5, 0.86)

def test_softmax_properties():
    # Test softmax normalization across 5 classes
    logits = np.array([2.5, 0.1, -1.2, 0.4, -0.5])
    exp_l = np.exp(logits - np.max(logits))
    probs = exp_l / np.sum(exp_l)

    assert len(probs) == 5
    assert np.sum(probs) == pytest.approx(1.0)
    assert np.all(probs >= 0.0)
    assert np.all(probs <= 1.0)
    assert np.argmax(probs) == 0
