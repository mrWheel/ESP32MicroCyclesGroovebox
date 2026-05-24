# ESP32 MicroCycles Groovebox R3
# Developer Build Guide From Scratch

This guide explains how to rebuild the full system from zero, including firmware, wiring assumptions, SD card layout, and validation steps.

## 1. Scope

This guide targets:

- Hardware revision: R3
- Firmware environment: ESP32GrooveboxR3
- Audio sample source: SD card in /samples

This guide does not target R2.

## 2. Prerequisites

## 2.1 Hardware

- ESP32 board compatible with this project (ESP32-WROVER recommended)
- ST7789 display (320x240)
- Rotary encoder with push button
- KEY0 button
- I2S DAC or DAC/amplifier board
- microSD card module wired in SPI mode
- microSD card (FAT16 or FAT32)
- USB cable for flashing and serial monitor

## 2.2 Software

- Visual Studio Code
- PlatformIO extension
- PlatformIO Core CLI available in terminal (pio)

## 3. Project Setup

1. Clone or copy this repository.
2. Open the repository folder in VS Code.
3. Ensure PlatformIO detects the project (platformio.ini at root).
4. Use environment ESP32GrooveboxR3.

## 4. R3 Pin Mapping

Pin mapping is defined in platformio.ini under env:ESP32GrooveboxR3.

### Display

- PIN_TFT_BL=2
- PIN_TFT_RST=4
- PIN_TFT_CS=5
- PIN_TFT_SCL=18
- PIN_TFT_SDA=23
- PIN_TFT_DC=15

### Input

- PIN_ENC_BTN=35
- PIN_KEY0=0
- PIN_ENC_A=36
- PIN_ENC_B=39

### Audio (I2S)

- PIN_I2S_BCLK=26
- PIN_I2S_WS=25
- PIN_I2S_DOUT=27
- PIN_I2S_SD=14

Note: PIN_I2S_SD is used as enable/shutdown control for the audio board and is driven HIGH by firmware.

### SD Card (SPI)

- PIN_SD_CS=13
- PIN_SD_SCK=18
- PIN_SD_MISO=19
- PIN_SD_MOSI=23

Note: SD and TFT share SCK and MOSI on R3.

## 5. SD Card Content

The firmware expects these exact files:

- /samples/kick.wav
- /samples/snare.wav
- /samples/ch.wav
- /samples/oh.wav
- /samples/tone.wav
- /samples/metal.wav

If one or more files are missing, firmware falls back to internal generated waveforms for missing slots.

## 6. Build Flags You Should Know

In platformio.ini, env:ESP32GrooveboxR3:

- TEST_TONE: optional audio path test mode (disables normal sample playback)
- TEST_TONE_FREQUENCY_HZ: frequency used in TEST_TONE mode
- AUDIO_MASTER_GAIN_PERCENT: final software output gain
- SD_SMOKE_TEST: isolated SD diagnostics mode at boot

Keep TEST_TONE disabled for normal groovebox usage.

Keep SD_SMOKE_TEST disabled for normal groovebox usage.

## 7. Build And Flash

From terminal in repository root:

- pio run -e ESP32GrooveboxR3

Upload is done by the developer on hardware (do not automate upload unless explicitly requested).

## 8. First Boot Validation

Open serial monitor at 115200 and verify:

1. Boot log shows R3 pin mapping.
2. Audio engine initializes without pin conflict errors.
3. Sample manager reports SD status and sample load result.

## 9. SD Smoke Test Workflow

Use this when SD card behavior is uncertain.

1. In platformio.ini, enable SD_SMOKE_TEST for ESP32GrooveboxR3.
2. Build and flash.
3. Open serial monitor.
4. Check output:
   - SD init attempts and frequency
   - mount status
   - root directory listing
   - presence of each /samples/*.wav file
5. Disable SD_SMOKE_TEST after diagnostics.

In SD smoke test mode, firmware intentionally halts after diagnostics.

## 10. Runtime Architecture (Quick Orientation)

Main runtime modules:

- src/main.cpp: startup, tasks, orchestration
- src/uiManager.cpp: UI state machine and controls
- src/audioEngine.cpp: I2S output and mixer
- src/sampleManager.cpp: SD sample loading and WAV decoding
- src/sequencer.cpp: sequencing logic
- src/settingsStore.cpp: settings and pattern persistence
- src/systemManager.cpp: Wi-Fi/system command handling

## 11. Common Failure Modes And Fast Checks

## 11.1 SD Mount Fails

- Verify CS/SCK/MISO/MOSI wiring exactly
- Verify shared SPI lines with TFT are stable
- Verify card format (FAT16 or FAT32)
- Run SD_SMOKE_TEST and inspect missing file names

## 11.2 No Sound

- Verify I2S wiring (BCLK, WS, DOUT)
- Verify PIN_I2S_SD connection to DAC enable/shutdown behavior
- Confirm TEST_TONE is disabled for normal playback
- Confirm sample files are present on SD

## 11.3 Distorted Sound

- Lower AUDIO_MASTER_GAIN_PERCENT
- Verify DAC power and grounding
- Verify sample WAV files are valid PCM WAV

## 12. Rebuild Checklist For A New Developer

1. Open repository and platformio.ini
2. Confirm env:ESP32GrooveboxR3 pin map
3. Prepare SD card with required /samples/*.wav files
4. Build with pio run -e ESP32GrooveboxR3
5. If SD issues appear, enable SD_SMOKE_TEST and diagnose
6. Disable diagnostic flags
7. Rebuild and run normal firmware

## 13. Pattern JSON Schema (Current)

Pattern files are stored in:

- /sequences/Pnnn.json

Top-level fields:

- version
- name
- bpm
- swing
- masterLevel
- tracks

Each track contains:

- name
- machine
- sample
- mute
- solo
- level
- pan
- steps

Each step contains:

- trig
- velocity
- locks (object)

The loader now expects this schema directly.
Backward conversion from older pattern schemas is intentionally not used.

## 14. Related Documentation

- User manual home: docs/README.md
- Browser manual index: docs/index.html
- Coding conventions: codingRules.md
