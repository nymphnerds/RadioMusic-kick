#include <Wire.h>
#include <SPI.h>
#include <Audio.h>
#include <Bounce.h>
#include <EEPROM.h>
#include "Brain.h"

//#include "MIDI.h"

#define ADC_BITS 13
#define ADC_MAX_VAL 8192

#define BASS_DRUM 0
#define KICK_ENGINE_COUNT 4
#define KICK_ENGINE_PARAM_COUNT 4
#define KICK_PRIMARY_PARAM_COUNT 2
#define KICK_PAGE2_PARAM_COUNT 2
#define KICK_SETTINGS_SLOT_COUNT 6
#define KICK_ENGINE_808 0
#define KICK_ENGINE_909 1
#define KICK_ENGINE_HOT_909 2
#define KICK_ENGINE_DIRTY_808 3

#define CHORD_POT_PIN 9 // pin for Chord pot
#define CHORD_CV_PIN 6 // pin for Chord CV 
#define ROOT_POT_PIN 7 // pin for Root Note pot
#define ROOT_CV_PIN 8 // pin for Root Note CV
#define RESET_BUTTON 8 // Reset button 
#define RESET_LED 11 // Reset LED indicator 
#define RESET_CV 9 // Reset pulse in / out
#define LED0 6
#define LED1 5
#define LED2 4
#define LED3 3
#define FRAME_DIVIDER 1000
#define KICK_SETTINGS_EEPROM_BASE 64
#define KICK_SETTINGS_MAGIC_0 0x52
#define KICK_SETTINGS_MAGIC_1 0x4B
#define KICK_SETTINGS_VERSION 4
#define KICK_SETTINGS_SAVE_DELAY_FRAMES 6000
#define KICK_SETTINGS_PARAM_BYTES (KICK_ENGINE_COUNT * KICK_SETTINGS_SLOT_COUNT)

Bounce pushbutton1 = Bounce(RESET_BUTTON, 40);  // 10 ms debounce

boolean startCounter = false;
int param1 = 64;
int param2 = 64;
int instrumentsParams[1][KICK_ENGINE_PARAM_COUNT];
int kickEngineParams[KICK_ENGINE_COUNT][KICK_ENGINE_PARAM_COUNT];
int kickPage2Params[KICK_ENGINE_COUNT][KICK_PAGE2_PARAM_COUNT];
int frame;
int dividedFrame;
int param1CV;
int param2CV;
int shiftTimer = 0;
boolean activeTrig = false;
boolean modeChanged = false; 
boolean hasBeenSaved = false;
byte instrument;
int buttonState = 0;
boolean controlPage2 = false;
boolean hasBeenLoad = false;
int DIVIDED_EEPROM_Mode_Frame = 0;
int EEPROM_Mode_Frame = 0;
int kickEngine = KICK_ENGINE_909;
int kickLatchedAccent = 75;
boolean kickAccentCvSeen = false;
boolean kickControlsHoldUntilMove = false;
boolean kickParam1HoldUntilMove = false;
boolean kickParam2HoldUntilMove = false;
boolean kickControlsJustSwitched = false;
boolean kickSettingsDirty = false;
int kickSettingsSaveTimer = 0;
int kickSettingsObserved[KICK_ENGINE_COUNT][KICK_SETTINGS_SLOT_COUNT];
int lastKickParam1 = -1;
int lastKickParam2 = -1;

Brain                    voice1;
AudioMixer4              mixer1;         //xy=489,362
AudioOutputAnalog        dac1;           //xy=655,361
AudioConnection          patchCord1(voice1, 0, mixer1, 0);
AudioConnection          patchCord5(mixer1, dac1);
void setup() {
  mixer1.gain(0, 2.3);
  AudioMemory(12); //50
  voice1.begin();
  voice1.controlChange(43, kickEngine, 1);
  pinMode(RESET_BUTTON, INPUT);
  pinMode(RESET_CV, INPUT); 
  pinMode(RESET_LED, OUTPUT);
  pinMode(LED0,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  analogReadRes(ADC_BITS); 

}

void loop() {

  if (hasBeenLoad == false)
  {
       for( int j = 0; j < KICK_ENGINE_PARAM_COUNT; j++)
       {
        int value = EEPROM.read(j);
        if (value < 128)
        {
          instrumentsParams[BASS_DRUM][j] = value;
        }
        else
        {
          instrumentsParams[BASS_DRUM][j] = 0;
        }
       }
       for (int engine = 0; engine < KICK_ENGINE_COUNT; engine++)
       {
          for (int j = 0; j < KICK_ENGINE_PARAM_COUNT; j++)
          {
            kickEngineParams[engine][j] = instrumentsParams[BASS_DRUM][j];
          }
       }
       for (int engine = 0; engine < 2; engine++)
       {
          for (int j = 0; j < KICK_ENGINE_PARAM_COUNT; j++)
          {
            int value = EEPROM.read(30 + (engine * 4) + j);
            if (value < 128)
            {
              kickEngineParams[engine][j] = value;
            }
          }
          for (int j = 0; j < KICK_PAGE2_PARAM_COUNT; j++)
          {
            int value = EEPROM.read(20 + (engine * 2) + j);
            if (value < 128)
            {
              kickPage2Params[engine][j] = value;
            }
            else
            {
              kickPage2Params[engine][j] = instrumentsParams[BASS_DRUM][j + 2];
            }
          }
       }
       for (int j = 0; j < KICK_PRIMARY_PARAM_COUNT; j++)
       {
          instrumentsParams[BASS_DRUM][j] = kickEngineParams[kickEngine][j];
       }
       initializeKickSettingsPersistence();
       for (int j = 0; j < KICK_PRIMARY_PARAM_COUNT; j++)
       {
          instrumentsParams[BASS_DRUM][j] = kickEngineParams[kickEngine][j];
       }
       checkInterface();
       kickControlsHoldUntilMove = true;
       kickParam1HoldUntilMove = true;
       kickParam2HoldUntilMove = true;
       kickControlsJustSwitched = true;
       lastKickParam1 = param1;
       lastKickParam2 = param2;
       instrument = BASS_DRUM;
       hasBeenLoad = true;
  }
  
  checkButton();
  checkInterface();
  selectInstrument();
  setControlValues();
  saveKickSettingsIfNeeded();
  controlInstrumentParams();
  trigInstrument();
 
  /*
  Serial.println("AUDIO_CPU: ");
  Serial.println(AudioMemoryUsage());
  Serial.println(voice1.processorUsage());
  Serial.println("all=");
  Serial.println(AudioProcessorUsage());*/
}
