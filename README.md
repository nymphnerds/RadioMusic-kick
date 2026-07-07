# RadioMusic Kick

Kick-only firmware for the MTM Radio Music / Chord Organ eurorack hardware, based on HoRa Music's MultiDrums firmware.

This version removes the sample drum playback paths and keeps the modeled bass drum as the only playable instrument. The trigger input fires the kick voice, the button no longer cycles through snare, hi-hat, or sample modes, and the sample data files are not part of the sketch.

## Controls

The firmware keeps the original bass drum control layout:

| Page | Chord knob/CV | Root knob/CV |
| --- | --- | --- |
| Page 1 | Tune | Decay |
| Page 2 | Pitch envelope | Hardness |

Hold the button to toggle between page 1 and page 2. The reset LED flashes while page 2 is active.

LED3 indicates that the kick voice is selected. LED0, LED1, and LED2 are unused in this kick-only version.

## Source Notes

The modeled kick engine lives in `Brain.cpp` and `Brain.h`. The hardware control and trigger behavior lives in `MultiDrums.ino` and `functions.ino`.

The original sample files have been removed from the sketch so Arduino/Teensy builds do not compile the Linn drum sample payload.
