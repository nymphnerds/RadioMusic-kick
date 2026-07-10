
int kick909AccentCurve(int accent)
{
    accent = constrain(accent, 0, 127);
    if (accent <= 75)
    {
        return (accent * 30) / 75;
    }
    return 30 + (((accent - 75) * 30) / 52);
}

void recallKickEngineParams()
{
    for (int j = 0; j < 4; j++)
    {
        instrumentsParams[BASS_DRUM][j] = kickEngineParams[kickEngine][j];
    }
    kickPage2Params[kickEngine][0] = kickEngineParams[kickEngine][2];
    kickPage2Params[kickEngine][1] = kickEngineParams[kickEngine][3];
}

void checkInterface(){
    // Read pots + CVs
    int param1Pot = analogRead(CHORD_POT_PIN); 
    param1CV = analogRead(CHORD_CV_PIN); 
    int param2Pot = analogRead(ROOT_POT_PIN); 
    param2CV = analogRead(ROOT_CV_PIN); 

    int param1Raw = controlPage2 ? constrain(param1Pot + param1CV, 0, ADC_MAX_VAL - 1) : param1Pot;
    int param2Raw = param2Pot;
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
            if (shiftTimer < 3000 && modeChanged == false)
            {
                kickEngine = kickEngine == KICK_ENGINE_808 ? KICK_ENGINE_909 : KICK_ENGINE_808;
                recallKickEngineParams();
                kickControlsHoldUntilMove = true;
                kickParam1HoldUntilMove = true;
                kickParam2HoldUntilMove = true;
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
        kickParam1HoldUntilMove = true;
        kickParam2HoldUntilMove = true;
        lastKickParam1 = param1;
        lastKickParam2 = param2;
        modeChanged = true;
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
            for( int j = 0; j <4; j++)
            {
              EEPROM.write(j, instrumentsParams[BASS_DRUM][j]);
            }
            for (int engine = 0; engine < 2; engine++)
            {
              for (int j = 0; j < 4; j++)
              {
                EEPROM.write(30 + (engine * 4) + j, kickEngineParams[engine][j]);
              }
              for (int j = 0; j < 2; j++)
              {
                EEPROM.write(20 + (engine * 2) + j, kickPage2Params[engine][j]);
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
    voice1.controlChange(30, instrumentsParams[BASS_DRUM][0], 1);
    voice1.controlChange(31, instrumentsParams[BASS_DRUM][1], 1);
    if (kickEngine == KICK_ENGINE_909)
    {
        voice1.controlChange(33, kickLatchedAccent, 1);
        voice1.controlChange(34, kickLatchedAccent, 1);
    }
    else
    {
        voice1.controlChange(33, kickPage2Params[kickEngine][0], 1);
        voice1.controlChange(34, kickPage2Params[kickEngine][1], 1);
    }
    voice1.controlChange(35, param1CV, 1);
    voice1.controlChange(36, param2CV, 1);
    voice1.controlChange(43, kickEngine == KICK_ENGINE_909 ? 1 : 0, 1);
    voice1.controlChange(44, kick909AccentCurve(kickLatchedAccent), 1);
}

void updateKickParam(int paramIndex, int value){
    kickEngineParams[kickEngine][paramIndex] = value;
    if (paramIndex >= 2)
    {
        kickPage2Params[kickEngine][paramIndex - 2] = value;
        instrumentsParams[BASS_DRUM][paramIndex] = value;
    }
    else
    {
        instrumentsParams[BASS_DRUM][paramIndex] = value;
    }
}

void setControlValues(){  
   
    if (buttonState == 0)
    {
        int firstParam = controlPage2 == false ? 0 : 2;
        if (kickControlsHoldUntilMove == true)
        {
            boolean param1Moved = abs(param1 - lastKickParam1) > 1;
            boolean param2Moved = abs(param2 - lastKickParam2) > 1;
            if (kickParam1HoldUntilMove == false || param1Moved)
            {
                updateKickParam(firstParam, param1);
                lastKickParam1 = param1;
                kickParam1HoldUntilMove = false;
            }
            if (kickParam2HoldUntilMove == false || param2Moved)
            {
                updateKickParam(firstParam + 1, param2);
                lastKickParam2 = param2;
                kickParam2HoldUntilMove = false;
            }
            if (kickParam1HoldUntilMove == false && kickParam2HoldUntilMove == false)
            {
                kickControlsHoldUntilMove = false;
            }
        }
        else
        {
            updateKickParam(firstParam, param1);
            updateKickParam(firstParam + 1, param2);
            lastKickParam1 = param1;
            lastKickParam2 = param2;
        }
    }
}

void selectInstrument(){
    instrument = BASS_DRUM;
    digitalWrite(LED3, kickEngine == KICK_ENGINE_909 ? HIGH : LOW);
    digitalWrite(LED2, kickEngine == KICK_ENGINE_808 ? HIGH : LOW);
    digitalWrite(LED1, LOW);
    digitalWrite(LED0, LOW);
    voice1.controlChange(28, 15, 1);
}

void trigInstrument(){
    int gateState = digitalRead(RESET_CV);
    if (gateState == 1 && activeTrig == false)
    {
      param2CV = analogRead(ROOT_CV_PIN);
      if (param2CV > 96)
      {
          kickAccentCvSeen = true;
      }
      kickLatchedAccent = kickAccentCvSeen ? constrain(param2CV / 64.5, 0, 127) : 75;
      voice1.controlChange(36, param2CV, 1);
      if (kickEngine == KICK_ENGINE_909)
      {
          voice1.controlChange(33, kickLatchedAccent, 1);
          voice1.controlChange(34, kickLatchedAccent, 1);
      }
      voice1.controlChange(44, kick909AccentCurve(kickLatchedAccent), 1);
      voice1.noteOn(32 ,127,1);
      activeTrig = true;
    }
    if (gateState == 0 && activeTrig == true)
    {
      voice1.noteOff(32, 0, 1);
      activeTrig = false;
    }
}
