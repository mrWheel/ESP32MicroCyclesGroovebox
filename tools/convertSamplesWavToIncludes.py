#!/usr/bin/env python3
"""Convert all .wav files from samplesWav into C/C++ include headers.

Default behavior:
- input directory:  samplesWav
- output directory: include
- output filename:  sample<OriginalName>.h

Example:
- kick.wav -> sampleKick.h
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path
from typing import Iterable


def split_name_parts(name: str) -> list[str]:
  return [part for part in re.split(r"[^A-Za-z0-9]+", name) if part]


def to_pascal_case(name: str) -> str:
  parts = split_name_parts(name)

  if not parts:
    return "Sample"

  return "".join(part[:1].upper() + part[1:] for part in parts)


def to_macro_name(name: str) -> str:
  value = re.sub(r"[^A-Za-z0-9]", "_", name)
  value = re.sub(r"_+", "_", value).strip("_")

  if not value:
    value = "sample"

  return value.upper()


def format_byte_lines(data: bytes, bytes_per_line: int = 12) -> Iterable[str]:
  for offset in range(0, len(data), bytes_per_line):
    chunk = data[offset : offset + bytes_per_line]
    formatted = ", ".join(f"0x{value:02X}" for value in chunk)
    yield f"  {formatted},"


def build_header_text(output_stem: str, payload: bytes, source_name: str) -> str:
  guard = f"{to_macro_name(output_stem)}_H"
  symbol_base = output_stem[:1].lower() + output_stem[1:]
  bytes_symbol = f"{symbol_base}WavBytes"
  size_symbol = f"{symbol_base}WavSize"

  lines = [
      f"#ifndef {guard}",
      f"#define {guard}",
      "",
      "#include <stddef.h>",
      "#include <stdint.h>",
      "",
      f"//-- Auto-generated from {source_name}.",
      f"static const uint8_t {bytes_symbol}[] =",
      "{",
  ]

  lines.extend(format_byte_lines(payload))

  lines.extend(
      [
          "};",
          "",
          f"static const size_t {size_symbol} = sizeof({bytes_symbol});",
          "",
          f"#endif // {guard}",
          "",
      ]
  )

  return "\n".join(lines)


def convert_wav_files(source_dir: Path, output_dir: Path) -> int:
  wav_files = sorted(source_dir.glob("*.wav"))

  if not wav_files:
    print(f"No .wav files found in: {source_dir}")
    return 0

  output_dir.mkdir(parents=True, exist_ok=True)

  converted_count = 0

  for wav_file in wav_files:
    original_name = wav_file.stem
    header_stem = f"sample{to_pascal_case(original_name)}"
    header_path = output_dir / f"{header_stem}.h"
    payload = wav_file.read_bytes()

    header_text = build_header_text(header_stem, payload, wav_file.name)
    header_path.write_text(header_text, encoding="utf-8")

    print(f"Generated: {header_path}")
    converted_count += 1

  return converted_count


def parse_arguments() -> argparse.Namespace:
  parser = argparse.ArgumentParser(
      description="Convert all .wav files to C/C++ include headers."
  )
  parser.add_argument(
      "--source",
      default="samplesWav",
      help="Source directory with .wav files (default: samplesWav)",
  )
  parser.add_argument(
      "--output",
      default="include",
      help="Output directory for generated headers (default: include)",
  )

  return parser.parse_args()


def main() -> int:
  args = parse_arguments()
  source_dir = Path(args.source).resolve()
  output_dir = Path(args.output).resolve()

  if not source_dir.is_dir():
    print(f"Source directory does not exist: {source_dir}")
    return 1

  converted_count = convert_wav_files(source_dir, output_dir)
  print(f"Done. Converted {converted_count} file(s).")

  return 0


if __name__ == "__main__":
  raise SystemExit(main())
