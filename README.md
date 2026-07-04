# RadioMusic Kick

Kick-only firmware for the Music Thing Modular Radio Music / Chord Organ hardware, based on HoRaMusic's RadioMusic-MultiDrums firmware.

This fork turns the module into a two-engine kick voice:

- an original analog-modelled kick engine from the upstream firmware
- a new 909-inspired kick engine added inside the existing `Brain` DSP path

The goal is immediate playability: one trigger input, two kick characters, linked tuning, and no menu diving beyond a short engine toggle and a long page toggle.

## Flashing

Flash the included firmware hex:

```text
MultiDrums.hex
```

The project is currently built for the Teensy 3.1 / 3.2 target used by the Radio Music / Chord Organ platform.

## Operation

Patch a trigger or gate into the normal trigger input. Audio comes from the normal module output.

| Action | Result |
| --- | --- |
| Trigger input | Fires the selected kick engine |
| Short button tap | Toggle original / 909 engine |
| Long button hold | Toggle parameter page |
| LED3 on | Original kick engine |
| LED2 on | 909 kick engine |
| Flashing LED | Page 2 is active |

## Controls

The two front-panel controls are live. The CV inputs are still summed with the pots, so the pot acts as an offset if CV is used.

| Page | Control 1 | Control 2 |
| --- | --- | --- |
| Page 1 | Tune | Decay |
| Page 2 | Pitch sweep | Punch / click / hardness |

Tune is linked between the original and 909 engines so switching engines keeps the kick in the same musical area. The engines still have separate decay, sweep, and punch behaviour.

## Engines

### Original

The original engine is the upstream bass drum voice, kept in the generated `Brain` DSP code path. It uses the existing modelled kick circuit behaviour and parameter controls.

### 909

The 909 engine is a separate fixed-point kick processor added beside the original kick in `Brain.cpp`. It uses:

- sine body oscillator
- fast pitch-drop envelope
- short noise click transient
- drive / soft clipping
- low-pass shaping through the existing kick filter style

It does not use samples and it does not add a separate Teensy Audio `AudioStream`; it runs inside the existing `Brain::update()` path so the audio routing stays stable on Teensy 3.x.

## Current Tuning Checkpoint

This build is the current "spot on" checkpoint:

- 909 pitch raised by ear until it matched the original kick well
- linked tune retained
- original kick tune mapped against the shared tune
- one branch, one flashable hex

## Build Notes

The firmware has been compiled with Arduino CLI / Teensyduino for:

```text
teensy:avr:teensy31
```

Most recent build size:

```text
Sketch uses 172776 bytes (65%) of program storage.
Global variables use 11824 bytes (18%) of dynamic memory.
```

## Credits

Original MultiDrums firmware by HoRaMusic:

https://github.com/HoRaMusic/RadioMusic-MultiDrums

This fork keeps the upstream kick engine intact and adds the kick-only workflow plus the 909-inspired engine for private research and performance use.
