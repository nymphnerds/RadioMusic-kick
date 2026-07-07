# RadioMusic Kick

Kick-only firmware for the MTM Radio Music hardware, based on HoRa Music's MultiDrums firmware.

This branch turns the module into a dedicated synthesized bass drum voice. The sample drum paths are removed from the control flow, the trigger input fires only the kick engine, and the output is tuned for long, boomy 808-style decay without digital wrap at the audio output.

## Highlights

- One dedicated synthesized kick voice
- Direct knob response with no pickup/safety-jump behavior
- Station CV routed as tune CV into the kick engine
- Start knob controls natural decay
- Trigger gate length extends the amp decay for longer booms
- Output headroom and final saturation to avoid harsh digital wrap
- Button switches between two compact control pages

## Controls

The names below assume the Radio Music front panel.

| Page | Station knob | Station CV | Start knob | Start CV |
| --- | --- | --- | --- | --- |
| Page 1 | Tune | Tune CV | Decay | Reserved |
| Page 2 | Pitch envelope | Pitch envelope CV | Click | Click CV |

Hold the button to toggle between pages. The reset LED flashes on Page 2.

The trigger input fires the kick. Longer trigger gates hold the amp envelope open for a longer 808-style boom while the Start knob remains the base decay control.

## Notes

The Teensy analog input reads positive CV only. On an OXI One pitch CV output, C2 is the first 0V point, so notes below that may not produce useful pitch CV on this hardware without external offset.

The current build artifact is `MultiDrums.hex`.

## Source

- `MultiDrums.ino`: hardware setup and audio output gain
- `functions.ino`: panel controls, page switching, trigger handling
- `Brain.cpp` / `Brain.h`: kick synthesis engine and output saturation
