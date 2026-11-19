"""Command-line interface for converting LumaGuard logs to JSON Lines."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import TextIO

from .errors import TelemetryParseError
from .parser import parse_line


def convert_stream(source: TextIO, destination: TextIO, *, strict: bool = False,
                   source_name: str = "<stdin>") -> tuple[int, int]:
    """Write parsed records as JSONL; return `(records_written, parse_errors)`."""
    records_written = 0
    errors = 0
    for line_number, line in enumerate(source, start=1):
        try:
            record = parse_line(line)
        except TelemetryParseError as exc:
            errors += 1
            print(f"{source_name}:{line_number}: {exc}", file=sys.stderr)
            continue
        if record is None:
            continue
        destination.write(json.dumps(record.to_dict(), separators=(",", ":"),
                                     ensure_ascii=False) + "\n")
        records_written += 1
    return records_written, errors


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="lumaguard-telemetry",
        description="Convert LumaGuard serial diagnostics into validated JSON Lines.",
    )
    subparsers = parser.add_subparsers(dest="command", required=True)
    convert = subparsers.add_parser("convert", help="convert a serial log to JSONL")
    convert.add_argument("input", nargs="?", default="-",
                         help="input log path, or - for standard input (default: -)")
    convert.add_argument("-o", "--output", default="-",
                         help="output JSONL path, or - for standard output (default: -)")
    convert.add_argument("--strict", action="store_true",
                         help="return exit code 2 if any malformed telemetry record is found")
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.command != "convert":
        return 2

    input_stream: TextIO
    output_stream: TextIO
    close_input = close_output = False
    try:
        if args.input == "-":
            input_stream = sys.stdin
            input_name = "<stdin>"
        else:
            input_stream = Path(args.input).open("r", encoding="utf-8")
            input_name = args.input
            close_input = True
        if args.output == "-":
            output_stream = sys.stdout
        else:
            output_stream = Path(args.output).open("w", encoding="utf-8", newline="\n")
            close_output = True
        try:
            written, errors = convert_stream(
                input_stream, output_stream, strict=args.strict, source_name=input_name
            )
            print(f"Wrote {written} record(s); {errors} malformed record(s).", file=sys.stderr)
            return 2 if args.strict and errors else 0
        finally:
            if close_input:
                input_stream.close()
            if close_output:
                output_stream.close()
    except OSError as exc:
        print(f"lumaguard-telemetry: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
