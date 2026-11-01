# LumaGuard — Verification Protocol & Test Report

**Testing Baseline:** Final Integrated Physical Breadboard Bench Prototype  
**Test Location:** S.A. Engineering College, Electronics & Communication Engineering Laboratory  
**Evidence Standard:** Reported Functional Pass/Fail Results (Zero Fabricated Metrics)  
**Status Lifecycle:** Stage 4 (`Hardware bench tested`)

---

## 1. Integrated Verification Matrix (IT-01 to IT-09)

The following verification suite was exercised on the physical hardware prototype using controlled stimulus sources under supervised laboratory conditions:

| Test ID | Test Scenario | Stimulus / Procedure | Expected System Response | Observed / Reported Result | Verification Status |
| :-: | :--- | :--- | :--- | :--- | :-: |
| **IT-01** | **Armed Nominal (No Active Hazard)** | Target at 50 cm (>33 cm), ambient room lighting (no flame). | Green LED ON; Red LED OFF; Buzzer silent; OLED shows `ARMED`, `FLAME: CLEAR`, distance `~50 cm`, `ALERT: SAFE`. | System correctly enters nominal safe state; Green LED illuminates only while echo is fresh. | **PASS** |
| **IT-02** | **Work-Zone Intrusion** | Move target into 20 cm ($\le 30.0\text{ cm}$). | Green LED OFF; Red LED ON; Buzzer pulses at 2.4 kHz with cadence mapped to proximity; OLED displays `ZONE ALERT`. | Boundary crossed; zone danger flag asserts; buzzer period accelerates as target moves closer. | **PASS** |
| **IT-03** | **Persistent Flame Event** | Supervised optical flame source held in front of sensor for $\ge 250\text{ ms}$. | DO switches LOW; AO drops $\le 800$; candidate confirmed after 250 ms hold; Red LED ON; Buzzer sounds; OLED displays `FLAME ALERT`. | Candidate persistence verified; transient flashes (<250ms) rejected as `FLAME CHECK` without alarming. | **PASS** |
| **IT-04** | **Concurrent Multi-Hazard** | Target at 15 cm ($\le 30\text{ cm}$) AND persistent flame source present. | Combined hazard detected; OLED displays `FLAME + ZONE`; buzzer selects shorter (more urgent) of calculated periods. | Multi-hazard arbitration verified; most critical threat dictates audible beep cadence. | **PASS** |
| **IT-05** | **Operator Alert Silence** | Short press ($< 2.0\text{ s}$) on tactile button while hazard alarm is sounding. | Buzzer instantly mutes; Red LED remains ON; OLED displays `ALERT: SILENCED`; continuous monitoring continues. | Alarm silenced without clearing underlying danger flag; new hazards can re-alarm independently. | **PASS** |
| **IT-06** | **Armed / Disarmed Mode Toggle** | Long press ($\ge 2.0\text{ s}$) on tactile button. | System toggles to `DISARMED`; Green/Red LEDs and buzzer forced OFF; OLED displays `DISARMED`; long press again re-arms. | Two-tiered single-button debounced operation verified; physical annunciators suppressed when disarmed. | **PASS** |
| **IT-07** | **Hazard Recovery & Hysteresis** | Move target back from 20 cm to 35 cm (>33 cm) and extinguish flame. | Zone alert holds until distance > 33 cm for 350 ms; flame alert holds until clear inputs persist for 1,200 ms; system returns to `SAFE`. | Hysteresis prevents threshold chatter; separate clear timers prevent rapid flapping. | **PASS** |
| **IT-08** | **Ultrasonic Echo Loss (Stale Data)** | Block acoustic path / aim sensor into free air (> 400 cm). | After 800 ms without valid echo, OLED shows `NO ECHO` and `ZONE UNKNOWN`; Green Safe LED turns OFF; active zone danger holds 1,200 ms before clearing. | Stale data is never presented as measured safe distance; graceful fault degradation verified. | **PASS** |
| **IT-09** | **Non-Volatile Event Logging** | Trigger newly active hazard event, observe Serial event count, power-cycle Nano, re-inspect Serial monitor. | `hazardStarts` increments upon new hazard bit activation; value restored from EEPROM upon reboot. | Persistent EEPROM signature validation and hazard counter survive power removal. | **PASS** |

---

## 2. Evidence Classification & Scientific Limitations

To uphold strict engineering documentation integrity:

> [!NOTE]
> **What This Report Affirms**:
> - The physical prototype was assembled and exercised end-to-end on a test bench.
> - All nine functional integration scenarios (IT-01 through IT-09) successfully operated according to the programmed state machine.
> - The firmware compiles deterministically with 100% pass rate in PlatformIO (`ATmega328P`).

> [!CAUTION]
> **What This Report Does NOT Claim**:
> - **No Metrological Accuracy Claims**: Individual centimetre-level distance accuracy or false-alarm rates per 1,000 hours are omitted because an automated statistical logging campaign was not conducted.
> - **No Life-Safety Certification**: LumaGuard is an educational engineering demonstrator. It is NOT certified under EN 54, UL 268, NFPA 72, or DGMS statutory mining regulations, and must never be used in life-critical environments.
> - **Exclusion of DHT22**: The DHT22 sensor failed data-read reliability tests during early prototyping and was explicitly removed from the hardware and software baseline.

