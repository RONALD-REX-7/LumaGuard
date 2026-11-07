# Optical Infrared Flame Sensor Electrical Thresholding & Hysteresis Model

**Specification Version:** 1.0.0  
**Target Hardware:** Arduino Nano + LM393 Optical Flame Detector Module  
**Application:** LumaGuard Fail-Safe Hazard Protection Subsystem

---

## 1. Photodiode Spectral Response & Signal Conditioning

The LumaGuard optical flame sensor employs an NPN silicon photodiode sensitive to infrared wavelengths emitted by hydrocarbon combustion flames:

$$\text{Active Spectral Bandwidth: } 760\,\text{nm} \le \lambda \le 1100\,\text{nm}$$

$$\text{Peak Spectral Sensitivity: } \lambda_{\text{peak}} \approx 940\,\text{nm}$$

The photodiode reverse saturation leakage current $I_{\text{photo}}$ increases proportionally with radiant flux $\Phi_e$ ($\mu\text{W}/\text{cm}^2$):

$$I_{\text{photo}} = R_\lambda \cdot \Phi_e$$

Where $R_\lambda \approx 0.55\,\text{A/W}$ is the spectral responsivity at $940\,\text{nm}$.

---

## 2. Schmitt Trigger Hysteresis Model

To prevent rapid oscillation (electrical chatter) when flame radiation hovers near the comparator decision threshold, the LM393 dual differential comparator incorporates positive feedback hysteresis:

$$V_{\text{high}} = 2.80\,\text{V} \quad (\text{Flame Assert Threshold})$$
$$V_{\text{low}} = 2.20\,\text{V} \quad (\text{Flame De-assert Threshold})$$
$$V_{\text{hys}} = V_{\text{high}} - V_{\text{low}} = 0.60\,\text{V}$$

In software, temporal candidate persistence confirms flame existence only when $V_{\text{sensor}} \le V_{\text{low}}$ continuously for $N \ge 3$ consecutive $50\,\text{ms}$ control cycles, completely immunizing the system against transient incandescent flicker.
