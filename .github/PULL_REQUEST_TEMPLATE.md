## Description
<!-- Brief 1-2 sentence description of the change and its engineering purpose -->

## Technical Scope
- [ ] Microcontroller firmware (ATmega328P / PlatformIO)
- [ ] Sensor interfacing (HC-SR04, IR Flame, LM393)
- [ ] Actuator annunciators (PWM Piezo Buzzer, RGB status LEDs)
- [ ] I2C bus & SSD1306 OLED display driver
- [ ] Hardware schematics, pinout tables & electrical documentation

## Seven-Stage Project Status Lifecycle Alignment
- [ ] Stage 1: Planned (Concept or pinout mapping)
- [ ] Stage 2: Implemented in software (Firmware written and compiles)
- [ ] Stage 3: Software tested (Host unit test / simulator verification)
- [ ] Stage 4: Hardware bench tested (Flashed to isolated Arduino Nano on bench)
- [ ] Stage 5: Hardware integrated (Connected to sensors and display)
- [ ] Stage 6: Integrated prototype tested (Laboratory workspace evaluated)

## Quality & Evidence Checklist
- [ ] **Firmware Compilation**: Builds cleanly with PlatformIO (`pio run -e nanoatmega328`).
- [ ] **Zero Credential Exposure**: Confirmed no secrets, tokens, or private details exist in this PR.
- [ ] **Zero Fabrication**: Physical measurements are strictly distinguished from simulated or theoretical calculations.
- [ ] **Electrical Safety**: Voltage domain compatibility (5V DC single rail) and current limits verified.
