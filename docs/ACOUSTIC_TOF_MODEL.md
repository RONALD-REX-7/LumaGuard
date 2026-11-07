# Ultrasonic Time-of-Flight Acoustic Propagation Model

**Specification Version:** 1.0.0  
**Target Hardware:** Arduino Nano + HC-SR04 Ultrasonic Transducer  
**Application:** LumaGuard Autonomous Obstacle Detection & Ranging

---

## 1. Temperature-Compensated Speed of Sound

Acoustic wave propagation in dry air varies non-linearly with thermodynamic temperature $T_K$ (Kelvin):

$$c(T) = \sqrt{\gamma \cdot R_{\text{specific}} \cdot T_K}$$

Where:
- Adiabatic index for air: $\gamma = 1.400$
- Specific gas constant for dry air: $R_{\text{specific}} = 287.058\,\text{J}/(\text{kg}\cdot\text{K})$

Linear Taylor approximation around $0^\circ\text{C}$ ($273.15\,\text{K}$) for Celsius temperature $T_C$:

$$c(T_C) \approx 331.3 \cdot \sqrt{1 + \frac{T_C}{273.15}} \approx 331.3 + 0.606 \cdot T_C \quad (\text{m/s})$$

### Speed of Sound Reference Table

| Ambient Temperature ($T_C$) | Speed of Sound ($c$) | Round-Trip Propagation Delay per Centimeter |
| :---: | :---: | :---: |
| $0^\circ\text{C}$ | $331.3\,\text{m/s}$ | $60.37\,\mu\text{s/cm}$ |
| $15^\circ\text{C}$ | $340.4\,\text{m/s}$ | $58.75\,\mu\text{s/cm}$ |
| $20^\circ\text{C}$ | $343.4\,\text{m/s}$ | $58.24\,\mu\text{s/cm}$ |
| $25^\circ\text{C}$ | $346.5\,\text{m/s}$ | $57.72\,\mu\text{s/cm}$ |
| $35^\circ\text{C}$ | $352.5\,\text{m/s}$ | $56.74\,\mu\text{s/cm}$ |

---

## 2. Microcontroller Ranging Formulation

The ATmega328P 16-bit Timer1 captures the high pulse duration $\Delta t$ ($\mu\text{s}$) emitted on the HC-SR04 `ECHO` pin:

$$d = \frac{c(T_C) \cdot \Delta t \times 10^{-4}}{2} \quad (\text{cm})$$

At nominal room temperature ($20^\circ\text{C}$, $c = 343.4\,\text{m/s}$):

$$d = \frac{\Delta t}{58.24} \approx \frac{\Delta t \times 70}{4077} \quad (\text{integer fixed-point arithmetic})$$
