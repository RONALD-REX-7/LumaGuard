"""Structured parsing for LumaGuard serial telemetry."""

from .errors import TelemetryParseError
from .models import EventRecord, StatusRecord
from .parser import parse_line, parse_status_line

__all__ = [
    "EventRecord",
    "StatusRecord",
    "TelemetryParseError",
    "parse_line",
    "parse_status_line",
]

__version__ = "1.0.0"
