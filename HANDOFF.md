# RadioMusic Kick Handoff

This project is for Music Thing Modular Radio Music / Radio Station hardware. Use Radio Music panel language. Do not frame the work as Chord Organ.

## Goal

Build a pure kick module:

- no sample drum paths
- no generic multi-drum behavior
- no snare/hat/sample engine reintroduction
- clean direct code suitable for a delicate musical instrument
- exactly four separate kick engines, switchable with the button

## Current State

Fork:

```text
https://github.com/nymphnerds/RadioMusic-kick
```

Main branch target:

```text
main
```

Current release target:

```text
v1.0.6
```

Current built hex:

```text
MultiDrums.hex
SHA256: 2fb7f9d9400dbb40512903cbd79c015f5c7fdc79eeb027bf1b7ecb9f97f4ef07
```

## Engine Model

There are four engines:

1. **909**
   - Default engine.
   - Sine body, pitch drop, noise click, soft clipping, low-pass shaping.
   - Strong velocity/accent response.

2. **808**
   - Boomy bridged-T/resonator-style engine.
   - Long decay and gate-length boom.
   - Fast strike bend instead of a slow pitch sweep.
   - Start CV accent adds body, short attack pulse, and high-velocity saturation.

3. **Hot 909**
   - Separate 909 voice state.
   - Uses the proven 909 core with extra post-drive.
   - Page 2 macros feed the hotter punch/drive behavior.

4. **Dirty 808**
   - Separate 808 voice state.
   - Uses the proven 808 core with extra post-drive.
   - Page 2 macros feed sweep/drive behavior.

Slots 3 and 4 deliberately reuse the two proven musical cores. Avoid replacing them with abstract scratch engines unless there is a strong tested reason.

## Controls

Radio Music panel names:

- Station knob: tune offset
- Station CV: tune CV
- Start knob: decay
- Start CV: accent
- Page 2 Station: engine macro 1
- Page 2 Start: engine macro 2

Button behavior:

- short tap: cycle 909 / 808 / Hot 909 / Dirty 808
- long hold: toggle page 1 / page 2
- LED3: 909
- LED2: 808
- LED1: Hot 909
- LED0: Dirty 808
- reset LED flashing: page 2 active

## Settings / Persistence

Page 1 and page 2 storage must stay separate.

Rules:

- page 1 stores tune/decay per engine
- page 2 stores macro values per engine
- page 2 values must not overwrite tune/decay
- engine/page switching should not immediately write physical knob positions into saved params

Current persistence:

- EEPROM base: `64`
- magic: `RK`
- version: `4`
- engines: `4`
- settings slots per engine: `6`
- delayed save after stable edits

## OXI / CV Notes

OXI One pitch CV useful direct range starts at C2 because that is the first 0V note from the OXI.

```text
C-1 = -3V, below readable range
C0  = -2V, below readable range
C1  = -1V, below readable range
C2  =  0V, first useful pitch CV point
C3  = +1V
C4  = +2V
C5  = +3V
```

Radio Music analog inputs read positive CV only. Negative pitch CV needs external offset.

OXI velocity:

- range: 0-127
- default velocity: 75
- velocity 0 should be unaccented
- higher velocity should add body/attack/saturation without killing decay

## Code Map

- `MultiDrums.ino`
  - hardware setup
  - audio routing
  - engine constants/global state
  - settings block constants

- `functions.ino`
  - panel and CV reads
  - button/page/engine switching
  - trigger handling
  - settings persistence

- `Brain.cpp` / `Brain.h`
  - 808 engine
  - 909 engine
  - Hot 909 wrapper
  - Dirty 808 wrapper
  - audio callback

## Development Rules

- Preserve a known-good hex before experiments.
- Change one musical behavior at a time.
- Do not overwrite a liked sound with a broad rewrite.
- Do not reintroduce sample drum code.
- Do not add poly/tail-overlap unless explicitly restarted as a separate experiment.
- Do not touch unrelated Chord Organ work.
- Do not put session-history language in the README.

## Useful Future Work

Best next directions:

- Tune Hot 909 and Dirty 808 by ear against the main 909/808.
- Improve page 2 macro meaning per engine.
- Make Hot 909 more obviously aggressive without wrecking low/mid velocity.
- Make Dirty 808 more clearly different while keeping the 808 boom.
- Keep Start CV accent musically direct: body, attack, drive, saturation.

Avoid for now:

- brand-new FM/sample/digital scratch engines
- complicated delay experiments
- tail-overlap/poly experiments
- broad rewrites of the tuned 808/909 pitch handling
