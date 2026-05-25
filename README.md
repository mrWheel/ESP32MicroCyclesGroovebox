# ESP32 MicroCycles Groovebox (R3)

This repository contains firmware for the ESP32 MicroCycles Groovebox on the R3 hardware revision.

## Developer Guide

For a full from-scratch developer setup and rebuild workflow, read:

- [DEVELOPER_FROM_SCRATCH.md](DEVELOPER_FROM_SCRATCH.md)

## User Manual

The end-user manual is in the docs folder.

- Start page: [docs/index.html](docs/index.html)
- Manual home: [docs/README.md](docs/README.md)

## Current Scope

- Hardware focus: R3 only
- 6 drum/sample tracks
- 16-step sequencer
- Per-step velocity and probability in the active sequencer model
- Track mute and swing playback controls
- Pattern load/save/new/delete from System Settings
- Optional Wi-Fi setup from System Settings
- SD card sample set from `/samples/*.wav`

## Build Environment

Use PlatformIO environment:

- `ESP32GrooveboxR3`

## Notes

- `TEST_TONE` is for audio-path testing and disables normal sample playback while active.
- `AUDIO_MASTER_GAIN_PERCENT` controls final software output volume.
