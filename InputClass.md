# InputClass

[Back to README](README.md)

## Purpose

InputClass is a reusable hardware input class for projects that use:
- an EC11-style rotary encoder (A/B quadrature + push button)
- one auxiliary push button (PIN_KEY0 in this project)

The class owns all input state and event generation internally.
There is no wrapper layer and no helper/service forwarding.

## Build Flag Integration

This class uses build_flags from platformio.ini through appConfig.h.

The function inputCreateConfigFromBuildFlags reads these macros:
- PIN_ENC_A
- PIN_ENC_B
- PIN_ENC_BTN
- PIN_KEY0
- ENCODER_SHORT_PRESS_MS
- ENCODER_MEDIUM_PRESS_MS
- ENCODER_LONG_PRESS_MS
- BUTTON_SHORT_PRESS_MS
- BUTTON_MEDIUM_PRESS_MS
- BUTTON_LONG_PRESS_MS

The default constructor InputClass() uses the appConfig.h function, so the global input instance automatically starts with build_flag values.

## Public API

### Types

Encoder events:
- ENCODER_EVENT_NONE
- ENCODER_EVENT_LEFT
- ENCODER_EVENT_RIGHT
- ENCODER_EVENT_SHORT_PRESS
- ENCODER_EVENT_LONG_PRESS
- ENCODER_EVENT_MEDIUM_PRESS

Auxiliary button events:
- BUTTON_EVENT_NONE
- BUTTON_EVENT_SHORT_PRESS
- BUTTON_EVENT_LONG_PRESS
- BUTTON_EVENT_MEDIUM_PRESS

Configuration structs:
- InputPinConfig
- InputTimingConfig
- InputConfig

### Constructors

- InputClass()
  - Uses build_flag-based defaults from inputCreateConfigFromBuildFlags.

- InputClass(const InputConfig& inputConfig)
  - Uses a fully custom pin/timing configuration.

### Initialization and Update

- begin()
  - Configures all input pins as INPUT_PULLUP.
  - Captures current encoder state.
  - Clears all pending events and internal press-tracking state.

- update()
  - Calls updateEncoder() and updateAuxButton().

- updateEncoder()
  - Processes quadrature transitions and encoder button press-duration events.

- updateAuxButton()
  - Processes auxiliary button press-duration events.

### Configuration

- setConfig(const InputConfig& inputConfig)
  - Replaces active config and resets internal runtime state.
  - If already initialized, reapplies pinMode and refreshes last encoder state.

- getConfig() const
  - Returns active configuration.

### Event Access

- getEncoderEvent()
  - Returns and clears the pending encoder event.

- clearEncoderEvent()
  - Clears pending encoder event.

- getAuxButtonEvent()
  - Returns and clears the pending auxiliary button event.

- clearAuxButtonEvent()
  - Clears pending auxiliary button event.

### Encoder Direction

- setEncoderDirectionReversed(bool reversed)
  - Reverses logical LEFT/RIGHT mapping for generated rotation events.

- getEncoderDirectionReversed() const
  - Returns current direction reversal state.

## Operational Behavior

### Encoder Rotation

- The class decodes A/B quadrature changes.
- A full logical detent is emitted after an accumulated delta of +4 or -4 transitions.
- Event mapping:
  - delta +4: RIGHT (or LEFT when direction is reversed)
  - delta -4: LEFT (or RIGHT when direction is reversed)

### Encoder Push Button Timing

- LOW level means pressed.
- Long press is emitted immediately once long threshold is reached while still held.
- On release (if long press was not already emitted):
  - duration >= medium threshold: MEDIUM
  - duration >= short threshold: SHORT
  - duration < short threshold: no event

### Auxiliary Button Timing

- LOW level means pressed.
- Long press is emitted immediately once long threshold is reached while still held.
- On release (if long press was not already emitted):
  - duration >= medium threshold: MEDIUM
  - duration >= short threshold: SHORT
  - duration < short threshold: no event

### Event Queue Model

- Each input source keeps one pending event slot.
- getEncoderEvent and getAuxButtonEvent are consume-on-read.
- clearEncoderEvent and clearAuxButtonEvent explicitly drop pending events.

## Integration in this Repository

- Global instance: input
- Used directly by:
  - main loop/task update path
  - uiMenu event handling
  - system setting Encoder Order toggle

## Portability Notes

To reuse InputClass in another project on the same hardware family:
- Keep default constructor and provide required build macros in appConfig.h.
- Or construct with explicit InputConfig and custom pin/timing values.
- Call begin() once in startup.
- Call update() periodically in a fast task loop.
- Consume events through getEncoderEvent and getAuxButtonEvent.

[Back to README](README.md)
