"""Typed records emitted by the serial-log parser."""

from dataclasses import asdict, dataclass
from typing import Any


@dataclass(frozen=True, slots=True)
class StatusRecord:
    distance_cm: float | None
    flame_do: int
    flame_ao: int
    flame_status: str
    mode: str
    active_mask: int
    silenced_mask: int
    alert: str
    hazard_starts: int

    def to_dict(self) -> dict[str, Any]:
        """Return a JSON-serialisable representation."""
        return {"record_type": "status", **asdict(self)}


@dataclass(frozen=True, slots=True)
class EventRecord:
    event: str
    message: str
    count: int | None = None

    def to_dict(self) -> dict[str, Any]:
        """Return a JSON-serialisable representation."""
        result: dict[str, Any] = {
            "record_type": "event",
            "event": self.event,
            "message": self.message,
        }
        if self.count is not None:
            result["count"] = self.count
        return result
