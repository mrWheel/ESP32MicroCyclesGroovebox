# ESP32 MicroCycles Groovebox
## Delta Notes / Architectural Changes

This document ONLY describes the important architectural differences
between the earlier design and the new implementation direction.

These notes are intended to be merged into the existing README.md.

---

# SECTION: Hardware

## Replace I2S Pin Mapping

REMOVE:

```ini
-D PIN_I2S_BCLK=27
-D PIN_I2S_WS=21
-D PIN_I2S_DOUT=22
```

REPLACE WITH:

```ini
-D PIN_I2S_BCLK=27
-D PIN_I2S_WS=4
-D PIN_I2S_DOUT=5
```

Reason:

- GPIO21 and GPIO22 are internally connected to the TFT display
- GPIO21 and GPIO22 are therefore NOT available for I2S audio

---

# SECTION: Hardware
## Add GPIO Warning Notes

ADD:

```text
GPIO04 and GPIO05 are ESP32 strapping pins.

Requirements:
- use weak pull resistors only
- verify cold boot behavior
- keep DAC inputs high impedance
```

Recommended DAC wiring:

```text
GPIO27 → I2S_BCLK
GPIO04 → I2S_WS
GPIO05 → I2S_DOUT
```

---

# SECTION: Libraries
## Remove LittleFS Requirement

REMOVE:

```text
LittleFS
```

Reason:

- ESP32-WROVER N4R8 does not provide enough flash space
  for reliable WAV sample storage
- filesystem access during playback is undesirable
- deterministic playback is preferred

---

# SECTION: Sample Storage Strategy
## Completely Replace LittleFS Sample Architecture

REMOVE ALL REFERENCES TO:

- sample WAV files in LittleFS
- data/samples/
- filesystem sample loading
- runtime WAV file access

REPLACE WITH:

```text
Samples are compiled directly into firmware
as const uint8_t PROGMEM arrays.
```

---

# SECTION: Sample Storage Strategy
## Add Sample Header Architecture

ADD:

```text
include/
├── sampleKick.h
├── sampleSnare.h
├── sampleCh.h
├── sampleOh.h
├── sampleTone.h
└── sampleMetal.h
```

Each file contains:

- WAV header
- WAV sample data
- total array length

---

# SECTION: WAV Conversion
## Add WAV to Header Conversion Workflow

ADD:

```bash
xxd -i kick.wav > sampleKick.h
```

Recommended cleanup:

```cpp
#pragma once
#include <Arduino.h>

const uint8_t sampleKickWav[] PROGMEM =
{
  0x52, 0x49, 0x46, 0x46
};

const uint32_t sampleKickWavLen = sizeof(sampleKickWav);
```

Important:

- preserve the WAV header
- existing WAV parser can still be used
- no filesystem code required

---

# SECTION: Sample Naming
## Add Logical Sample Mapping

ADD:

| Track | Sample |
|---|---|
| Track 1 | kick.wav |
| Track 2 | snare.wav |
| Track 3 | ch.wav |
| Track 4 | oh.wav |
| Track 5 | tone.wav |
| Track 6 | metal.wav |

Recommended source mapping:

| Original File | Logical Name |
|---|---|
| kick.wav | kick.wav |
| snare.wav | snare.wav |
| hihat-closed.wav | ch.wav |
| hihat-open-1.wav | oh.wav |
| tom-l.wav | tone.wav |
| rim.wav | metal.wav |

---

# SECTION: Audio Architecture
## Replace Audio Source Layer

REMOVE:

```text
LittleFS
→ preload samples
→ sample pool
→ voice mixer
→ I2S DMA
→ DAC
```

REPLACE WITH:

```text
Compiled WAV Arrays
→ WAV parser
→ sample pool
→ voice mixer
→ I2S DMA
→ DAC
```

---

# SECTION: Memory Strategy
## Replace Flash Usage Description

REMOVE:

```text
LittleFS used for sample storage
```

REPLACE WITH:

```text
Flash / PROGMEM is used for:
- WAV sample arrays
- UI bitmaps
- fonts
```

---

# SECTION: Directory Layout
## Remove data/samples/

REMOVE:

```text
data/
└── samples/
```

ADD TO include/:

```text
├── sampleKick.h
├── sampleSnare.h
├── sampleCh.h
├── sampleOh.h
├── sampleTone.h
└── sampleMetal.h
```

---

# SECTION: Phase 1
## Replace Filesystem Feature Block

REMOVE:

```text
### Filesystem

- initialize LittleFS
- preload samples into RAM/PSRAM
- validate WAV headers
- fixed sample pool
```

REPLACE WITH:

```text
### Sample System

- compile samples into firmware
- parse WAV headers from memory arrays
- fixed sample pool
- fixed voice pool
```

---

# SECTION: Critical Rules
## Add Audio Determinism Note

ADD:

```text
Audio playback must never depend on:
- filesystem latency
- SD card access
- WiFi activity
- heap allocation
```

---

# SECTION: Design Philosophy
## Add Embedded Appliance Philosophy

ADD:

```text
The groovebox should behave like a dedicated hardware appliance.

Fast boot time, deterministic playback and reliability
are more important than flexible file management.
```