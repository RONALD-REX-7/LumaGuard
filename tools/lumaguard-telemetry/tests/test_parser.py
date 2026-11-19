import io
import json

import pytest

from lumaguard_telemetry import TelemetryParseError, parse_line, parse_status_line
from lumaguard_telemetry.cli import convert_stream

SAFE = (
    "Distance_cm=42.5 | Flame_DO=1 | Flame_AO=1023 | Flame=CLEAR | "
    "Mode=ARMED | ActiveMask=0 | SilencedMask=0 | Alert=SAFE | HazardStarts=0"
)


def test_parse_normal_status_record():
    record = parse_status_line(SAFE)
    assert record.distance_cm == 42.5
    assert record.flame_ao == 1023
    assert record.alert == "SAFE"
    assert record.to_dict()["record_type"] == "status"


def test_no_echo_is_null_not_zero():
    record = parse_status_line(SAFE.replace("Distance_cm=42.5", "Distance_cm=NO_ECHO")
                               .replace("Alert=SAFE", "Alert=ZONE UNKNOWN"))
    assert record.distance_cm is None


def test_parse_zone_event():
    record = parse_line("EVENT: ZONE ALERT STARTED")
    assert record.to_dict()["event"] == "ZONE_ALERT_STARTED"


def test_parse_system_event():
    record = parse_line("SYSTEM: DISARMED")
    assert record.to_dict()["event"] == "SYSTEM_DISARMED"


def test_parse_hazard_count_event():
    record = parse_line("HAZARD START COUNT: 17")
    assert record.to_dict()["count"] == 17


def test_unrelated_boot_text_is_ignored():
    assert parse_line("LumaGuard started; DHT22 excluded") is None


def test_duplicate_field_rejected():
    with pytest.raises(TelemetryParseError, match="Duplicate"):
        parse_status_line(SAFE + " | Alert=SAFE")


def test_missing_field_rejected():
    with pytest.raises(TelemetryParseError, match="Missing fields"):
        parse_status_line(" | ".join(SAFE.split(" | ")[:-1]))


@pytest.mark.parametrize("value", ["NaN", "inf", "401", "1.9", "not-a-number"])
def test_invalid_distance_rejected(value):
    with pytest.raises(TelemetryParseError):
        parse_status_line(SAFE.replace("Distance_cm=42.5", f"Distance_cm={value}"))


def test_silenced_mask_must_be_subset_of_active_mask():
    line = SAFE.replace("ActiveMask=0", "ActiveMask=1").replace("SilencedMask=0", "SilencedMask=2")
    with pytest.raises(TelemetryParseError, match="SilencedMask"):
        parse_status_line(line)


def test_flame_adc_bounds_are_enforced():
    with pytest.raises(TelemetryParseError, match="Flame_AO"):
        parse_status_line(SAFE.replace("Flame_AO=1023", "Flame_AO=1024"))


def test_unknown_event_is_not_silently_accepted():
    with pytest.raises(TelemetryParseError, match="Unknown LumaGuard event"):
        parse_line("EVENT: MADE UP EVENT")


def test_cli_converter_emits_jsonl_and_counts_errors(capsys):
    source = io.StringIO(SAFE + "\nEVENT: FLAME CONFIRMED\nEVENT: UNKNOWN\nnoise\n")
    destination = io.StringIO()
    written, errors = convert_stream(source, destination, source_name="sample.log")
    rows = [json.loads(line) for line in destination.getvalue().splitlines()]
    assert written == 2
    assert errors == 1
    assert rows[0]["record_type"] == "status"
    assert rows[1]["event"] == "FLAME_CONFIRMED"
    assert "sample.log:3" in capsys.readouterr().err
