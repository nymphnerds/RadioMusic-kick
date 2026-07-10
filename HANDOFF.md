# RadioMusic Kick Handoff

This project is for MTM Radio Music / Radio Station hardware. Use Radio Music panel language. Do not frame the work as Chord Organ.

## Goal

Build a pure kick module:

- no sample drum paths
- no generic multi-drum behavior
- no snare/hat/sample engine reintroduction
- clean direct code suitable for a delicate musical instrument

The final direction is exactly four separate kick synthesis engines, switchable with the button. This is not optional or a maybe.

Current path:

1. Keep the main 808-style kick stable.
2. Add independent kick engines one at a time.
3. Keep each engine's character intact.
4. Finish with four independent kick synth paths.

## Current Published State

Fork:

```text
https://github.com/nymphnerds/RadioMusic-kick
```

Current pushed branch:

```text
main
```

Current commit:

```text
v1.0.4 release commit
```

Current built hex:

```text
MultiDrums.hex
SHA256: 64d48d6004de5d6b01f0fec354a243b29d67b56cae90c09624c71eff74d254ac
```

Local branch at time of writing:

```text
accent-saturation
```

It is ahead of its old tracking branch because the latest commit was pushed to `kick-fork/main`.

## Current Engine Model

There are two switchable engines.

### 808 Engine

The 808 engine is the current main/reference kick:

- boomy low-end voice
- most developed decay behavior
- Station CV uses the current 1V/oct-style pitch scaling
- Start CV accent uses a linear internal strike path
- trigger gate length extends the amp envelope naturally

The 808 path is the stable reference for pitch behavior and general playability.

Current 808 accent lesson:

- Do not use post-output trim as accent.
- Do not add the old post-drive/saturation path back as the default.
- Do not paste on extra click/noise as the first accent layer.
- The version that sounded better scales the internal amp/pitch/hardness strike at the trigger edge.
- Keep the 808 accent linear for now.

### 909 Engine

The 909 engine is imported from the earlier failed attempt fork:

```text
https://github.com/nymphnerds/RadioMusic-kick
```

Important: keep the 909 as its own synthesis path. Do not turn it into the 808 with different constants.

Current 909 code should keep the failed-fork character:

- sine body oscillator
- pitch drop
- noise click
- body envelope with `gateSamples/bodyFall`
- soft clip
- low-pass shaping
- velocity/accent response from the failed fork
- cached pitch-CV rate calculation, avoiding expensive `powf()` work inside the per-sample audio loop

Recent lesson: replacing the 909 body with the 808 `Kick_ampEnvelope()` made it sound like the 808 and was rejected. Do not do that again.

## Controls

Radio Music panel names:

- Station knob: tune offset
- Station CV: tune CV
- Start knob: decay
- Start CV: accent
- Page 2 Station: pitch envelope
- Page 2 Start: click

Button behavior:

- short tap: toggle 909 / 808
- long hold: toggle page 1 / page 2
- LED3: 909 selected
- LED2: 808 selected
- reset LED flashing: page 2 active

User-facing docs should call "hardness" **click**.

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
- higher velocity should add accent without killing decay

## Accent Direction

Accent is not fully solved.

Desired direction:

- performance accent, not just volume
- saturation/drive-like character
- should feel integrated into the kick body
- must not clip, cut off, or shorten the decay tail

Current state:

- 808 uses a linear internal strike accent. Start CV is latched at trigger edge and scales amp, pitch-envelope amount, and hardness strike.
- 909 uses the failed-fork velocity/accent behavior and currently has the stronger/nicer accent response.

Do not assume accent is final.

## Development Rules

- Preserve a known-good hex before experiments.
- Change one musical behavior at a time.
- Do not overwrite a liked sound with a broad rewrite.
- Do not reintroduce sample drum code.
- Do not add poly/tail-overlap unless explicitly restarted as a separate experiment.
- Do not touch unrelated Chord Organ work.
- Do not put session-history language in the README.

## Next Sensible Work

Possible next steps:

- Fine tune 909 pitch offset so engine switching lands closer musically.
- Keep 909 character while improving Station CV tracking.
- Revisit 808 accent carefully, starting from the linear internal strike method.
- Add engine 3 only after 808/909 switching is stable and documented.

For 909 pitch matching: adjust its own oscillator constants or base offset. Do not swap in the 808 oscillator/envelope wholesale.
