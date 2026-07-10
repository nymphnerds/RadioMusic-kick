// Persistent storage for the two kick-engine parameter banks.
//
// This intentionally uses a separate EEPROM region so the original
// MultiDrums settings remain available for one-time migration.

#define KICK_SETTINGS_EEPROM_BASE 32
#define KICK_SETTINGS_MAGIC_0 0x52  // 'R'
#define KICK_SETTINGS_MAGIC_1 0x4B  // 'K'
#define KICK_SETTINGS_VERSION 1
#define KICK_SETTINGS_SAVE_DELAY_MS 2000

#define KICK_SETTINGS_MAGIC_0_ADDR (KICK_SETTINGS_EEPROM_BASE + 0)
#define KICK_SETTINGS_MAGIC_1_ADDR (KICK_SETTINGS_EEPROM_BASE + 1)
#define KICK_SETTINGS_VERSION_ADDR (KICK_SETTINGS_EEPROM_BASE + 2)
#define KICK_SETTINGS_PARAMS_ADDR  (KICK_SETTINGS_EEPROM_BASE + 3)
#define KICK_SETTINGS_CHECKSUM_ADDR (KICK_SETTINGS_PARAMS_ADDR + 8)

elapsedMillis kickSettingsSaveTimer = 0;
boolean kickSettingsDirty = false;
int kickSettingsObserved[2][4];

void writeKickSettingByte(int address, byte value)
{
  if (EEPROM.read(address) != value)
  {
    EEPROM.write(address, value);
  }
}

byte calculateKickSettingsChecksum()
{
  byte checksum = 0xA5 ^ KICK_SETTINGS_VERSION;

  for (int engine = 0; engine < 2; engine++)
  {
    for (int parameter = 0; parameter < 4; parameter++)
    {
      byte value = (byte)constrain(kickEngineParams[engine][parameter], 0, 127);
      checksum = (byte)((checksum << 1) | (checksum >> 7));
      checksum ^= value;
    }
  }

  return checksum;
}

boolean loadKickSettingsFromEEPROM()
{
  if (EEPROM.read(KICK_SETTINGS_MAGIC_0_ADDR) != KICK_SETTINGS_MAGIC_0 ||
      EEPROM.read(KICK_SETTINGS_MAGIC_1_ADDR) != KICK_SETTINGS_MAGIC_1 ||
      EEPROM.read(KICK_SETTINGS_VERSION_ADDR) != KICK_SETTINGS_VERSION)
  {
    return false;
  }

  int loadedParams[2][4];
  byte checksum = 0xA5 ^ KICK_SETTINGS_VERSION;
  int address = KICK_SETTINGS_PARAMS_ADDR;

  for (int engine = 0; engine < 2; engine++)
  {
    for (int parameter = 0; parameter < 4; parameter++)
    {
      int value = EEPROM.read(address++);
      if (value < 0 || value > 127)
      {
        return false;
      }

      loadedParams[engine][parameter] = value;
      checksum = (byte)((checksum << 1) | (checksum >> 7));
      checksum ^= (byte)value;
    }
  }

  if (EEPROM.read(KICK_SETTINGS_CHECKSUM_ADDR) != checksum)
  {
    return false;
  }

  for (int engine = 0; engine < 2; engine++)
  {
    for (int parameter = 0; parameter < 4; parameter++)
    {
      kickEngineParams[engine][parameter] = loadedParams[engine][parameter];
    }
  }

  return true;
}

void saveKickSettingsToEEPROM()
{
  // Invalidate the block first so a power loss during the write cannot make
  // a partially updated block look valid on the next boot.
  writeKickSettingByte(KICK_SETTINGS_MAGIC_0_ADDR, 0);

  int address = KICK_SETTINGS_PARAMS_ADDR;
  for (int engine = 0; engine < 2; engine++)
  {
    for (int parameter = 0; parameter < 4; parameter++)
    {
      byte value = (byte)constrain(kickEngineParams[engine][parameter], 0, 127);
      writeKickSettingByte(address++, value);
    }
  }

  writeKickSettingByte(KICK_SETTINGS_CHECKSUM_ADDR, calculateKickSettingsChecksum());
  writeKickSettingByte(KICK_SETTINGS_VERSION_ADDR, KICK_SETTINGS_VERSION);
  writeKickSettingByte(KICK_SETTINGS_MAGIC_1_ADDR, KICK_SETTINGS_MAGIC_1);
  writeKickSettingByte(KICK_SETTINGS_MAGIC_0_ADDR, KICK_SETTINGS_MAGIC_0);
}

void captureObservedKickSettings()
{
  for (int engine = 0; engine < 2; engine++)
  {
    for (int parameter = 0; parameter < 4; parameter++)
    {
      kickSettingsObserved[engine][parameter] = kickEngineParams[engine][parameter];
    }
  }
}

boolean kickSettingsHaveChanged()
{
  for (int engine = 0; engine < 2; engine++)
  {
    for (int parameter = 0; parameter < 4; parameter++)
    {
      if (kickSettingsObserved[engine][parameter] != kickEngineParams[engine][parameter])
      {
        return true;
      }
    }
  }

  return false;
}

void initializeKickSettingsPersistence()
{
  // When no versioned kick block exists, kickEngineParams already contains
  // the old shared bass-drum settings loaded by MultiDrums.ino. Save that as
  // the initial value for both engines, preserving the existing installation.
  if (!loadKickSettingsFromEEPROM())
  {
    saveKickSettingsToEEPROM();
  }

  captureObservedKickSettings();
  kickSettingsDirty = false;
  kickSettingsSaveTimer = 0;
}

void serviceKickSettingsPersistence()
{
  if (kickSettingsHaveChanged())
  {
    captureObservedKickSettings();
    kickSettingsDirty = true;
    kickSettingsSaveTimer = 0;
  }

  if (kickSettingsDirty && kickSettingsSaveTimer >= KICK_SETTINGS_SAVE_DELAY_MS)
  {
    saveKickSettingsToEEPROM();
    kickSettingsDirty = false;
  }
}
