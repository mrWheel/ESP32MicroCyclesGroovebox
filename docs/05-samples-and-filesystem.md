# 5. Samples and Sequences

## Samples

The Groovebox reads samples from SD card files:

- Kick
- Snare
- CH
- OH
- Tone
- Metal

Required paths on SD card:

- `/samples/kick.wav`
- `/samples/snare.wav`
- `/samples/ch.wav`
- `/samples/oh.wav`
- `/samples/tone.wav`
- `/samples/metal.wav`

## Sequence Storage

User sequences are saved in internal flash filesystem.

- Sequence folder: `/sequences`
- Naming format: `Snnn` (example: `S014`)

## If a Sample Sounds Wrong

- First test with `TEST_TONE` to verify audio wiring path.
- Then disable `TEST_TONE` again for normal drum playback.
- Lower `AUDIO_MASTER_GAIN_PERCENT` if output is clipping or harsh.
