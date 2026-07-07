
void checkInterface(){
    // Read pots + CVs
    int param1Pot = analogRead(CHORD_POT_PIN); 
    param1CV = analogRead(CHORD_CV_PIN); 
    int param2Pot = analogRead(ROOT_POT_PIN); 
    param2CV = analogRead(ROOT_CV_PIN); 

    int param1Raw = controlPage2 ? constrain(param1Pot + param1CV, 0, ADC_MAX_VAL - 1) : param1Pot;
    int param2Raw = controlPage2 ? constrain(param2Pot + param2CV, 0, ADC_MAX_VAL - 1) : param2Pot;
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
    voice1.controlChange(33, instrumentsParams[BASS_DRUM][2], 1);
    voice1.controlChange(34, instrumentsParams[BASS_DRUM][3], 1);
    voice1.controlChange(35, param1CV, 1);
}

void updateKickParam(int paramIndex, int value){
    instrumentsParams[BASS_DRUM][paramIndex] = value;
}

void setControlValues(){  
   
    if (buttonState == 0)
    {
        int firstParam = controlPage2 == false ? 0 : 2;
        updateKickParam(firstParam, param1);
        updateKickParam(firstParam + 1, param2);
    }
}

void selectInstrument(){
    instrument = BASS_DRUM;
    digitalWrite(LED3, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED1, LOW);
    digitalWrite(LED0, LOW);
    voice1.controlChange(28, 15, 1);
}

void trigInstrument(){
    int gateState = digitalRead(RESET_CV);
    if (gateState == 1 && activeTrig == false)
    {
      voice1.noteOn(32 ,127,1);
      activeTrig = true;
    }
    if (gateState == 0 && activeTrig == true)
    {
      voice1.noteOff(32, 0, 1);
      activeTrig = false;
    }
}
