# Kick Settings Persistence Test

This test covers only the separate 808/909 parameter-bank persistence change.

## Before flashing

Keep the current known-good `MultiDrums.hex` so the module can be restored immediately.

## Migration test

1. Flash a build from `fix/kick-settings-persistence`.
2. Power the module once with the existing EEPROM contents.
3. Confirm both engines initially inherit the previous shared kick settings.
4. Do not expect the old EEPROM region to be erased; it remains available as the migration fallback.

## Separate-bank test

1. Select 808.
2. Set an obviously short decay and low pitch sweep.
3. Select 909.
4. Set an obviously long decay and high pitch sweep.
5. Leave the controls untouched for at least two seconds so the delayed EEPROM save completes.
6. Power the module off.
7. Power it on again.
8. Confirm the restored settings are not immediately replaced by the physical knob positions.
9. Select 808 and verify its short-decay/low-sweep settings.
10. Select 909 and verify its long-decay/high-sweep settings.

## Shared-tune test

1. Change Tune on either engine.
2. Wait at least two seconds.
3. Power-cycle the module.
4. Confirm both engines retain the same linked Tune value.

## Save-delay test

1. Move a parameter continuously for several seconds.
2. Confirm the module remains responsive while moving it.
3. Stop moving it and wait at least two seconds.
4. Power-cycle and confirm the final stable value was saved.

## Regression checks

Confirm that this branch does not change:

- 808 sound,
- 909 sound,
- accent response,
- trigger/gate response,
- engine-switch button behaviour,
- page-switch button behaviour,
- LED indications.

## Expected EEPROM behaviour

- Legacy MultiDrums data remains at its original addresses.
- The new kick-only block begins at EEPROM address 32.
- The block stores eight values: four for 808 and four for 909.
- A version byte, magic bytes and checksum reject invalid data.
- Writes occur only after the parameter banks remain unchanged for two seconds.
