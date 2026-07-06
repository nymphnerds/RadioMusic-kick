
int originalKickAccentCurve(int accent)
{
    accent = constrain(accent, 0, 127);
    if (accent <= 75)
    {
        return (accent * 40) / 75;
    }
    return 40 + (((accent - 75) * 87) / 52);
}

int kick909AccentCurve(int accent)
{
    accent = constrain(accent, 0, 127);
    if (accent <= 75)
    {
        return (accent * 30) / 75;
    }
    return 30 + (((accent - 75) * 30) / 52);
}

void checkInterface(){
    // Read pots + CVs
    int param1Pot = analogRead(CHORD_POT_PIN); 
    param1CV = analogRead(CHORD_CV_PIN); 
    int param2Pot = analogRead(ROOT_POT_PIN); 
    param2CV = analogRead(ROOT_CV_PIN); 
    int param1Raw;
    int param2Raw;
    if (instrument == BASS_DRUM)
    {
       param1Raw = param1Pot + param1CV; 
       param1Raw = constrain(param1Raw, 0, ADC_MAX_VAL - 1);
       param2Raw = param2Pot;
       if (param2CV > 96)
       {
          kickAccentCvSeen = true;
       }
       kickAccent = kickAccentCvSeen ? constrain(param2CV / 64.5, 0, 127) : 75;
    }
    else if (instrument != 2 && instrument != 3 )
    {
        // cv pot and input summing
       param1Raw = param1Pot + param1CV; 
       param1Raw = constrain(param1Raw, 0, ADC_MAX_VAL - 1);
       param2Raw = param2Pot + param2CV; 
       param2Raw = constrain(param2Raw, 0, ADC_MAX_VAL - 1);
    }
    else if (instrument == 2)
    {
      param1Raw = param1Pot + param1CV; 
      param1Raw = constrain(param1Raw, 0, ADC_MAX_VAL - 1); 
      param2Raw = param2Pot;
    }   
    else if (instrument == 3)
    {
      param1Raw = param1Pot; 
      param2Raw = param2Pot + param2CV; 
      param2Raw = constrain(param2Raw, 0, ADC_MAX_VAL - 1);
    }   
    param1 = param1Raw / 64.5;
    param2 = param2Raw / 64.5;
}

void checkButton(){
    buttonState = digitalRead(RESET_BUTTON);
    if (pushbutton1.update())
    {
        if (pushbutton1.risingEdge() ) 
        {
            shiftTimer  = 0;
            startCounter = true;
        }
        else if (pushbutton1.fallingEdge() ) 
        {
            startCounter = false;
            if ( shiftTimer < 3000 )
            {
                if (kickEngine == KICK_ENGINE_ORIGINAL)
                {
                  kickEngine = KICK_ENGINE_909;
                }
                else
                {
                  kickEngine = KICK_ENGINE_ORIGINAL;
                }
                kickControlsHoldUntilMove = true;
                lastKickParam1 = param1;
                lastKickParam2 = param2;
            }
            shiftTimer  = 0;
            modeChanged = false;
        }
    }
    if (buttonState == 1 && shiftTimer > 4500 && modeChanged == false)
    {   
        if (controlPage2 == true)
        {
           controlPage2 = false;
        }
        else
        {
            controlPage2 = true;
        }
        kickControlsHoldUntilMove = true;
        lastKickParam1 = param1;
        lastKickParam2 = param2;
        modeChanged = true;
    }
    if (instrument >= 4)
    {
        instrument = 0;
    }
    instrument = BASS_DRUM;
    if (startCounter == true)
    {
      shiftTimer ++;
    }
    
   frame++;
   if (frame%FRAME_DIVIDER == 0)
   {
      dividedFrame ++;
   }
   if (frame > 10000)
   {
      frame = 0;
   }
   if (dividedFrame > 1000)
   {
      dividedFrame = 0;
   }
  
   if (controlPage2 == true)
   {
      if (dividedFrame%2 == 0)
      {
         digitalWrite(RESET_LED, HIGH);
      }
      else
      {
         digitalWrite (RESET_LED, LOW);
      }
   }
   else
   {
      digitalWrite (RESET_LED, LOW);
   }
    if ( modeChanged == true)
   {
      EEPROM_Mode_Frame ++;
      if (EEPROM_Mode_Frame > 15000)
      {
          if (hasBeenSaved == false)
          {
            int address = 0;
            for (int i = 0; i < 4; i ++)
            {
              for( int j = 0; j <4; j++)
              {
                  EEPROM.write(address, instrumentsParams[i][j]);
                  address ++;
              }
            }
            EEPROM.write(17, instrument);
            hasBeenSaved = true;
          }
          if (EEPROM_Mode_Frame%(FRAME_DIVIDER / 4) == 0)
          {
              DIVIDED_EEPROM_Mode_Frame ++;
          }
          if (EEPROM_Mode_Frame > 1000)
          {
              DIVIDED_EEPROM_Mode_Frame = 0;
          }
          if (DIVIDED_EEPROM_Mode_Frame%2 == 0)
          {
             digitalWrite(RESET_LED, HIGH);
          }
          else
          {
             digitalWrite (RESET_LED, LOW);
          }
      }
   }
}

void controlInstrumentParams(){
    // BD: 30 tune, 31 decay, 33 pitchenv, 34 hardness, 
    // SD: 35 tune, 36 decay, 37 noise decay, 38 noise balance
    // HH: 39 tune , 40 decay, 41 open decay, 42 noise balance
   if (instrument == BASS_DRUM)
   {
      int *kickParams = kickEngineParams[kickEngine];
      voice1.controlChange(43, kickEngine == KICK_ENGINE_909 ? 1 : 0, 1);
      if (kickEngine == KICK_ENGINE_ORIGINAL)
      {
        int matchedOriginalTune = (kickParams[0] - 27) / 2;
        voice1.controlChange(30, constrain(matchedOriginalTune, 0, 127), 1);
      }
      else
      {
        voice1.controlChange(30, kickParams[0], 1);
      }
      int effectiveKickAccent = kickEngine == KICK_ENGINE_909 ? kick909AccentCurve(kickLatchedAccent) : originalKickAccentCurve(kickLatchedAccent);
      int accentAboveNormal = effectiveKickAccent > 75 ? effectiveKickAccent - 75 : 0;
      int accentBelowNormal = effectiveKickAccent < 75 ? 75 - effectiveKickAccent : 0;
      int accentPitch = kickEngine == KICK_ENGINE_909 ? 0 : accentAboveNormal / 3;
      int accentHardness = kickEngine == KICK_ENGINE_909 ? 0 : accentAboveNormal / 4;
      int accentClick = kickEngine == KICK_ENGINE_909 ? 0 : accentBelowNormal;
      int gateDecay = kickParams[1];
      if (kickEngine == KICK_ENGINE_909)
      {
        gateDecay = kickParams[1];
      }
      else
      {
        if (activeTrig && kickGateTimer > 50)
        {
          kickGateDecayBoost = constrain((int)(kickGateTimer - 50) / 3, 0, 127);
        }
        gateDecay = constrain(gateDecay + (kickGateDecayBoost * 2) + (accentAboveNormal / 8) - (accentBelowNormal / 2), 0, 127);
      }
      voice1.controlChange(31, gateDecay, 1);
      int pitchSweep = kickParams[2];
      voice1.controlChange(33, constrain(pitchSweep + accentPitch, 0, 127), 1);
      voice1.controlChange(34, constrain(kickParams[3] + accentHardness, 0, 127), 1);
      voice1.controlChange(44, effectiveKickAccent, 1);
      voice1.controlChange(45, accentClick, 1);
   }
   else if (instrument == SNARE_DRUM)
   {
      for (int i = 0; i < 4; i++)
      {
        voice1.controlChange(35 + i, instrumentsParams[instrument][i], 1);
      }
   }
   else if (instrument == CLOSED_HI_HAT)
   {
      for (int j = 0; j < 4; j++)
      {
       voice1.controlChange(39 + j, instrumentsParams[instrument][j], 1);
      }
   }
}

void setControlValues(){  
   
    if (buttonState == 0)
    {
        if (instrument == BASS_DRUM)
        {
          int firstParam = controlPage2 == false ? 0 : 2;
          int secondParam = firstParam + 1;
          if (kickControlsHoldUntilMove == true)
          {
              boolean param1Moved = abs(param1 - lastKickParam1) > 1;
              boolean param2Moved = abs(param2 - lastKickParam2) > 1;
              if (param1Moved)
              {
                  kickEngineParams[kickEngine][firstParam] = param1;
                  lastKickParam1 = param1;
              }
              if (param2Moved)
              {
                  kickEngineParams[kickEngine][secondParam] = param2;
                  lastKickParam2 = param2;
              }
              if (param1Moved || param2Moved)
              {
                  kickControlsHoldUntilMove = false;
              }
          }
          else
          {
              kickEngineParams[kickEngine][firstParam] = param1;
              kickEngineParams[kickEngine][secondParam] = param2;
              lastKickParam1 = param1;
              lastKickParam2 = param2;
          }
          if (firstParam == 0)
          {
              kickEngineParams[KICK_ENGINE_ORIGINAL][0] = kickEngineParams[kickEngine][0];
              kickEngineParams[KICK_ENGINE_909][0] = kickEngineParams[kickEngine][0];
          }
          if (controlPage2 == false)
          {
              instrumentsParams[instrument][0] = kickEngineParams[kickEngine][0];
              instrumentsParams[instrument][1] = kickEngineParams[kickEngine][1];
          }
          else
          {
              instrumentsParams[instrument][2] = kickEngineParams[kickEngine][2];
              instrumentsParams[instrument][3] = kickEngineParams[kickEngine][3];
          }
          return;
        }
        for (int i = 0; i < 2; i++)
        {
           if (controlPage2 == false)
          {
              param[0] = param1;
              param[1] = param2;
              if ((analogRead(CHORD_CV_PIN) < 15 && i == 0) || instrument == 3)
              {
                if ((param[i] < instrumentsParams[instrument][i] + POT_TOLERANCE && param[i] > instrumentsParams[instrument][i] - POT_TOLERANCE) )
                {
                  instrumentsParams[instrument][i] = param[i];
                }
              }
              else if (analogRead(CHORD_CV_PIN) > 15 && i == 0)
              {
                  instrumentsParams[instrument][i] = param[i];
              }
              if ((analogRead(ROOT_CV_PIN) < 15 && i == 1 ) || instrument == 3)
              {
                if ((param[i] < instrumentsParams[instrument][i] + POT_TOLERANCE && param[i] > instrumentsParams[instrument][i] - POT_TOLERANCE) )
                {
                  instrumentsParams[instrument][i] = param[i];
                }
              }
              else if (analogRead(ROOT_CV_PIN) > 15 && i == 1)
              {
                  instrumentsParams[instrument][i] = param[i];
              }
          }
          else
          {
              param[2] = param1;
              param[3] = param2;
              if ((analogRead(CHORD_CV_PIN) < 15 && i == 0 ) || instrument == 3)
              {
                if ((param[i + 2] < instrumentsParams[instrument][i + 2] + POT_TOLERANCE && param[i + 2] > instrumentsParams[instrument][i + 2] - POT_TOLERANCE) )
                {
                  instrumentsParams[instrument][i + 2] = param[i + 2];
                }
              }
              else if (analogRead(CHORD_CV_PIN) > 15 && i == 0)
              {
                  instrumentsParams[instrument][i + 2] = param[i + 2];
              }
              if ((analogRead(ROOT_CV_PIN) < 15 && i == 1) || instrument == 3)
              {
                if ((param[i + 2] < instrumentsParams[instrument][i + 2] + POT_TOLERANCE && param[i + 2] > instrumentsParams[instrument][i + 2] - POT_TOLERANCE) )
                {
                  instrumentsParams[instrument][i + 2] = param[i + 2];
                }
              }
              else if (analogRead(ROOT_CV_PIN) > 15 && i == 1)
              {
                  instrumentsParams[instrument][i + 2] = param[i + 2];
              }
          }
        }
    }
}

void selectInstrument(){
    // select sound : control 28 <35 => kick, <75 => snare, >75 => hihat
    int currentInstrument;
    if (instrument == BASS_DRUM)
    {
     digitalWrite(LED3, HIGH);
      currentInstrument = 15;
    }
    else 
    {
     digitalWrite(LED3, LOW);
    }
    if (instrument == SNARE_DRUM)
    {
     digitalWrite(LED2, HIGH);
      currentInstrument = 60;
    }
    else
    {
     digitalWrite(LED2, LOW);
    }
    if (instrument == CLOSED_HI_HAT)
    {
     digitalWrite(LED1, HIGH);
      currentInstrument = 80;
    }
    else
    {
     digitalWrite(LED1, LOW);
    }
    if (instrument == SAMPLES)
    {
      digitalWrite(LED0, HIGH);
      currentInstrument = 80;
    }
    else
    {
      digitalWrite(LED0, LOW);
    }
    voice1.controlChange(28, currentInstrument , 1);
    if (instrument == BASS_DRUM)
    {
      digitalWrite(LED3, kickEngine == KICK_ENGINE_ORIGINAL ? HIGH : LOW);
      digitalWrite(LED2, kickEngine == KICK_ENGINE_909 ? HIGH : LOW);
      digitalWrite(LED1, LOW);
      digitalWrite(LED0, LOW);
    }
}

void trigInstrument(){
     int param2CV = analogRead(ROOT_CV_PIN); 
    // trig drums 32 => kick, 33 => snare, 34 => hihat closed, 35 => hihat open
    int currentInstrument = 32 + instrument;
    if (digitalRead (RESET_CV) == 1 && activeTrig == false && instrument < 3 )
    {
      if (instrument == BASS_DRUM)
      {
        int triggerAccentCV = analogRead(ROOT_CV_PIN);
        if (triggerAccentCV > 96)
        {
          kickAccentCvSeen = true;
        }
        kickLatchedAccent = kickAccentCvSeen ? constrain(triggerAccentCV / 64.5, 0, 127) : 75;
        int triggerEffectiveAccent = kickEngine == KICK_ENGINE_909 ? kick909AccentCurve(kickLatchedAccent) : originalKickAccentCurve(kickLatchedAccent);
        voice1.controlChange(44, triggerEffectiveAccent, 1);
        int triggerClick = kickEngine == KICK_ENGINE_909 || triggerEffectiveAccent >= 75 ? 0 : 75 - triggerEffectiveAccent;
        voice1.controlChange(45, triggerClick, 1);
        kickGateTimer = 0;
        kickGateDecayBoost = 0;
      }
      voice1.noteOn(currentInstrument ,127,1);
      activeTrig = true;
    }
    if (digitalRead(RESET_CV) == 0 && instrument < 3 )
    {
      voice1.noteOff(currentInstrument, 0, 1);
      activeTrig = false;
    }
    
    if (instrument == CLOSED_HI_HAT)
    {
       if (param2CV >500 && activeOpenHatTrig == false)
       {
         voice1.noteOn(currentInstrument + 1 ,127,1);
         activeOpenHatTrig = true;
       }
       if (param2CV < 200)
       {
         voice1.noteOff(currentInstrument + 1 ,0,1);
         activeOpenHatTrig = false;
       }
    } 
    
    if (instrument == SAMPLES)
    {
      //param2CV = analogRead(CHORD_CV_PIN); 
        if (controlPage2 == false)
        {
            sampleNumber1 = instrumentsParams[instrument][1] /20;
            if (sampleNumber1 == 0)
            {
              if (instrumentsParams[instrument][0] < 110)
              {
                 envelope0.decay((instrumentsParams[instrument][0] * 1.5) + 10);
              }
              else
              {
                envelope0.decay(10000);
              }
            }
            else if (sampleNumber1 == 1)
            {
              if (instrumentsParams[instrument][0] < 110)
              {
                 envelope0.decay((instrumentsParams[instrument][0] * 2.1) + 10);
              }
              else
              {
                envelope0.decay(10000);
              }
            }
             else if (sampleNumber1 == 2)
            {
              if (instrumentsParams[instrument][0] < 110)
              {
                 envelope0.decay((instrumentsParams[instrument][0] * 2.2) + 10);
              }
              else
              {
                envelope0.decay(10000);
              }
            }
             else if (sampleNumber1 == 3)
            {
              if (instrumentsParams[instrument][0] < 110)
              {
                 envelope0.decay((instrumentsParams[instrument][0] * 4) + 10);
              }
              else
              {
                envelope0.decay(10000);
              }
            }
             else if (sampleNumber1 == 4)
            {
              if (instrumentsParams[instrument][0] < 110)
              {
                 envelope0.decay((instrumentsParams[instrument][0] * 0.5) + 5);
              }
              else
              {
                envelope0.decay(5000);
              }
            }
             else if (sampleNumber1 == 5)
            {
              if (instrumentsParams[instrument][0] < 110)
              {
                 envelope0.decay((instrumentsParams[instrument][0] * 15) + 5);
              }
              else
              {
                envelope0.decay(15000);
              }
            }
        }

        if (controlPage2 == true)
        {
          sampleNumber2 = instrumentsParams[instrument][3]/20;
          if (sampleNumber2 == 0)
          {
              if (instrumentsParams[instrument][2] < 110)
              {
                 envelope1.decay((instrumentsParams[instrument][2] * 1.5) + 10);
              }
              else
              {
                envelope1.decay(10000);
              }
          }
          else if (sampleNumber2 == 1)
          {
             if (instrumentsParams[instrument][2] < 110)
              {
                 envelope1.decay((instrumentsParams[instrument][2] * 2.1) + 10);
              }
              else
              {
                envelope1.decay(10000);
              }
          }
          else if (sampleNumber2 == 2)
          {
              if (instrumentsParams[instrument][2] < 110)
              {
                 envelope1.decay((instrumentsParams[instrument][2] * 2.2) + 10);
              }
              else
              {
                envelope1.decay(10000);
              }
          }
          else if (sampleNumber2 == 3)
          {
            if (instrumentsParams[instrument][2] < 110)
              {
                 envelope1.decay((instrumentsParams[instrument][2] * 4) + 10);
              }
              else
              {
                envelope1.decay(10000);
              }
          }
          else if (sampleNumber2 == 4)
          {
            if (instrumentsParams[instrument][2] < 110)
              {
                 envelope1.decay((instrumentsParams[instrument][2] * 0.5) + 5);
              }
              else
              {
                envelope1.decay(5000);
              }
          }
          else if (sampleNumber2 == 5)
          {
             if (instrumentsParams[instrument][2] < 110)
              {
                 envelope1.decay((instrumentsParams[instrument][2] * 15) + 5);
              }
              else
              {
                envelope1.decay(15000);
              }
          } 
        }


        if (sampleNumber1 == 0)
            {
              if (digitalRead(RESET_CV) == 1 && activeTrig == false )
              {
                  sound0.play(AudioSampleKick);
                  activeTrig = true;
                  envelope0.noteOn();
              }
            }
            else if (sampleNumber1 == 1)
            {
              if (digitalRead(RESET_CV) == 1 && activeTrig == false )
              {
                  sound0.play(AudioSampleSnare);
                  activeTrig = true;
                  envelope0.noteOn();
              }
            }
            else if (sampleNumber1 == 2)
            {
              if (digitalRead(RESET_CV) == 1 && activeTrig == false )
              {
                  sound0.play(AudioSampleClap);
                  activeTrig = true;
                  envelope0.noteOn();
              }
            }
            else if (sampleNumber1 == 3)
            {
              if (digitalRead(RESET_CV) == 1 && activeTrig == false )
              {
                  sound0.play(AudioSampleHihatopened);
                  activeTrig = true;
                  envelope0.noteOn();
              }
            }
            else if (sampleNumber1 == 4)
            {
              if (digitalRead(RESET_CV) == 1 && activeTrig == false )
              {
                  sound0.play(AudioSampleHihatclosed);
                  activeTrig = true;
                  envelope0.noteOn();
              }
            }
            else if (sampleNumber1 == 5)
            {
              if (digitalRead(RESET_CV) == 1 && activeTrig == false )
              {
                  sound0.play(AudioSampleExtraSound);
                  activeTrig = true;
                  envelope0.noteOn();
              }
            }
            if (sampleNumber2 == 0)
            {
              
              if (param1CV >500 && activeOpenHatTrig == false )
              {
                  sound1.play(AudioSampleKick);
                  activeOpenHatTrig = true;
                  envelope1.noteOn();
              }
            }
            else if (sampleNumber2 == 1)
            {
              if (param1CV >500 && activeOpenHatTrig == false )
              {
                  sound1.play(AudioSampleSnare);
                  activeOpenHatTrig = true;
                  envelope1.noteOn();
              }
            }
            else if (sampleNumber2 == 2)
            {
              if (param1CV >500 && activeOpenHatTrig == false )
              {
                  sound1.play(AudioSampleClap);
                  activeOpenHatTrig = true;
                  envelope1.noteOn();
              }
            }
            else if (sampleNumber2 == 3)
            {
              if (param1CV >500 && activeOpenHatTrig == false )
              {
                  sound1.play(AudioSampleHihatopened);
                  activeOpenHatTrig = true;
                  envelope1.noteOn();
              }
            }
            else if (sampleNumber2 == 4)
            {
              if (param1CV >500 && activeOpenHatTrig == false )
              {
                  sound1.play(AudioSampleHihatclosed);
                  activeOpenHatTrig = true;
                  envelope1.noteOn() ;
              }
            }
            else if (sampleNumber2 == 5)
            {
              if (param1CV >500 && activeOpenHatTrig == false )
              {
                  sound1.play(AudioSampleExtraSound);
                  activeOpenHatTrig = true;
                  envelope1.noteOn();
              }
            }
            if (digitalRead(RESET_CV) == 0)
            {
                activeTrig = false;
            }
            
            if (param1CV < 200)
            { 
                  activeOpenHatTrig = false;
            }
            if (digitalRead(RESET_CV) == 0)
            {
                 activeTrig = false;
            }
    }
}

/*
void setReverb(){
  
  if ((buttonState == 1 && param1 < reverbKnobParam + 5 && param1 > reverbKnobParam - 5) || (buttonState == 1 && delock == true))
  {
    float gainValue = param1/40.0;
    mixer2.gain(0, gainValue);
    mixer2.gain(1, gainValue);
    reverbKnobParam = param1;
    delock = true;
  }
  if (buttonState == false) 
  {
    delock = false;
  }
  if ((buttonState == 1 && param2 < reverbKnobParam2 + 5 && param2 > reverbKnobParam2 - 5) || (buttonState == 1 && delock2 == true))
  {
    float timeValue = param2/10.0;
    reverb1.reverbTime(timeValue);
    reverbKnobParam2 = param2;
    delock2 = true;
  }
  if (buttonState == false) 
  {
    delock2 = false;
  }
 }
*/
