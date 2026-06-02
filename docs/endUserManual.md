# ESP32 MicroCycles Groovebox R3
# End User Manual 

Software Version: v0.8.5

## Introduction

Welcome to the ESP32 MicroCycles Groovebox.

This instrument is designed around a simple idea:

- Load a Pattern Group
- Create patterns
- Chain patterns into songs
- Save your work
- Perform live

This manual focuses on making music, not programming.

---

# 1 - First Power On

After boot the Groovebox automatically loads:

- The last active Pattern Group
- The last active Sample Set
- All patterns belonging to that group

When startup finishes you will see the main Groovebox screen.

```text
 BPM:124 SW:54 PLAY

 KICK  X...X...X...X...
 SNARE ....X.......X...
 CH    X.X.X.X.X.X.X.X.
 OH    ....X.......X...
 TONE  ................
 METAL ................

 P01 -> P02
 GROUP: HOUSEJAM
```

What you are seeing:

- BPM = current tempo
- SW = swing amount
- Six drum/percussion tracks
- Current pattern
- Next chained pattern
- Active Pattern Group

---

# 2 - Controls

## Encoder

Rotate:
- Move cursor
- Select menu entries
- Change values

Short press:
- Confirm
- Enter edit mode

Medium press:
- Tempo editor

Long press:
- System Settings

## KEY0

Short press:
- Play / Stop

Medium press:
- BPM quick edit

Long press:
- Swing quick edit

---

# 3 - Understanding Patterns

A Pattern is one musical section.

Examples:

```text
P01 Intro
P02 Groove
P03 Breakdown
P04 Drop
```

Think of each pattern as a building block.

A complete song may contain:

```text
P01 Intro
P02 Groove
P03 Groove Variation
P04 Breakdown
P05 Build
P06 Drop
```

---

# 4 - Understanding Tracks

The Groovebox contains six tracks.

```text
KICK
SNARE
CH
OH
TONE
METAL
```

Suggested uses:

KICK
- Bass drum

SNARE
- Snare
- Clap

CH
- Closed hi-hat

OH
- Open hi-hat

TONE
- Tom
- Percussion
- Bass hit

METAL
- Ride
- Cymbal
- FX

---

# 5 - Trigger Page (TRIG)

The Trigger page decides when sounds play.

Example:

```text
X...X...X...X...
```

Meaning:

```text
1
5
9
13
```

What you hear:

- More triggers = busier rhythm
- Fewer triggers = more space

Good beginner pattern:

```text
KICK  X...X...X...X...
SNARE ....X.......X...
CH    X.X.X.X.X.X.X.X.
```

---

# 6 - Velocity (VEL)

Range:

```text
1 - 127
```

Velocity controls impact.

| Value | What You Hear |
|---------|---------|
| 1-20 | Almost silent |
| 21-50 | Soft hit |
| 51-90 | Normal hit |
| 91-120 | Strong hit |
| 121-127 | Maximum attack |

Example:

```text
127 80 115 70
```

This sounds more human than:

```text
127 127 127 127
```

---

# 7 - Pitch (PITCH)

Range:

```text
-24 to +24
```

What you hear:

| Value | Result |
|---------|---------|
| -24 | Very deep |
| -12 | One octave lower |
| 0 | Original sound |
| +12 | One octave higher |
| +24 | Very high |

Useful for:

- Tuned kicks
- Bass sounds
- Melodic percussion

---

# 8 - Decay (DECAY)

Range:

```text
0 - 127
```

Controls sound length.

| Value | What You Hear |
|---------|---------|
| 0-20 | Extremely short |
| 21-50 | Tight |
| 51-90 | Natural |
| 91-127 | Long tail |

Typical settings:

Kick:

```text
20-50
```

Open Hat:

```text
90-127
```

---

# 9 - Probability (PROB)

Range:

```text
0 - 100%
```

Probability decides how often a step plays.

| Value | Result |
|---------|---------|
| 100% | Always |
| 75% | Usually |
| 50% | Coin flip |
| 25% | Rare |
| 0% | Never |

Musical use:

Ghost snare:

```text
30%
```

Main snare:

```text
100%
```

Creates variation automatically.

---

# 10 - Mute

Values:

```text
OFF
ON
```

Use during performance.

Mute allows tracks to disappear without deleting steps.

---

# 11 - Chain

Chain creates song structure.

Example:

```text
P01 -> P02
P02 -> P03
P03 -> P04
P04 -> P01
```

Result:

```text
P01
P02
P03
P04
repeat
```

Another example:

```text
P01 -> P02
P02 -> P03
P03 -> P02
```

Creates an endless groove.

---

# 12 - Tempo Screen

```text
+------------------+
| BPM              |
|                  |
|      124         |
|                  |
+------------------+
```

Range:

```text
30 - 300 BPM
```

Suggested styles:

```text
80-100 Hip Hop
110-125 House
125-140 Techno
140-175 Drum & Bass
```

---

# 13 - Swing Screen

```text
+------------------+
| SWING            |
|                  |
|       56         |
|                  |
+------------------+
```

Range:

```text
50 - 75
```

What you hear:

50:
- Straight

54:
- Groove

58:
- Strong shuffle

65+:
- Heavy swing

---

# 14 - Pattern Groups

Pattern Groups store collections of patterns.

Examples:

```text
HOUSEJAM
TECHNO01
LIVESET
IDEAS
```

A group may contain:

```text
P01
P02
P03
P04
P05
...
```

---

# 15 - Card Storage Screen

```text
 Load Pattern
 Save Pattern
 Copy Group
 Rename Group
 Delete Group
```

All long-term storage happens here.

---

# 16 - Load Pattern

Loads a group from the SD card.

```text
 HOUSEJAM
 TECHNO01
 ACIDLIVE
 DEMOSONG
```

Select and confirm.

---

# 17 - Save Pattern

Writes all patterns to the SD card.

During save:

```text
 Saving...
 Please wait
```

Never remove power during save.

---

# 18 - Copy Group

Useful before experimentation.

```text
HOUSEJAM
     |
     v
HOUSEJAM2
```

---

# 19 - Rename Group

Changes the group name only.

```text
OLDNAME
   |
   v
NEWNAME
```

Pattern contents stay unchanged.

---

# 20 - Delete Group

Permanently removes a group.

```text
DELETE?
HOUSEJAM
```

Use carefully.

---

# 21 - Sample Sets

Available sets:

```text
S1
S2
S3
...
S9
```

Changing Sample Set:

- keeps patterns
- changes sound character

Try the same pattern with different sample sets.

The groove remains.
The personality changes.

---

# 22 - System Settings

Example:

```text
 Sample Set
 Display Rotation
 Theme
 Encoder Direction
 WiFi
 Card Storage
```

Settings are remembered after reboot.

---

# 23 - Building A Song

Example workflow:

```text
P01 Intro
P02 Groove
P03 Variation
P04 Breakdown
P05 Build
P06 Drop
```

Chain:

```text
P01 -> P02
P02 -> P03
P03 -> P04
P04 -> P05
P05 -> P06
P06 -> P02
```

You now have a complete arrangement.

---

# 24 - Live Performance Tips

Start simple.

Use:

- Mute
- Velocity
- Probability

before adding complexity.

Keep one pattern stable.

Experiment on another.

Always save before major changes.

---

# 25 - Troubleshooting

No sound:

- Check volume
- Check sample set
- Check mute state

Lost changes:

- Save Pattern Group

Wrong sounds:

- Verify active Sample Set

Missing patterns:

- Load correct Pattern Group

---

# 26 - Quick Start

1. Power on
2. Press PLAY
3. Program KICK
4. Add SNARE
5. Add hats
6. Adjust velocity
7. Create P02
8. Chain P01 -> P02
9. Save Pattern Group
10. Make music

---

# Appendix A - Recommended Starter Groove

```text
KICK  X...X...X...X...
SNARE ....X.......X...
CH    X.X.X.X.X.X.X.X.
OH    ....X.......X...
TONE  ................
METAL ................
```

Tempo:

```text
124 BPM
```

Swing:

```text
54
```

This is an excellent starting point for House and Tech House.
