# Contributing to LumaGuard

Thank you for your interest in contributing to **LumaGuard**!

LumaGuard is an academic and educational embedded systems prototype developed for micro-workspace safety monitoring. We welcome improvements, bug reports, and hardware documentation enhancements that adhere to strict engineering integrity.

---

## Code of Conduct & Engineering Principles

1. **Zero Fabrication**: Never submit simulated sensor telemetry or claim physical execution on hardware that has not been physically validated.
2. **Defensible Documentation**: Distinguish clearly between software simulations, bench tests, and future improvements.
3. **Embedded Awareness**: Maintain memory bounds suitable for 8-bit AVR microcontrollers (ATmega328P has only 2,048 bytes of SRAM and 32 KB of Flash). Avoid dynamic heap allocations (`malloc`, `String` objects) in hot execution paths.

---

## Development & Verification Workflow

### 1. Build Verification via PlatformIO
Before opening a pull request, verify that the firmware compiles with zero errors and warnings:
```bash
pio run -e nanoatmega328
```

### 2. Style and Conventions
- Follow standard embedded C++ conventions.
- Keep timing constants centralized near the top of the sketch or in dedicated configuration headers.
- Use explicit non-blocking timers (`millis()`) rather than blocking delays where possible.

---

## Reporting Issues
When reporting an issue, please include:
- Arduino IDE / PlatformIO Core version.
- Exact microcontroller variant (Classic Nano ATmega328P vs new Nano Every / Nano 33).
- Pinout and supply voltage details.
- Steps to reproduce with Serial Monitor output at 9,600 baud.
