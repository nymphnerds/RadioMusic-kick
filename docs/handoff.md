# RadioMusic Kick Handoff

Date: 2026-07-06

This file records the current state of the kick-only Radio Music / Radio Station firmware so future tuning can continue without rediscovering the same hardware behavior.

## Current Status

The current firmware is a two-engine kick module:

- 808 engine, based on the upstream kick voice
- custom 909-inspired engine, currently considered the keeper sound

The current flashable file is:

```text
/home/nymph/RadioMusic-MultiDrums/MultiDrums.hex
SHA256: b2ea1718b714e289307cbd330f206c0d0c065abfe4ab7c2d2b8ad692f14489d0
```

Current branch:

```text
master
```

Do not create extra branches or extra hex names for normal tuning iterations. Keep the project to one branch and one flashable `MultiDrums.hex`.

## User-Facing Hardware Names

Use Radio Station / Radio Music wording when discussing the panel.

Internal source names still come from the old platform code:

| User-facing name | Source name |
| --- | --- |
| Station knob | `CHORD_POT_PIN` |
| Station CV | `CHORD_CV_PIN` |
| Start knob | `ROOT_POT_PIN` |
| Start CV | `ROOT_CV_PIN` |
| Trigger/gate input | `RESET_CV` |
| Button | `RESET_BUTTON` |

Important: the Start CV does not change function by page. It is the accent / velocity input. The knobs change meaning by page.

## Controls

The firmware forces `instrument = BASS_DRUM`.

Button behavior:

- short tap toggles 808 / 909 engine
- long hold toggles page 1 / page 2
- LED3 means 808 engine
- LED2 means 909 engine
- flashing reset LED means page 2

Knob/CV behavior:

| Page | Station knob/CV | Start knob | Start CV |
| --- | --- | --- | --- |
| Page 1 | Tune | Decay | Accent / velocity |
| Page 2 | Pitch sweep | Punch / hardness | Accent / velocity |

Tune is linked between engines:

```cpp
kickEngineParams[KICK_ENGINE_ORIGINAL][0] = kickEngineParams[kickEngine][0];
kickEngineParams[KICK_ENGINE_909][0] = kickEngineParams[kickEngine][0];
```

The 808 engine tune is internally matched with:

```cpp
int matchedOriginalTune = (kickParams[0] - 27) / 2;
```

## Accent / Velocity

Start CV is read as a `0..127` accent/velocity signal and latched at trigger time.

Relevant state:

```cpp
int kickAccent;
int kickLatchedAccent = 75;
boolean kickAccentCvSeen = false;
```

If no real Start CV is seen after boot, accent defaults to `75` so the module produces a normal hit when unpatched.

### 808 Curve

Function:

```cpp
int originalKickAccentCurve(int accent)
{
    accent = constrain(accent, 0, 127);
    if (accent <= 75)
    {
        return (accent * 40) / 75;
    }
    return 40 + (((accent - 75) * 87) / 52);
}
```

Calibration:

```text
external 0   -> internal 0
external 75  -> internal 40
external 127 -> internal 127
```

Reason: by ear, the 808 kick sounded good around old internal velocity `40`; that should now happen at OXI velocity `75`.

Additional 808 accent shaping in `controlInstrumentParams()`:

```cpp
accentPitch = accentAboveNormal / 3;
accentHardness = accentAboveNormal / 4;
accentClick = accentBelowNormal;
gateDecay += (kickGateDecayBoost * 2) + (accentAboveNormal / 8) - (accentBelowNormal / 2);
```

Low-velocity click is a separate transient in `Brain.cpp`, controlled by CC 45. Do not reintroduce low-velocity hardness when the requested change is "more click".

### 909 Curve

Function:

```cpp
int kick909AccentCurve(int accent)
{
    accent = constrain(accent, 0, 127);
    if (accent <= 75)
    {
        return (accent * 30) / 75;
    }
    return 30 + (((accent - 75) * 30) / 52);
}
```

Calibration:

```text
external 0   -> internal 0
external 75  -> internal 30
external 127 -> internal 60
```

Reason: by ear, the 909 sounded right around old internal `30`, and old internal `60` should be the full accent rather than letting the external range overcook the voice.

The 909 accent is applied inside `Kick909_process()` to body, pitch, and click envelopes. Avoid post-output gain tricks for the 909; earlier attempts distorted or broke up too quickly.

## Gate Length / Decay

Gate-length decay extension begins after about 50 ms:

```cpp
if (activeTrig && kickGateTimer > 50)
{
    kickGateDecayBoost = constrain((int)(kickGateTimer - 50) / 3, 0, 127);
}
```

Current scaling:

- 808: `kickGateDecayBoost * 2`
- 909: `kickGateDecayBoost * 3`

Goal:

- normal short OXI trigger should use the decay knob setting
- longer gates should extend decay
- Start CV remains accent, not gate length

## Important Code Locations

Main tuning files:

- `functions.ino`
  - `originalKickAccentCurve()`
  - `kick909AccentCurve()`
  - `checkInterface()`
  - `controlInstrumentParams()`
  - `trigInstrument()`
- `MultiDrums.ino`
  - globals for engine, accent latch, gate timer
  - initial loading into `kickEngineParams`
- `Brain.cpp`
  - 808 generated `Kick_process()`
  - custom `Kick909_process()`
  - 808 output velocity scaling
  - 808 separate low-velocity click transient
  - CC 43 engine mode, CC 44 accent, CC 45 808 click
- `Brain.h`
  - `Kick909__ctx_type_0`
  - added `kickAccent`, `kick808ClickAmount`, `kick808ClickEnv`, `kick808ClickNoise`

## Build Command

The current working build flow copies the Arduino sketch files to a temporary sketch folder and builds from Windows Arduino CLI:

```bash
cp -a /home/nymph/RadioMusic-MultiDrums/*.ino /home/nymph/RadioMusic-MultiDrums/*.cpp /home/nymph/RadioMusic-MultiDrums/*.h /home/nymph/RadioMusic-MultiDrums/*.c /tmp/rm909/MultiDrums/
powershell.exe -NoProfile -Command "& 'C:\Users\babyj\AppData\Local\Programs\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe' compile --fqbn teensy:avr:teensy31 --build-path '//wsl.localhost/NymphsCore_Lite/tmp/rm909/build' '//wsl.localhost/NymphsCore_Lite/tmp/rm909/MultiDrums'"
cp -a /tmp/rm909/build/MultiDrums.ino.hex /home/nymph/RadioMusic-MultiDrums/MultiDrums.hex
sha256sum /home/nymph/RadioMusic-MultiDrums/MultiDrums.hex
find /home/nymph/RadioMusic-MultiDrums -maxdepth 2 -type f -name '*.hex' -print
```

Expected check after copying:

```text
/home/nymph/RadioMusic-MultiDrums/MultiDrums.hex
```

Only one project hex should be present.

## Future Tuning Notes

Preserve unless explicitly changing:

- 909 core sound is the keeper
- 909 accent full range should top out around internal `60`
- 808 external `75` should stay near old internal `40`
- low-velocity 808 click should remain a separate transient, not hardness
- normal short trigger should follow the decay knob
- long gate should extend decay

Good next tuning moves:

- If 808 high velocity still feels pitch-shifted, reduce `accentPitch = accentAboveNormal / 3` or make the pitch envelope itself decay faster.
- If 808 low velocity needs more click, raise the CC 45 click transient amount or decay shape in `Brain.cpp`, not `accentHardness`.
- If 909 accents are too flat after the current remap, raise only the top of `kick909AccentCurve()` from `60` to a small higher value such as `65`.
- If gate length feels too dramatic, lower the `* 2` or `* 3` gate scaling rather than touching the decay knob mapping.

Avoid:

- adding alternate hex filenames
- creating throwaway branches for tuning
- changing 909 core synthesis while testing 808 accent
- treating Start CV as page-dependent
