"""Domain-specific parsing errors."""


class TelemetryParseError(ValueError):
    """Raised when a LumaGuard telemetry record is malformed or invalid."""
