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

Current code anchors:

- `Kick_customBridgeT()` now takes the current strike offset directly instead of deriving a slow pitch envelope from decay.
- `Kick_process()` latches Start CV accent at the trigger edge.
- 808 accent currently uses:
  - `clickEnv` for body level
  - `strikeEnv` for fast strike bend
  - `attackEnv` for the short attack pulse
  - `highAccentSat` for high-velocity saturation
- Page 2 Station currently feeds `pitchEnvInt`, which becomes 808 strike bend amount.
- Page 2 Start currently feeds `hardness`, but its audible role is still weaker than desired.
- The 808 low-pass is retained because upstream HoRa uses it and real 808 descriptions include low-pass/VCA shaping.

Do not bring back the old slow page-2 pitch sweep. It sounded like a generic pitch rise, not an 808 strike.

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

## Research Notes For Page 2 Experiments

Real 808/909 and broader kick-synthesis research points toward these useful building blocks:

- 808 bass drum: sine/bridged-T-style resonator, low-pass, VCA, decay, and a short CV attack pulse mixed into the sound.
- 808 character: deep boom, tuned body, fast strike behavior, subtle instability/component feel.
- 909 character: punchier body, stronger attack, pitch drop, noise click, clipping/compression-like edge.
- Modern kick shaping: transient design, saturation, subharmonics, compression, wavefolding/asym clipping, and controlled pitch/phase variation.
- Modern 808 processing: tune the kick, preserve the sub, add low-mid harmonics with saturation, shape transients carefully, and avoid uncontrolled sub mud below the useful range.
- Drum FX research: useful processors include compression, saturation, transient shaping, filters, pitch/formant shift, waveshaping, clippers, texture/noise layers, and low-end enhancement.

Keep page 1 stable. Treat page 2 as engine-specific experimental macros.

## Kick FX Ideas

These are FX-style ideas that could live behind page 2 or respond to Start CV accent. They should be tried as internal kick processors, not as broad rewrites.

### Utility FX

- **Transient shaper**
  - Increase or soften only the initial attack.
  - Useful for making accents audible without changing base decay.

- **Parallel saturation**
  - Blend clean body with a driven copy.
  - Good for adding low-mid harmonics while preserving sub.

- **Sub cleanup / sub contour**
  - Gentle high-pass or controlled sub decay below the main body.
  - Goal is tighter loud playback, not thinner sound.

- **Tilt / tone EQ**
  - One macro shifts darker/rounder versus brighter/clickier.
  - Could be implemented with simple filter blend or post-body tone shaping.

- **Accent FX send**
  - Start CV controls how much processing happens.
  - Low velocity remains clean; high velocity gets drive, attack, or compression.

### Character FX

- **Asymmetric clipping**
  - Diode-ish drive, more odd/even character than plain soft clip.
  - Best first on 909, where aggression is welcome.

- **Wavefolder / foldback**
  - Wild industrial kick texture.
  - Dangerous for output/gain, so keep dry/wet and safety conservative.

- **Body compressor**
  - Holds the boom forward without changing decay time.
  - Could be a simple envelope-dependent gain curve rather than a full compressor.

- **Attack texture / noise color**
  - Shape click tone and click decay.
  - Useful for both 909 snap and 808 attack pulse.

- **Analog drift**
  - Tiny random pitch, phase, or accent changes per hit.
  - Should feel like hardware tolerance, not obvious random detune.

- **Subharmonic body**
  - Add octave-under or divided-body energy.
  - Useful for 808 bassline territory, but must keep tune stable and avoid mud.

Best first FX experiments:

1. 808: transient shaper plus attack pulse tone/level.
2. 808: parallel saturation controlled by Start CV.
3. 909: snap/pitch-drop plus asymmetric clipping.
4. 909: body compression/soft clip macro.

## Page 2 Experiment Menu

Try these one at a time. Build a hex after each and test before stacking ideas.

### 808 Experiments

1. **Attack/Click Macro**
   - Page 2 Station: strike bend amount
   - Page 2 Start: attack pulse level/tone
   - Goal: make page 2 Start clearly audible without harsh clipping.

2. **Resonator Energy**
   - Page 2 Station: strike bend amount
   - Page 2 Start: body/Q/drive into the resonator
   - Goal: more "hit into circuit" energy rather than plain output volume.

3. **Analog Drift**
   - Page 2 Station: per-hit pitch/phase drift amount
   - Page 2 Start: random accent/body variation
   - Goal: tiny hardware-like inconsistency, not obvious detune.

4. **Subharmonic Body**
   - Page 2 Station: sub octave blend
   - Page 2 Start: sub decay
   - Goal: 808 bassline territory while keeping the main kick tune stable.

5. **Dirt Circuit**
   - Page 2 Station: saturation shape
   - Page 2 Start: drive amount
   - Goal: audible high-velocity growl that follows the decay tail.

6. **Transient FX Send**
   - Page 2 Station: attack transient shape
   - Page 2 Start: accent-to-FX amount
   - Goal: low velocity clean, high velocity punchy and present.

7. **Sub Contour**
   - Page 2 Station: sub blend or sub emphasis
   - Page 2 Start: sub tail contour
   - Goal: heavier low end without uncontrolled rumble.

### 909 Experiments

1. **Snap/Punch**
   - Page 2 Station: pitch-drop snap
   - Page 2 Start: click level
   - Goal: more classic aggressive 909 transient.

2. **Body Compression**
   - Page 2 Station: body gain into soft clip
   - Page 2 Start: clip/compression amount
   - Goal: rave/industrial 909 without destroying the separate engine character.

3. **Noise Click Color**
   - Page 2 Station: noise click tone
   - Page 2 Start: noise click decay
   - Goal: useful attack range from soft thud to hard snap.

4. **Asymmetric Clip**
   - Page 2 Station: drive
   - Page 2 Start: asymmetry / dry-wet
   - Goal: aggressive 909 without flattening all dynamics.

5. **Fold / Crush**
   - Page 2 Station: foldback amount
   - Page 2 Start: post-fold level or tone
   - Goal: extreme industrial/rave kicks as an optional page-2 mode.

## Next Sensible Work

- Make 808 page 2 Start more obviously useful as click/attack without harsh distortion.
- Decide which 909 page 2 experiment should be tried first.
- Fine tune gain/accent balance between 909 and 808.
- Add engine 3 only after 808/909 switching, page storage, and docs are stable.
