
#ifndef BRAIN_H
#define BRAIN_H

#include <stdint.h>
#include <math.h>
#include "vultin.h"
#include "AudioStream.h"

typedef struct Kick__ctx_type_0 {
   fix16_t pre_x;
} Kick__ctx_type_0;

typedef Kick__ctx_type_0 Kick_change_type;

void Kick__ctx_type_0_init(Kick__ctx_type_0 &_output_);

void Kick_change_init(Kick__ctx_type_0 &_output_);

uint8_t Kick_change(Kick__ctx_type_0 &_ctx, fix16_t x);

fix16_t Kick_pitchToRate(fix16_t d);

fix16_t Kick_cvToRateMultiplier(fix16_t cvRaw);

fix16_t Kick_decayToFallingRate(fix16_t d);

typedef struct Kick__ctx_type_3 {
   uint8_t pre;
} Kick__ctx_type_3;

typedef Kick__ctx_type_3 Kick_edge_type;

void Kick__ctx_type_3_init(Kick__ctx_type_3 &_output_);

void Kick_edge_init(Kick__ctx_type_3 &_output_);

uint8_t Kick_edge(Kick__ctx_type_3 &_ctx, uint8_t x);

typedef struct Kick__ctx_type_4 {
   fix16_t envelope;
   Kick__ctx_type_3 _inst69;
} Kick__ctx_type_4;

typedef Kick__ctx_type_4 Kick_envelope_type;

void Kick__ctx_type_4_init(Kick__ctx_type_4 &_output_);

void Kick_envelope_init(Kick__ctx_type_4 &_output_);

fix16_t Kick_envelope(Kick__ctx_type_4 &_ctx, fix16_t dec, uint8_t g);

typedef struct Kick__ctx_type_5 {
   fix16_t z2;
   fix16_t z1;
} Kick__ctx_type_5;

typedef Kick__ctx_type_5 Kick_LP_type;

void Kick__ctx_type_5_init(Kick__ctx_type_5 &_output_);

void Kick_LP_init(Kick__ctx_type_5 &_output_);

fix16_t Kick_LP(Kick__ctx_type_5 &_ctx, fix16_t x);

typedef struct Kick__ctx_type_6 {
   int counter;
} Kick__ctx_type_6;

typedef Kick__ctx_type_6 Kick_divideFreq_type;

void Kick__ctx_type_6_init(Kick__ctx_type_6 &_output_);

void Kick_divideFreq_init(Kick__ctx_type_6 &_output_);

uint8_t Kick_divideFreq(Kick__ctx_type_6 &_ctx, int divider);

typedef struct Kick__ctx_type_7 {
   fix16_t rate;
   fix16_t phase;
   fix16_t env;
   Kick__ctx_type_0 _inst84;
   Kick__ctx_type_0 _inst83;
   Kick__ctx_type_4 _inst82;
   Kick__ctx_type_6 _inst80;
   Kick__ctx_type_3 _inst77;
} Kick__ctx_type_7;

typedef Kick__ctx_type_7 Kick_customBridgeT_type;

void Kick__ctx_type_7_init(Kick__ctx_type_7 &_output_);

void Kick_customBridgeT_init(Kick__ctx_type_7 &_output_);

fix16_t Kick_customBridgeT(Kick__ctx_type_7 &_ctx, fix16_t tune, fix16_t tuneCV, uint8_t g, fix16_t envInt, fix16_t dec, fix16_t hardness);

typedef struct Kick__ctx_type_8 {
   fix16_t tune;
   fix16_t tuneCV;
   fix16_t pitchEnvInt;
   fix16_t hardness;
   fix16_t accentDrive;
   fix16_t clickEnv;
   fix16_t highAccentSat;
   fix16_t accentAmount;
   fix16_t strikeEnv;
   fix16_t attackEnv;
   int clickNoise;
   uint8_t gate;
   fix16_t decay;
   Kick__ctx_type_5 _inst92;
   Kick__ctx_type_4 _inst91;
   Kick__ctx_type_7 _inst90;
} Kick__ctx_type_8;

typedef Kick__ctx_type_8 Kick_process_type;

void Kick__ctx_type_8_init(Kick__ctx_type_8 &_output_);

void Kick_process_init(Kick__ctx_type_8 &_output_);

fix16_t Kick_process(Kick__ctx_type_8 &_ctx, fix16_t gateI, fix16_t tuneI, fix16_t tuneCVI, fix16_t decayI, fix16_t pitchEnvIntI, fix16_t hardnessI, fix16_t accentDriveI);

typedef struct Kick909__ctx_type_0 {
   uint8_t gate;
   fix16_t phase;
   fix16_t bodyEnv;
   fix16_t pitchEnv;
   fix16_t clickEnv;
   fix16_t bodyRate;
   fix16_t tune;
   fix16_t tuneCV;
   int noise;
   int gateSamples;
   Kick__ctx_type_3 _edge;
   Kick__ctx_type_5 _lp;
} Kick909__ctx_type_0;

typedef Kick909__ctx_type_0 Kick909_process_type;

void Kick909__ctx_type_0_init(Kick909__ctx_type_0 &_output_);

void Kick909_process_init(Kick909__ctx_type_0 &_output_);

fix16_t Kick909_process(Kick909__ctx_type_0 &_ctx, fix16_t gateI, fix16_t tuneI, fix16_t tuneCVI, fix16_t decayI, fix16_t pitchEnvIntI, fix16_t hardnessI, fix16_t accentI);

typedef struct Brain__ctx_type_0 {
   fix16_t kickParam[7];
   fix16_t voice1;
   int kickEngine;
   fix16_t kick909Accent;
   Kick909__ctx_type_0 hot909;
   Kick__ctx_type_8 dirty808;
   Kick909__ctx_type_0 kick909;
   Kick__ctx_type_8 kick808;
} Brain__ctx_type_0;

typedef Brain__ctx_type_0 Brain_process_type;

void Brain__ctx_type_0_init(Brain__ctx_type_0 &_output_);

void Brain_process_init(Brain__ctx_type_0 &_output_);

fix16_t Brain_process(Brain__ctx_type_0 &_ctx, fix16_t input);

typedef Brain__ctx_type_0 Brain_noteOn_type;

void Brain_noteOn_init(Brain__ctx_type_0 &_output_);

void Brain_noteOn(Brain__ctx_type_0 &_ctx, int note, int velocity, int channel);

typedef Brain__ctx_type_0 Brain_noteOff_type;

void Brain_noteOff_init(Brain__ctx_type_0 &_output_);

void Brain_noteOff(Brain__ctx_type_0 &_ctx, int note, int channel);

typedef Brain__ctx_type_0 Brain_controlChange_type;

void Brain_controlChange_init(Brain__ctx_type_0 &_output_);

void Brain_controlChange(Brain__ctx_type_0 &_ctx, int control, int value, int channel);

typedef Brain__ctx_type_0 Brain_default_type;

void Brain_default_init(Brain__ctx_type_0 &_output_);

void Brain_default(Brain__ctx_type_0 &_ctx);



class Brain : public AudioStream
{
public:
  Brain(void) : AudioStream(0, NULL)
  {
     Brain_process_init(data);
  }

  void begin() {
    Brain_default(data);
  }

  // Handles note on events
  void noteOn(int note, int velocity, int channel){
    // If the velocity is larger than zero, means that is turning on
    if(velocity) Brain_noteOn(data, note, velocity, channel);
    else         Brain_noteOff(data, note, channel);
  }

  // Handles note off events
  void noteOff(int note, int velocity, int channel) {
    Brain_noteOff(data, note, channel);

  }

  // Handles control change events
  void controlChange(int control, int value, int channel) {
    Brain_controlChange(data, control, value, channel);
  }

  virtual void update(void);

private:
  Brain_process_type data;

};

#endif // BRAIN_H
