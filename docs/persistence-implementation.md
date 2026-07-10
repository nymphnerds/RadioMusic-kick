# Kick Persistence Implementation

The persistence fix deliberately leaves the original MultiDrums EEPROM layout untouched.

## New EEPROM block

Base address: `32`

| Offset | Value |
|---:|---|
| 0 | Magic byte `R` |
| 1 | Magic byte `K` |
| 2 | Format version |
| 3–6 | 808 Tune, Decay, Pitch Sweep, Hardness |
| 7–10 | 909 Tune, Decay, Pitch Sweep, Hardness |
| 11 | Checksum |

## Migration

On the first boot with this firmware:

1. The old shared bass-drum settings are loaded normally.
2. Both kick-engine banks are initialised from those values.
3. If no valid versioned block exists, that initial state is written to the new block.
4. Later boots load the two banks separately from the versioned block.

## Write policy

The firmware observes both parameter banks and starts a two-second timer whenever either bank changes. EEPROM is written only after the values remain unchanged for two seconds.

Before updating the block, the first magic byte is invalidated. It is restored only after the parameter bytes, checksum and version have been written.

## Boot control pickup

After restoring settings, the current physical pot/CV readings are sampled and the existing hold-until-move behaviour is armed. This prevents the first normal control-loop pass from immediately replacing the restored values.

## Scope

This change does not alter synthesis, accent mapping, gate processing, button timing, engine switching or LED behaviour.
