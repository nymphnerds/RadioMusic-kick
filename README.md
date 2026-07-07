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

## Tuning And Keyboard Range

Station CV is intended for pitch control from a 1V/oct source. With an OXI One pitch CV output, the useful direct range starts at C2 because that is the first 0V note from the OXI.

| OXI note | OXI pitch CV | Module input result |
| --- | ---: | --- |
| C-1 | -3V | Below readable range |
| C0 | -2V | Below readable range |
| C1 | -1V | Below readable range |
| C2 | 0V | First useful pitch CV point |
| C3 | +1V | One octave above C2 CV |
| C4 | +2V | Two octaves above C2 CV |
| C5 | +3V | Three octaves above C2 CV |

The Radio Music analog input reads positive CV only, so notes below OXI C2 need an external positive offset if you want them to affect pitch.

In the current build, the Station knob is the manual tune offset and Station CV is added inside the kick engine as pitch CV. For a practical starting point, set the Station knob fully left or low, then bring the OXI note range up from C2 until the kick speaks clearly.

## Playing Notes

- Page 1 is the main performance page: Tune, Tune CV, Decay.
- Page 2 is tone shaping: pitch envelope and click.
- Start CV is reserved in this build.
- Longer trigger gates extend the boom naturally.
- Very long gates are supported; the output stage includes final saturation to stop digital wrap distortion.
- The current known-good branch is `boombaseline`.

The current build artifact is `MultiDrums.hex`.

## Source

- `MultiDrums.ino`: hardware setup and audio output gain
- `functions.ino`: panel controls, page switching, trigger handling
- `Brain.cpp` / `Brain.h`: kick synthesis engine and output saturation
