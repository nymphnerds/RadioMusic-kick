# RadioMusic Kick

Kick-only firmware for Music Thing Modular Radio Music / Radio Station hardware, based on HoRa Music's MultiDrums firmware.

This branch turns the module into a dedicated synthesized bass drum voice. It is not a sample drum firmware and it is not a general multi-drum mode.

## Engines

RadioMusic Kick currently has two switchable kick engines:

- **808**: the main boomy kick voice, tuned for long low-end decay and musical pitch control.
- **909**: a separate punchier synthesized kick engine imported from the earlier `RadioMusic-kick` fork.

The long-term direction is four separate kick synthesis engines, selected from the button, with each engine kept clean and independent.

## Flashing

Flash:

```text
MultiDrums.hex
```

Current local build:

```text
Target: teensy:avr:teensy31
Version: v1.0.4
SHA256: 64d48d6004de5d6b01f0fec354a243b29d67b56cae90c09624c71eff74d254ac
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
| Page 2 | Pitch envelope | Pitch envelope CV | Click | Accent |

Notes:

- Start CV is always accent.
- Page 2 Start is called **click** in this firmware.
- The 909 engine is the default engine and has the strongest current velocity/accent response.
- The 808 engine has long decay behavior, 1V/oct-style pitch control, and a linear internal strike accent.

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
- The 808 engine is the reference for the current 1V/oct-style scaling.
- The 909 engine is being tuned to land in a similar pitch range while keeping its own character.

## Accent

Start CV is intended for sequencer velocity/accent CV.

For OXI One velocity CV:

- OXI velocity is 0-127.
- OXI default velocity is usually 75.
- Velocity 0 should be treated as unaccented.
- Higher velocity should add accent without changing the basic decay setting.

Current behavior:

- 909 accent uses a velocity-style response across body, pitch, and click energy.
- 808 accent is a simpler linear internal strike control. It scales the amp/pitch/hardness strike at the trigger edge.
- 808 accent does not use the rejected post-drive/click/trim experiments.
- Accent should add performance character without killing the kick decay.

Accent is still a tuning area, but the current 808 path is intentionally simple and stable.

## Decay And Gates

The 808 engine uses gate length to extend the amp envelope for longer booms while the Start knob remains the base decay control.

The 909 engine currently keeps the earlier fork's 909-style body envelope and gate/body behavior. It is intentionally a separate engine, not just another skin over the 808 path.

## Source Map

- `MultiDrums.ino`: hardware setup, audio routing, global engine state.
- `functions.ino`: panel controls, button behavior, CV reads, trigger handling.
- `Brain.cpp` / `Brain.h`: kick synthesis engines and output protection.
- `HANDOFF.md`: development notes and future direction.

## Credits

Based on HoRa Music's RadioMusic-MultiDrums firmware:

https://github.com/HoRaMusic/RadioMusic-MultiDrums
