
#include "Brain.h"

static const float KICK_ADC_MAX = 8192.0f;
static const float KICK_ADC_REF = 3.3f;
static const float KICK_TUNE_CV_SCALE = 1.0f;
static const fix16_t KICK_GATE_DECAY_BOOST = 0x800000 /* 128.000000 */;
static const fix16_t KICK_ACCENT_CV_FULL_SCALE = 0x20000000 /* 8192.000000 */;
static const fix16_t KICK_ACCENT_DRIVE = 0x14000 /* 1.250000 */;

void Kick__ctx_type_0_init(Kick__ctx_type_0 &_output_){
   Kick__ctx_type_0 _ctx;
   _ctx.pre_x = 0x0 /* 0.000000 */;
   _output_ = _ctx;
   return ;
}

void Kick_change_init(Kick__ctx_type_0 &_output_){
   Kick__ctx_type_0_init(_output_);
   return ;
}

uint8_t Kick_change(Kick__ctx_type_0 &_ctx, fix16_t x){
   uint8_t v;
   v = (_ctx.pre_x != x);
   _ctx.pre_x = x;
   return v;
}

fix16_t Kick_pitchToRate(fix16_t d){
   return fix_mul(0x2 /* 0.000045 */,d);
}

fix16_t Kick_cvToRateMultiplier(fix16_t cvRaw){
   float cvOctaves = (fix_to_float(cvRaw) / KICK_ADC_MAX) * KICK_ADC_REF * KICK_TUNE_CV_SCALE;
   return float_to_fix(powf(2.0f, cvOctaves));
}

fix16_t Kick_decayToFallingRate(fix16_t d){
   return fix_div(0x10000 /* 1.000000 */,fix_mul(d,0x15b3e7c /* 347.244094 */));
}

fix16_t Kick_accentDrive(fix16_t x, fix16_t accentCV, fix16_t envelope){
   if(accentCV <= 0x0 /* 0.000000 */){
      return x;
   }
   fix16_t accent;
   accent = fix_div(accentCV,KICK_ACCENT_CV_FULL_SCALE);
   fix16_t amount;
   amount = fix_mul(fix_mul(accent,envelope),KICK_ACCENT_DRIVE);
   fix16_t x2;
   x2 = fix_mul(x,x);
   fix16_t x3;
   x3 = fix_mul(x2,x);
   return (x + fix_mul(amount,(x + (- x3))));
}

void Kick__ctx_type_3_init(Kick__ctx_type_3 &_output_){
   Kick__ctx_type_3 _ctx;
   _ctx.pre = 0;
   _output_ = _ctx;
   return ;
}

void Kick_edge_init(Kick__ctx_type_3 &_output_){
   Kick__ctx_type_3_init(_output_);
   return ;
}

uint8_t Kick_edge(Kick__ctx_type_3 &_ctx, uint8_t x){
   uint8_t ret;
   ret = (x && bool_not(_ctx.pre));
   _ctx.pre = x;
   return ret;
}

void Kick__ctx_type_4_init(Kick__ctx_type_4 &_output_){
   Kick__ctx_type_4 _ctx;
   _ctx.envelope = 0x0 /* 0.000000 */;
   Kick__ctx_type_3_init(_ctx._inst69);
   _output_ = _ctx;
   return ;
}

void Kick_envelope_init(Kick__ctx_type_4 &_output_){
   Kick__ctx_type_4_init(_output_);
   return ;
}

fix16_t Kick_envelope(Kick__ctx_type_4 &_ctx, fix16_t dec, uint8_t g){
   fix16_t fallingRate;
   fallingRate = Kick_decayToFallingRate(((dec >> 1) + 0x50000 /* 5.000000 */));
   uint8_t _cond_93;
   _cond_93 = Kick_edge(_ctx._inst69,g);
   if(_cond_93){
      _ctx.envelope = 0x10000 /* 1.000000 */;
   }
   uint8_t _cond_94;
   _cond_94 = (_ctx.envelope > 0x0 /* 0.000000 */);
   if(_cond_94){
      _ctx.envelope = (_ctx.envelope + (- fallingRate));
   }
   uint8_t _cond_95;
   _cond_95 = (_ctx.envelope <= 0x0 /* 0.000000 */);
   if(_cond_95){
      _ctx.envelope = 0x0 /* 0.000000 */;
   }
   return _ctx.envelope;
}

fix16_t Kick_ampEnvelope(Kick__ctx_type_4 &_ctx, fix16_t dec, uint8_t g){
   fix16_t activeDecay;
   activeDecay = g ? (dec + KICK_GATE_DECAY_BOOST) : dec;
   return Kick_envelope(_ctx,activeDecay,g);
}

void Kick__ctx_type_5_init(Kick__ctx_type_5 &_output_){
   Kick__ctx_type_5 _ctx;
   _ctx.z2 = 0x0 /* 0.000000 */;
   _ctx.z1 = 0x0 /* 0.000000 */;
   _output_ = _ctx;
   return ;
}

void Kick_LP_init(Kick__ctx_type_5 &_output_){
   Kick__ctx_type_5_init(_output_);
   return ;
}

fix16_t Kick_LP(Kick__ctx_type_5 &_ctx, fix16_t x){
   fix16_t g;
   g = 0x3333 /* 0.200000 */;
   fix16_t R;
   R = 0x10000 /* 1.000000 */;
   fix16_t inv_den;
   inv_den = fix_div(0x10000 /* 1.000000 */,(0x10000 /* 1.000000 */ + fix_mul(g,g)));
   fix16_t high;
   high = fix_mul((x + (- fix_mul(((R << 1) + g),_ctx.z1)) + (- _ctx.z2)),inv_den);
   fix16_t band;
   band = (fix_mul(g,high) + _ctx.z1);
   fix16_t low;
   low = (fix_mul(g,band) + _ctx.z2);
   _ctx.z1 = (fix_mul(g,high) + band);
   _ctx.z2 = (fix_mul(g,band) + low);
   return low;
}

void Kick__ctx_type_6_init(Kick__ctx_type_6 &_output_){
   Kick__ctx_type_6 _ctx;
   _ctx.counter = 0;
   _output_ = _ctx;
   return ;
}

void Kick_divideFreq_init(Kick__ctx_type_6 &_output_){
   Kick__ctx_type_6_init(_output_);
   return ;
}

uint8_t Kick_divideFreq(Kick__ctx_type_6 &_ctx, int divider){
   _ctx.counter = (_ctx.counter + 1);
   uint8_t tick;
   tick = 0;
   uint8_t _cond_96;
   _cond_96 = ((_ctx.counter % divider) == 0);
   if(_cond_96){
      _ctx.counter = divider;
      tick = 1;
   }
   return tick;
}

void Kick__ctx_type_7_init(Kick__ctx_type_7 &_output_){
   Kick__ctx_type_7 _ctx;
   _ctx.rate = 0x0 /* 0.000000 */;
   _ctx.phase = 0x0 /* 0.000000 */;
   _ctx.env = 0x0 /* 0.000000 */;
   Kick__ctx_type_0_init(_ctx._inst84);
   Kick__ctx_type_0_init(_ctx._inst83);
   Kick__ctx_type_4_init(_ctx._inst82);
   Kick__ctx_type_6_init(_ctx._inst80);
   Kick__ctx_type_3_init(_ctx._inst77);
   _output_ = _ctx;
   return ;
}

void Kick_customBridgeT_init(Kick__ctx_type_7 &_output_){
   Kick__ctx_type_7_init(_output_);
   return ;
}

fix16_t Kick_customBridgeT(Kick__ctx_type_7 &_ctx, fix16_t tune, fix16_t tuneCV, uint8_t g, fix16_t envInt, fix16_t dec, fix16_t hardness){
   uint8_t reset;
   uint8_t _cond_97;
   _cond_97 = Kick_edge(_ctx._inst77,g);
   if(_cond_97){
      reset = 1;
   }
   else
   {
      reset = 0;
   }
   uint8_t _cond_98;
   _cond_98 = Kick_divideFreq(_ctx._inst80,16);
   if(_cond_98){
      _ctx.env = (fix_mul(Kick_envelope(_ctx._inst82,(dec >> 4),g),envInt) >> 1);
   }
   uint8_t _cond_99;
   _cond_99 = (Kick_change(_ctx._inst83,(tune + tuneCV)) || Kick_change(_ctx._inst84,_ctx.env));
   if(_cond_99){
      _ctx.rate = fix_mul((Kick_pitchToRate((tune + _ctx.env)) + 0x41 /* 0.001000 */),Kick_cvToRateMultiplier(tuneCV));
   }
   if(reset){ _ctx.phase = fix_mul(hardness,0x81 /* 0.001969 */); }
   else
   { _ctx.phase = ((_ctx.phase + _ctx.rate) % 0x10000 /* 1.000000 */); }
   fix16_t sine;
   sine = fix_sin(fix_mul(0x6487e /* 6.283185 */,_ctx.phase));
   return sine;
}

void Kick__ctx_type_8_init(Kick__ctx_type_8 &_output_){
   Kick__ctx_type_8 _ctx;
   _ctx.tune = 0x0 /* 0.000000 */;
   _ctx.tuneCV = 0x0 /* 0.000000 */;
   _ctx.pitchEnvInt = 0x0 /* 0.000000 */;
   _ctx.hardness = 0x0 /* 0.000000 */;
   _ctx.accentDrive = 0x0 /* 0.000000 */;
   _ctx.gate = 0;
   _ctx.decay = 0x0 /* 0.000000 */;
   Kick__ctx_type_5_init(_ctx._inst92);
   Kick__ctx_type_4_init(_ctx._inst91);
   Kick__ctx_type_7_init(_ctx._inst90);
   _output_ = _ctx;
   return ;
}

void Kick_process_init(Kick__ctx_type_8 &_output_){
   Kick__ctx_type_8_init(_output_);
   return ;
}

fix16_t Kick_process(Kick__ctx_type_8 &_ctx, fix16_t gateI, fix16_t tuneI, fix16_t tuneCVI, fix16_t decayI, fix16_t pitchEnvIntI, fix16_t hardnessI, fix16_t accentDriveI){
   uint8_t _cond_101;
   _cond_101 = (gateI >= 0x8000 /* 0.500000 */);
   if(_cond_101){
      _ctx.gate = 1;
   }
   else
   {
      uint8_t _cond_100;
      _cond_100 = (gateI < 0x8000 /* 0.500000 */);
      if(_cond_100){
         _ctx.gate = 0;
      }
   }
   _ctx.tune = tuneI;
   _ctx.tuneCV = tuneCVI;
   _ctx.decay = decayI;
   _ctx.pitchEnvInt = pitchEnvIntI;
   _ctx.hardness = hardnessI;
   _ctx.accentDrive = accentDriveI;
   fix16_t kick;
   fix16_t sine;
   fix16_t amp;
   sine = Kick_customBridgeT(_ctx._inst90,_ctx.tune,_ctx.tuneCV,_ctx.gate,_ctx.pitchEnvInt,_ctx.decay,_ctx.hardness);
   amp = Kick_ampEnvelope(_ctx._inst91,_ctx.decay,_ctx.gate);
   kick = fix_mul(sine,amp);
   kick = Kick_accentDrive(kick,_ctx.accentDrive,amp);
   return Kick_LP(_ctx._inst92,kick);
}

void Brain__ctx_type_0_init(Brain__ctx_type_0 &_output_){
   Brain__ctx_type_0 _ctx;
   fix_init_array(7,0x0 /* 0.000000 */,_ctx.kickParam);
   _ctx.voice1 = 0x0 /* 0.000000 */;
   Kick__ctx_type_8_init(_ctx._inst160);
   _output_ = _ctx;
   return ;
}

void Brain_process_init(Brain__ctx_type_0 &_output_){
   Brain__ctx_type_0_init(_output_);
   return ;
}

fix16_t Brain_process(Brain__ctx_type_0 &_ctx, fix16_t input){
   _ctx.voice1 = Kick_process(_ctx._inst160,_ctx.kickParam[0],_ctx.kickParam[1],_ctx.kickParam[5],_ctx.kickParam[2],_ctx.kickParam[3],_ctx.kickParam[4],_ctx.kickParam[6]);
   return (_ctx.voice1 >> 1);
}

void Brain_noteOn_init(Brain__ctx_type_0 &_output_){
   Brain__ctx_type_0_init(_output_);
   return ;
}

void Brain_noteOn(Brain__ctx_type_0 &_ctx, int note, int velocity, int channel){
   if(note == 32){
      _ctx.kickParam[0] = 0x10000 /* 1.000000 */;
   }
}

void Brain_noteOff_init(Brain__ctx_type_0 &_output_){
   Brain__ctx_type_0_init(_output_);
   return ;
}

void Brain_noteOff(Brain__ctx_type_0 &_ctx, int note, int channel){
   if(note == 32){
      _ctx.kickParam[0] = 0x0 /* 0.000000 */;
   }
}

void Brain_controlChange_init(Brain__ctx_type_0 &_output_){
   Brain__ctx_type_0_init(_output_);
   return ;
}

void Brain_controlChange(Brain__ctx_type_0 &_ctx, int control, int value, int channel){
   if(control == 30){
      _ctx.kickParam[1] = int_to_fix(value);
   }
   else if(control == 31){
      _ctx.kickParam[2] = int_to_fix(value);
   }
   else if(control == 33){
      _ctx.kickParam[3] = int_to_fix(value);
   }
   else if(control == 34){
      _ctx.kickParam[4] = int_to_fix(value);
   }
   else if(control == 35){
      _ctx.kickParam[5] = int_to_fix(value);
   }
   else if(control == 36){
      _ctx.kickParam[6] = int_to_fix(value);
   }
}

void Brain_default_init(Brain__ctx_type_0 &_output_){
   Brain__ctx_type_0_init(_output_);
   return ;
}

void Brain_default(Brain__ctx_type_0 &_ctx){
   _ctx.kickParam[0] = 0x0 /* 0.000000 */;
   _ctx.kickParam[1] = 0x200000 /* 32.000000 */;
   _ctx.kickParam[2] = 0x200000 /* 32.000000 */;
   _ctx.kickParam[3] = 0x200000 /* 32.000000 */;
   _ctx.kickParam[4] = 0x200000 /* 32.000000 */;
   _ctx.kickParam[5] = 0x0 /* 0.000000 */;
   _ctx.kickParam[6] = 0x0 /* 0.000000 */;
   _ctx.voice1 = 0x0 /* 0.000000 */;
}



void Brain::update(void)
{
  audio_block_t *block;
  short *bp;

  block = allocate();
  if (block) {
    bp = block->data;
      for(int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        fix16_t v = Brain_process(data, 0);
        v = v / 2;
        if(v > 32767) v = 32767;
        if(v < -32768) v = -32768;
        *bp++ = (int16_t)v;
      }

    transmit(block, 0);
    release(block);
  }
}
