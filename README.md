# RadioMusic Kick

Kick-only firmware for Music Thing Modular Radio Music / Radio Station hardware, based on HoRa Music's MultiDrums firmware.

RadioMusic Kick turns the module into a dedicated synthesized bass drum voice. It is not sample playback firmware and it is not a general multi-drum mode.

## Engines

RadioMusic Kick currently has two switchable kick engines:

- **909**: the default engine. Punchy, direct, and responsive to velocity/accent.
- **808**: a boomy resonator-style kick with long decay, musical pitch CV, fast strike bend, and velocity-responsive body.

The long-term direction is four separate kick synthesis engines selected with the button. Each engine should remain its own clean synthesis path.

## Flashing

Flash:

```text
MultiDrums.hex
```

Current build:

```text
Target: teensy:avr:teensy31
Version: v1.0.5
SHA256: a89133241c673c5a9caf51a9ff0b580b869a8ce7e2ad7f19d2f25f62daf546d7
```

## Quick Start

1. Patch a trigger or gate into the reset/trigger input.
2. Patch the audio output to your mixer.
3. The module starts on the 909 engine.
4. Use a short button tap to switch 909 / 808.
5. Use a long button hold to switch parameter pages.
6. Patch pitch CV to Station CV and velocity/accent CV to Start CV.

## Panel Controls

The names below use the Radio Music front panel.

| Action | Result |
| --- | --- |
| Trigger/gate input | Fires the selected kick |
| Short button tap | Toggle 909 / 808 |
| Long button hold | Toggle page 1 / page 2 |
| LED3 on | 909 engine selected |
| LED2 on | 808 engine selected |
| Flashing reset LED | Page 2 active |

## Parameter Pages

| Page | Station knob | Station CV | Start knob | Start CV |
| --- | --- | --- | --- | --- |
| Page 1 | Tune | Tune CV | Decay | Accent |
| Page 2 | Engine macro 1 | Engine macro 1 CV | Engine macro 2 | Accent |

Current page 2 behavior:

- **808 page 2 Station**: strike bend amount. This is a fast one-shot bend at the start of the kick, not a slow pitch sweep.
- **808 page 2 Start**: attack/click character.
- **909 page 2**: reserved for engine-specific experiments.
- **Start CV** is always accent.

Page 2 settings are stored separately per engine and should not overwrite page 1 tune/decay.

## Pitch CV

Station CV is intended for pitch control from a 1V/oct source.

With an OXI One pitch CV output, useful direct pitch CV starts at OXI C2, because that is the first 0V note from the OXI.

| OXI note | OXI pitch CV | Module input result |
| --- | ---: | --- |
| C-1 | -3V | Below readable range |
| C0 | -2V | Below readable range |
| C1 | -1V | Below readable range |
| C2 | 0V | First useful pitch CV point |
| C3 | +1V | One octave above C2 CV |
| C4 | +2V | Two octaves above C2 CV |
| C5 | +3V | Three octaves above C2 CV |

The Radio Music analog input reads positive CV only, so negative pitch CV from a sequencer will not affect pitch unless externally offset.

Practical OXI setup:

- Set the OXI pitch CV track so the useful playing range starts around C2.
- Use Station knob as the manual offset for the kick's base pitch.
- Tune is intended to stay stable while accent and page 2 add performance character.

## Accent

Start CV is intended for sequencer velocity/accent CV.

For OXI One velocity CV:

- OXI velocity is 0-127.
- OXI default velocity is usually 75.
- Velocity 0 should be treated as unaccented.
- Higher velocity should add body, attack, and saturation without changing base tune or shortening decay.

Current behavior:

- **909 accent** uses a velocity-style response across body, pitch, and click energy.
- **808 accent** adds body, a short attack pulse, and high-velocity soft saturation.
- 808 accent does not change base tune or base decay.

## Decay And Gates

The 808 engine uses gate length to extend the amp envelope for longer booms while the Start knob remains the base decay control.

The 909 engine keeps its own separate body envelope and gate/body behavior. It is intentionally a different engine, not a reskinned 808.

## Source Map

- `MultiDrums.ino`: hardware setup, audio routing, global engine state.
- `functions.ino`: panel controls, button behavior, CV reads, trigger handling, settings persistence.
- `Brain.cpp` / `Brain.h`: kick synthesis engines.
- `HANDOFF.md`: development notes and future direction.

## Credits

Based on HoRa Music's RadioMusic-MultiDrums firmware:

https://github.com/HoRaMusic/RadioMusic-MultiDrums
