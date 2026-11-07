# LumaGuard — Sensor Calibration & Electrical Tuning Guide

**Target Hardware:** Arduino Nano (ATmega328P, 5V, 16 MHz)  
**Applicability:** Physical Breadboard Bench Prototype & Hardware Integration  
**Status Lifecycle:** Stage 4 (`Hardware bench tested`)

---

## 1. Ultrasonic Proximity Sensor Calibration (HC-SR04)

### 1.1 Acoustic Propagation Model
The HC-SR04 sensor determines obstacle distance using round-trip time-of-flight ($\Delta t$):

$$\text{Distance} = \frac{\Delta t \times c}{2}$$

Where the acoustic velocity in air $c$ is temperature-dependent:

$$c(T) = 331.3 + 0.606 \times T \quad (\text{m/s})$$

At nominal bench ambient temperature ($25^\circ\text{C}$), sound speed is $346.45\text{ m/s}$ ($0.034645\text{ cm/\mu s}$). The microcontroller measures pulse duration in microseconds:

$$\text{Distance (cm)} = \frac{\text{Echo Pulse Duration (\mu s)}}{57.7}$$

### 1.2 Boundary Hysteresis & Debouncing
To prevent threshold oscillation (chatter) at the work-zone boundary:
- **Intrusion Trigger Threshold**: $\le 30.0\text{ cm}$
- **Recovery Clear Threshold**: $\ge 33.0\text{ cm}$ held for $\ge 350\text{ ms}$
- **Echo Timeout**: $25\text{ ms}$ ($430\text{ cm}$ physical limit). Echoes exceeding this boundary trigger an echo loss fault state (`NO ECHO`).

---

## 2. Infrared Flame Sensor Calibration

### 2.1 Optical & Analog Tuning
The LM393 comparator module provides dual outputs:
1. **Digital Comparator Pin (`DO` $\to$ D9)**:
   - Trip point adjusted via the on-board 10 kΩ multi-turn potentiometer.
   - Adjust potentiometer until the digital indicator LED just turns OFF under ambient room illumination.
   - Test using a controlled flame stimulus at $50\text{ cm}$ distance to verify reliable switching to `LOW`.
2. **Analog Voltage Pin (`AO` $\to$ A0)**:
   - Sampled by the ATmega328P 10-bit successive approximation ADC ($V_{\text{REF}} = 5.0\text{ V}$).
   - **Nominal Ambient Reading**: $950 - 1023$ counts.
   - **Hazard Trigger Threshold**: $\le 800$ counts ($\le 3.91\text{ V}$).
   - **Recovery Clear Threshold**: $\ge 850$ counts ($\ge 4.15\text{ V}$) held continuously for $\ge 1,200\text{ ms}$.

### 2.2 Flame Candidate Persistence Filter
To prevent false alarms caused by fluorescent lighting flicker, moving shadows, or transient optical noise:
- The firmware enters a `FLAME CHECK` state upon initial detection.
- The hazard condition must persist continuously for $\ge 250\text{ ms}$ before transitioning to full `FLAME ALERT`.

---

## 3. Piezo Buzzer & Auditory Cadence

- **Resonant Frequency**: $2,400\text{ Hz}$ (square wave generated via ATmega328P hardware timer / `tone()`).
- **Dynamic Cadence Mapping**:
  - Distance $30\text{ cm}$: $600\text{ ms}$ period ($300\text{ ms}$ ON, $300\text{ ms}$ OFF).
  - Distance $10\text{ cm}$: $200\text{ ms}$ period ($100\text{ ms}$ ON, $100\text{ ms}$ OFF).
  - Distance $< 5\text{ cm}$: Continuous warning tone.

---

## 4. EEPROM Non-Volatile Memory Map

The ATmega328P incorporates 1,024 bytes of on-chip EEPROM. LumaGuard preserves the lifetime hazard event counter across power cycles:

| Byte Offset | Data Type | Field | Description |
| :---: | :---: | :--- | :--- |
| `0x00 - 0x01` | `uint16_t` | `Magic Signature` | `0x4C47` (`'LG'`). Validates initialised memory state. |
| `0x02 - 0x03` | `uint16_t` | `Hazard Starts` | Total count of unique hazard activations recorded. |

*Wear-Leveling Note*: The event counter is written to EEPROM only upon transition into an active hazard state (not periodically), preserving EEPROM cell endurance ($100,000$ write cycles minimum).
