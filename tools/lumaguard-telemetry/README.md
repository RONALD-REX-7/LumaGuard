# LumaGuard Telemetry

**A small, dependency-free Python tool that converts LumaGuard firmware serial diagnostics into validated JSON Lines (JSONL).**

It is designed for bench logs, debugging sessions, regression fixtures, and downstream scripts. It parses the status snapshots and event messages emitted by the LumaGuard firmware; it does not connect to hardware, change firmware behaviour, or claim to be a certified safety system.

## What it reads

The parser recognises the exact status line emitted by `printSerialStatus()` in the parent LumaGuard firmware:

```text
Distance_cm=22.4 | Flame_DO=1 | Flame_AO=1023 | Flame=CLEAR | Mode=ARMED | ActiveMask=1 | SilencedMask=0 | Alert=ZONE ALERT | HazardStarts=1
```

It also recognises the firmware's named hazard events, system arm/disarm messages, hazard-start counter, and OLED initialisation failure. Unrelated boot text is ignored. A malformed line that resembles a supported record is reported instead of silently converted.

## Install from this repository

Python 3.10 or newer is required. No runtime third-party dependencies are used.

```bash
python -m pip install .
```

For development and tests:

```bash
python -m pip install -e ".[test]"
python -m pytest -q
```

## Convert a saved serial log

```bash
lumaguard-telemetry convert examples/lumaguard-session.log -o session.jsonl
```

Read from standard input and write JSONL to standard output:

```bash
cat examples/lumaguard-session.log | lumaguard-telemetry convert
```

Use `--strict` to return exit code `2` when a malformed telemetry record is encountered. Malformed records are always reported to standard error; unrelated lines are skipped.

## Python API

```python
from lumaguard_telemetry import parse_line, parse_status_line

status = parse_status_line(
    "Distance_cm=22.4 | Flame_DO=1 | Flame_AO=1023 | Flame=CLEAR | "
    "Mode=ARMED | ActiveMask=1 | SilencedMask=0 | Alert=ZONE ALERT | HazardStarts=1"
)
print(status.to_dict())

record = parse_line("EVENT: ZONE ALERT STARTED")
print(record.to_dict())
```

`NO_ECHO` is represented as `distance_cm: null`, not as a fabricated zero reading. Distance and ADC values are range-checked; masks, states, and alert labels are validated against the current firmware output contract.

The JSON Schema for emitted status and event objects is included at `src/lumaguard_telemetry/schemas/telemetry-record.schema.json`.

## OCI container (GitHub Container Registry)

The multi-architecture container is published as:

```text
ghcr.io/ronald-rex-7/lumaguard-telemetry:1.0.0
```

After the package is made public in GitHub Packages, run it with a saved log:

```bash
docker run --rm \
  -v "$PWD/examples:/input:ro" \
  ghcr.io/ronald-rex-7/lumaguard-telemetry:1.0.0 \
  convert /input/lumaguard-session.log
```

Or stream a live capture into it:

```bash
cat capture.log | docker run --rm -i \
  ghcr.io/ronald-rex-7/lumaguard-telemetry:1.0.0 convert
```

The image supports `linux/amd64` and `linux/arm64`. Package publication is triggered by changes to this package on `main` or the workflow's manual dispatch input. Pull requests run verification but do not publish images.

## Scope and limitations

- The parser intentionally follows the current LumaGuard serial text format. If firmware field names or event wording change, update the parser, schema, examples, and tests together.
- This tool does not infer missing timestamps, interpolate invalid sensor readings, or treat `NO_ECHO` as a safe distance.
- It is a development/observability utility, not a safety-certified monitoring product.

## License

MIT. See [LICENSE](LICENSE).
