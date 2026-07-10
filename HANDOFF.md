# RadioMusic Kick Handoff

This project is for MTM Radio Music / Radio Station hardware. Use Radio Music panel language. Do not frame the work as Chord Organ.

## Goal

Build a pure kick module:

- no sample drum paths
- no generic multi-drum behavior
- no snare/hat/sample engine reintroduction
- clean direct code suitable for a delicate musical instrument

The final direction is exactly four separate kick synthesis engines, switchable with the button.

Current path:

1. Keep 909 and 808 stable and musically usable.
2. Add independent kick engines one at a time.
3. Keep each engine's character intact.
4. Finish with four independent kick synth paths.

## Current State

Fork:

```text
https://github.com/nymphnerds/RadioMusic-kick
```

Current branch/work target:

```text
main
```

Current release target:

```text
v1.0.5
```

Current built hex:

```text
MultiDrums.hex
SHA256: a89133241c673c5a9caf51a9ff0b580b869a8ce7e2ad7f19d2f25f62daf546d7
```

## Engine Model

There are two switchable engines.

### 909 Engine

The 909 engine is currently the default.

Keep it as its own synthesis path:

- sine body oscillator
- pitch drop
- noise click
- body envelope with `gateSamples/bodyFall`
- soft clip
- low-pass shaping
- strong velocity/accent response
- cached pitch-CV rate calculation

Do not turn the 909 into an 808 with different constants.

### 808 Engine

The 808 engine is now moving toward a struck-resonator model.

Current direction:

- bridged-T-style sine body
- low-pass retained
- long decay and gate-length boom
- Station CV pitch tracking retained
- page 2 Station acts as fast strike bend, not slow pitch sweep
- Start CV accent adds body, attack pulse, and high-velocity saturation
- accent must not change base tune or base decay

Important lesson: the old page-2 pitch sweep felt wrong because it behaved like a slow pitch raise. The better direction is:

```text
trigger -> brief higher strike bend -> fast fall to tuned body
```

## Controls

Radio Music panel names:

- Station knob: tune offset
- Station CV: tune CV
- Start knob: decay
- Start CV: accent
- Page 2 Station: engine macro 1
- Page 2 Start: engine macro 2

Current page 2:

- 808 Station: strike bend amount
- 808 Start: attack/click character
- 909 Station/Start: reserved for future experiments

Button behavior:

- short tap: toggle 909 / 808
- long hold: toggle page 1 / page 2
- LED3: 909 selected
- LED2: 808 selected
- reset LED flashing: page 2 active

## Settings / Persistence

Page 1 and page 2 storage must stay separate.

Rules:

- page 1 stores tune/decay per engine
- page 2 stores macro values per engine
- page 2 values must not overwrite tune/decay
- engine/page switching should not immediately write physical knob positions into saved params

Current code includes a `kickControlsJustSwitched` guard to prevent page/engine switches from snapping params to stale knob reads.

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

## Development Rules

- Preserve a known-good hex before experiments.
- Change one musical behavior at a time.
- Do not overwrite a liked sound with a broad rewrite.
- Do not reintroduce sample drum code.
- Do not add poly/tail-overlap unless explicitly restarted as a separate experiment.
- Do not touch unrelated Chord Organ work.
- Do not put session-history language in the README.

## Next Sensible Work

- Make 808 page 2 Start more obviously useful as click/attack without harsh distortion.
- Decide whether 909 page 2 should become experimental macros or stay reserved.
- Fine tune gain/accent balance between 909 and 808.
- Add engine 3 only after 808/909 switching, page storage, and docs are stable.
