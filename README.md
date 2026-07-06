# RadioMusic Kick

Kick-only firmware for Music Thing Modular Radio Music / Radio Station hardware.

RadioMusic Kick turns the module into a two-engine drum voice:

- **808**: a rounded, boomy kick based on the upstream MultiDrums bass drum
- **909**: a punchier synthesized kick with a sharper transient and more forward body

The firmware is designed for fast use in a modular patch: trigger in, audio out, one button to switch kick engines, and a second page for deeper tone shaping.

## Flashing

Flash the included firmware file:

```text
MultiDrums.hex
```

Current build:

```text
Target: teensy:avr:teensy31
SHA256: b2ea1718b714e289307cbd330f206c0d0c065abfe4ab7c2d2b8ad692f14489d0
```

Only one `.hex` file should be present in the project root.

## Quick Start

1. Patch a trigger or gate into the reset/trigger input.
2. Patch the module output to your mixer.
3. Use a short button tap to switch between 808 and 909.
4. Use the Station and Start knobs to tune and shape the kick.
5. Optional: patch velocity/accent CV into Start CV.

## Controls

The firmware is always in kick mode.

| Action | Result |
| --- | --- |
| Trigger/gate input | Fires the selected kick |
| Short button tap | Toggle 808 / 909 |
| Long button hold | Toggle page 1 / page 2 |
| LED3 on | 808 engine selected |
| LED2 on | 909 engine selected |
| Flashing reset LED | Page 2 active |

## Parameter Pages

| Page | Station knob / CV | Start knob | Start CV |
| --- | --- | --- | --- |
| Page 1 | Tune | Decay | Accent / velocity |
| Page 2 | Pitch sweep | Punch / hardness | Accent / velocity |

Notes:

- Tune is shared between both engines, so switching engines stays in the same musical range.
- Decay, pitch sweep, and punch/hardness are stored separately for each engine.
- Start CV is always accent/velocity. It does not change function on page 2.
- Accent is latched when the kick triggers, so CV changes do not wobble the tail of a hit.
- With no Start CV patched, the module plays at a normal accent level.

## Accent And Velocity

Start CV is intended for a sequencer velocity CV, such as OXI One velocity output.

The two engines respond differently:

### 808

The 808 engine uses a wide dynamic response:

- low velocity: softer, shorter, with a small click transient
- medium velocity: natural main kick tone
- high velocity: louder, slightly longer, with extra pitch movement and punch

Longer trigger gates can extend decay beyond the knob setting.

### 909

The 909 engine uses a tighter accent range so high velocity stays punchy without falling apart.

- low velocity: lighter body and transient
- medium velocity: focused 909 punch
- high velocity: full accent, still controlled

Longer trigger gates extend 909 decay more strongly than the 808 engine.

## Engines

### 808 Engine

The 808 engine keeps the upstream synthesized bass drum path from MultiDrums. It is the rounder, deeper voice and works well for longer, weightier kicks.

This fork adds performance shaping around it:

- trigger-latched accent
- velocity-sensitive level and decay
- separate low-velocity click transient
- restrained high-velocity pitch sweep
- trigger-gate decay extension

### 909 Engine

The 909 engine is a separate fixed-point synthesized kick running inside the existing DSP path.

It uses:

- sine body oscillator
- fast pitch drop
- noise click transient
- simple drive / soft clipping
- low-pass shaping

It is not sample-based.

## Build Notes

Built with Arduino CLI / Teensyduino for:

```text
teensy:avr:teensy31
```

Most recent build size:

```text
Sketch uses 174120 bytes (66%) of program storage.
Global variables use 11856 bytes (18%) of dynamic memory.
```

Developer tuning notes are in:

```text
docs/handoff.md
```

## Credits

Based on HoRaMusic's RadioMusic-MultiDrums firmware:

https://github.com/HoRaMusic/RadioMusic-MultiDrums
