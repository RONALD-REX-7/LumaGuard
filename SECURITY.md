# Security Policy

## Supported Versions

| Version | Supported | Notes |
| :--- | :---: | :--- |
| `v1.0.x` (Main) | Yes | Active academic prototype firmware |

---

## Reporting a Vulnerability

As an educational embedded prototype running on an isolated 8-bit microcontroller (Arduino Nano ATmega328P) without networking interfaces, traditional network vulnerabilities are out of scope. However, we take firmware reliability, timing safety, and hardware damage prevention seriously.

If you discover:
- Race conditions or timer rollover bugs in `millis()` comparisons.
- Pin configuration conflicts that could cause electrical damage (e.g. GPIO contention).
- Buffer overflows or unbounded stack growth risks.

Please report them by opening a GitHub Issue with the label `security` or contacting the repository maintainer directly.
