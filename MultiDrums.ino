#include <Wire.h>
#include <SPI.h>
#include <Audio.h>
#include <Bounce.h>
#include <EEPROM.h>
#include "Brain.h"

//#include "MIDI.h"

#define ADC_BITS 13
#define ADC_MAX_VAL 8192
#define POT_TOLERANCE 5

#define BASS_DRUM 0

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

Bounce pushbutton1 = Bounce(RESET_BUTTON, 40);  // 10 ms debounce

boolean startCounter = false;
int param1 = 64;
int param2 = 64;
int instrumentsParams[1][4]; // [kick][paramNumber]
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

Brain                    voice1;
AudioMixer4              mixer1;         //xy=489,362
AudioOutputAnalog        dac1;           //xy=655,361
AudioConnection          patchCord1(voice1, 0, mixer1, 0);
AudioConnection          patchCord5(mixer1, dac1);
// GUItool: end automatically generated code

void setup() {
  mixer1.gain(0, 2.3);
  AudioMemory(12); //50
  voice1.begin();
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
       for( int j = 0; j <4; j++)
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
       instrument = BASS_DRUM;
       hasBeenLoad = true;
  }
  
  checkButton(); // get button state and toggle the second kick control page
  checkInterface(); // get pot and cv values, mix these depending on the selected instrument and constrain these to 0-127 values
  selectInstrument(); // keep the modeled kick selected and display leds
  setControlValues();
  controlInstrumentParams(); // use the cv and pot value to control the voice1 paramete for the current instrument 
  trigInstrument(); // trig the modeled kick from the trigger input
 
  /*
  Serial.println("AUDIO_CPU: ");
  Serial.println(AudioMemoryUsage());
  Serial.println(voice1.processorUsage());
  Serial.println("all=");
  Serial.println(AudioProcessorUsage());*/
}
