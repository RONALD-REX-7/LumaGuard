---
name: Bug report
about: Report a firmware defect or sensor communication issue
title: "[BUG] "
labels: bug
assignees: ''
---

**Describe the bug**
A clear description of the defect observed in firmware execution or sensor reading.

**Target Hardware & Environment**
- Microcontroller: Arduino Nano (ATmega328P, 5V, 16 MHz)
- Build System: PlatformIO Core
- Framework: Arduino Core (AVR)

**To Reproduce**
Steps to reproduce the behavior:
1. Flash firmware with `pio run -e nanoatmega328 -t upload`
2. Apply sensor stimulus
3. Observe serial monitor or OLED output

**Expected behavior**
Expected state machine transition or alert annunciator state.

**Serial Logs / Evidence**
Attach serial monitor output (115200 baud).
