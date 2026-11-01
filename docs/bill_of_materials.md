# LumaGuard — Bill of Materials (BOM) & Technical Ratings

**Status:** Physical Breadboard Hardware Prototype Verified  
**Fabrication Cost:** Reused Academic Lab Inventory (Mandatory Additional Cost: ₹0 / $0.00 USD)

---

## 1. Itemized Component Specification

| Item # | Component Name | Description / Specification | Package / Form Factor | Qty | Interface | Nominal Supply | Operating Current |
| :-: | :--- | :--- | :--- | :-: | :--- | :---: | :-: |
| 1 | **Arduino Nano** | Microchip ATmega328P, 8-bit AVR, 16 MHz, 32 KB Flash, 2 KB SRAM, 1 KB EEPROM | 30-pin DIP Module | 1 | UART / I2C / SPI / GPIO | 5.0 V DC | ~25 mA |
| 2 | **HC-SR04** | Ultrasonic Distance Transceiver (40 kHz transducer pair, 2 cm to 400 cm range) | 4-pin Breakout | 1 | Digital Trigger (10 µs) / Echo | 5.0 V DC | 15 mA (65 mA peak pulse) |
| 3 | **IR Flame Sensor** | Phototransistor/photodiode with LM393 comparator and trimmer potentiometer (760–1100 nm spectral response) | 4-pin Breakout | 1 | Digital DO (Active LOW) / Analog AO | 3.3–5.0 V DC | ~15 mA |
| 4 | **SSD1306 OLED** | 0.96" Monochrome Graphic OLED Display (128 × 64 pixels, White/Blue emissive) | 4-pin I2C Breakout | 1 | Hardware I2C (Address: `0x3C`) | 3.3–5.0 V DC | ~20 mA typical |
| 5 | **Status Green LED** | 5 mm Diffused Green LED (Safe status annunciator) | Through-hole T-1 3/4 | 1 | GPIO `D4` (Active HIGH) | 2.0–2.2 V forward | 10–15 mA (via 220Ω) |
| 6 | **Status Red LED** | 5 mm Diffused Red LED (Hazard alert annunciator) | Through-hole T-1 3/4 | 1 | GPIO `D5` (Active HIGH) | 1.8–2.0 V forward | 10–15 mA (via 220Ω) |
| 7 | **Piezo Buzzer** | Continuous / Passive Piezoelectric Transducer | Through-hole Cylinder | 1 | PWM / Tone (`D6` via 1 kΩ) | 3.0–5.0 V DC | < 5 mA |
| 8 | **Tactile Pushbutton** | 6 mm Momentary Tactile Switch | 4-pin SPST Through-hole | 1 | GPIO `D7` (`INPUT_PULLUP`) | Internal Pull-up | < 0.2 mA |
| 9 | **Resistors** | 220 Ω ±5% 0.25W (LEDs) + 1 kΩ ±5% 0.25W (Buzzer) | Carbon Film Through-hole | 3 | Passive current limiter | — | — |
| 10 | **Solderless Breadboard** | 830-point Full-Size Solderless Prototyping Breadboard | ABS Plastic with power rails | 1 | Mechanical assembly | — | — |
| 11 | **Jumper Wires** | Premium 24 AWG Male-to-Male Dupont flexible jumpers | Multi-color bundle | ~25 | Interconnects | — | — |

---

## 2. Power Consumption Budget

| Operating State | Active Components | Estimated Current Draw (5V) | Total Power Dissipation |
| :--- | :--- | :---: | :---: |
| **Nominal (Safe, Fresh Distance)** | Nano (25mA) + Green LED (12mA) + OLED (20mA) + HC-SR04 idle (15mA) + Flame (15mA) | **~87 mA** | **~435 mW** |
| **Hazard Alert (Distance / Flame)** | Nano (25mA) + Red LED (15mA) + OLED (25mA) + Buzzer pulse (5mA) + Sensors (30mA) | **~100 mA** | **~500 mW** |
| **Disarmed Mode** | Nano (25mA) + OLED (15mA) + Sensors active (30mA) + LEDs/Buzzer OFF (0mA) | **~70 mA** | **~350 mW** |

*Power Conclusion:* The prototype operates well within standard USB 2.0 host power delivery limits (500 mA maximum at 5.0 V).
