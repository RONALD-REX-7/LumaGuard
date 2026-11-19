"""Strict parser for the serial diagnostics emitted by LumaGuard firmware."""

from __future__ import annotations

import math
import re
from typing import TypeAlias

from .errors import TelemetryParseError
from .models import EventRecord, StatusRecord

Record: TypeAlias = StatusRecord | EventRecord | None

_STATUS_KEYS = {
    "Distance_cm",
    "Flame_DO",
    "Flame_AO",
    "Flame",
    "Mode",
    "ActiveMask",
    "SilencedMask",
    "Alert",
    "HazardStarts",
}
_FLAME_STATES = {"DETECTED", "CHECK", "CLEAR"}
_MODES = {"ARMED", "DISARMED"}
_ALERTS = {
    "DISARMED",
    "FLAME + ZONE",
    "FLAME ALERT",
    "ZONE ALERT",
    "SILENCED",
    "FLAME CHECK",
    "ZONE UNKNOWN",
    "SAFE",
}
_EVENT_NAMES = {
    "ZONE ALERT STARTED": "ZONE_ALERT_STARTED",
    "ZONE ALERT CLEARED": "ZONE_ALERT_CLEARED",
    "DISTANCE ECHO LOST": "DISTANCE_ECHO_LOST",
    "FLAME CONFIRMED": "FLAME_CONFIRMED",
    "FLAME CONDITION CLEARED": "FLAME_CONDITION_CLEARED",
}
_COUNT_RE = re.compile(r"^HAZARD START COUNT:\s*(\d+)$")


def _integer(fields: dict[str, str], key: str, minimum: int, maximum: int) -> int:
    raw = fields[key]
    if not re.fullmatch(r"\d+", raw):
        raise TelemetryParseError(f"{key} must be an integer; received {raw!r}")
    value = int(raw)
    if not minimum <= value <= maximum:
        raise TelemetryParseError(
            f"{key} must be between {minimum} and {maximum}; received {value}"
        )
    return value


def parse_status_line(line: str) -> StatusRecord:
    """Parse one `Distance_cm=...` status line.

    The parser requires the exact set of fields produced by `printSerialStatus`
    in the LumaGuard firmware. `NO_ECHO` maps to `distance_cm=None`.
    """
    fields: dict[str, str] = {}
    for part in line.strip().split("|"):
        if "=" not in part:
            raise TelemetryParseError(f"Malformed field: {part.strip()!r}")
        key, value = (piece.strip() for piece in part.split("=", 1))
        if not key or key in fields:
            raise TelemetryParseError(f"Duplicate or empty field name: {key!r}")
        fields[key] = value

    missing = _STATUS_KEYS - fields.keys()
    extra = fields.keys() - _STATUS_KEYS
    if missing:
        raise TelemetryParseError("Missing fields: " + ", ".join(sorted(missing)))
    if extra:
        raise TelemetryParseError("Unexpected fields: " + ", ".join(sorted(extra)))

    distance_raw = fields["Distance_cm"]
    if distance_raw == "NO_ECHO":
        distance_cm = None
    else:
        try:
            distance_cm = float(distance_raw)
        except ValueError as exc:
            raise TelemetryParseError("Distance_cm must be a number or NO_ECHO") from exc
        if not math.isfinite(distance_cm) or not 2.0 <= distance_cm <= 400.0:
            raise TelemetryParseError("Distance_cm must be finite and within 2.0..400.0 cm")

    flame_do = _integer(fields, "Flame_DO", 0, 1)
    flame_ao = _integer(fields, "Flame_AO", 0, 1023)
    flame_status = fields["Flame"]
    if flame_status not in _FLAME_STATES:
        raise TelemetryParseError(f"Flame must be one of {sorted(_FLAME_STATES)}")

    mode = fields["Mode"]
    if mode not in _MODES:
        raise TelemetryParseError(f"Mode must be one of {sorted(_MODES)}")

    active_mask = _integer(fields, "ActiveMask", 0, 3)
    silenced_mask = _integer(fields, "SilencedMask", 0, 3)
    if silenced_mask & ~active_mask:
        raise TelemetryParseError("SilencedMask cannot contain hazards absent from ActiveMask")

    alert = fields["Alert"]
    if alert not in _ALERTS:
        raise TelemetryParseError(f"Alert must be one of {sorted(_ALERTS)}")
    if mode == "DISARMED" and alert != "DISARMED":
        raise TelemetryParseError("A DISARMED record must report Alert=DISARMED")
    if mode == "ARMED" and alert == "DISARMED":
        raise TelemetryParseError("An ARMED record cannot report Alert=DISARMED")

    hazard_starts = _integer(fields, "HazardStarts", 0, 2**32 - 1)
    return StatusRecord(
        distance_cm=distance_cm,
        flame_do=flame_do,
        flame_ao=flame_ao,
        flame_status=flame_status,
        mode=mode,
        active_mask=active_mask,
        silenced_mask=silenced_mask,
        alert=alert,
        hazard_starts=hazard_starts,
    )


def parse_line(line: str) -> Record:
    """Parse a status/event line, returning `None` for unrelated log text.

    Malformed lines that look like LumaGuard records raise
    :class:`TelemetryParseError`; informational boot messages are ignored.
    """
    candidate = line.strip()
    if not candidate:
        return None
    if candidate.startswith("Distance_cm="):
        return parse_status_line(candidate)
    if candidate.startswith("EVENT:"):
        label = candidate.removeprefix("EVENT:").strip()
        if label not in _EVENT_NAMES:
            raise TelemetryParseError(f"Unknown LumaGuard event: {label!r}")
        return EventRecord(event=_EVENT_NAMES[label], message=candidate)
    if candidate.startswith("SYSTEM:"):
        state = candidate.removeprefix("SYSTEM:").strip()
        if state not in _MODES:
            raise TelemetryParseError(f"Unknown system state: {state!r}")
        return EventRecord(event=f"SYSTEM_{state}", message=candidate)
    count_match = _COUNT_RE.fullmatch(candidate)
    if count_match:
        return EventRecord(
            event="HAZARD_START_COUNT",
            message=candidate,
            count=int(count_match.group(1)),
        )
    if candidate.startswith("HAZARD START COUNT:"):
        raise TelemetryParseError("Hazard start count must be a non-negative integer")
    if candidate.startswith("OLED init failed"):
        return EventRecord(event="OLED_INIT_FAILED", message=candidate)
    return None
