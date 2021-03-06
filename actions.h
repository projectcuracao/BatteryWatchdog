//
//
// actions file for hardware
//
//


time_t readRTCreliably()
{
  
 time_t returnTime;
 
 int i;
 i = 0;
 for (i=0; i< 5; i++)
 {
   returnTime = RTC.get();

   if (returnTime == 0)
    {
     Serial.println("<<<<<<<<<<<<<<Bad Time Read from RTC");
    }
    else
    {
      return returnTime;
    }
  
 }
 
 // 5 bad ones, take current time
 Serial.println(">>>>>>>>>>>>Time taken from Arduino");
 writeLogEntry( LOGINFO, LOGBad5RTCRead, 1); 
 
 //resetFunc (); //declare reset function @ address 0
 //delay(100);
 return now();

  
}



void justTurnPiOff()
{
  
      digitalWrite(PiRelayReset, false);
    digitalWrite(PiRelayReset, true);
    delay(100);
    digitalWrite(PiRelayReset, false);
    Serial.println("Pi Just Turned OFF");
    
    writeLogEntry( LOGINFO, LOGPiOff, -1); 
    digitalWrite(interruptPi, false); // make sure the line is low now
    
    requestFromPi = false;
    isPiSignaledWD = false;
    lastInterruptToPi = NOINTERRUPT;
   
    isPiOn = false;

  
  
  
  
  
}

void turnPiOff()
{

    digitalWrite(PiRelayReset, false);
    digitalWrite(PiRelayReset, true);
    delay(100);
    digitalWrite(PiRelayReset, false);
    Serial.println("Pi Turned OFF");
    
    writeLogEntry( LOGINFO, LOGPiOff, -1); 
    digitalWrite(interruptPi, false); // make sure the line is low now
    
    requestFromPi = false;
    isPiSignaledWD = false;
    lastInterruptToPi = NOINTERRUPT;
   
    isPiOn = false;
    shouldPiBeOn = false;

}

void justTurnPiOn()
{
    shouldPiBeOn = true;
    digitalWrite(interruptPi, false); // make sure the line is low now
    digitalWrite(PiRelaySet, false);
    digitalWrite(PiRelaySet, true);
    delay(100);
    digitalWrite(PiRelaySet, false);
    Serial.println("Pi Turned ON"); 
    writeLogEntry( LOGINFO, LOGPiOn, -1); 
    isPiOn = true;

}

void turnPiOn()
{
    shouldPiBeOn = true;
    if (piVoltageStartupThresholdOK == true)
    {
      digitalWrite(interruptPi, false); // make sure the line is low now
      digitalWrite(PiRelaySet, false);
      digitalWrite(PiRelaySet, true);
      delay(100);
      digitalWrite(PiRelaySet, false);
      Serial.println("Pi Turned ON"); 
      writeLogEntry( LOGINFO, LOGPiOn, -1); 
      isPiOn = true;
    }
    else
    {
      writeLogEntry( LOGINFO, LOGPiOnOveruledVoltage, -1); 

      isPiOn = false; 
    }
    enableWatchDog = true;
}


//
// support files for Pi Battery Monitor
//

float getAnalogVoltage(int pinNumber)
{
 
     return 5.0*(analogRead(pinNumber) / 1024.0);
}

//
// Interrupt Pi
//
bool doInterruptPi()
{
 
   Serial.println("-->Notifying Pi<--");
   // changing to level change letting pi know as interrupts on pi under python go away after a while
   digitalWrite(interruptPi, false);
   delay(100);
   digitalWrite(interruptPi, true); 
   //delay(100);
   //digitalWrite(interruptPi, false); 
   
   writeLogEntry( LOGINFO, LOGPiInterrupt, WAState); 
   return true;
}

#define selectSolarReset 43
#define selectWindSet 41

// select solar/wind
bool selectSolar()
{
    digitalWrite(selectSolarReset, false);
    digitalWrite(selectSolarReset, true);
    delay(100);
    digitalWrite(selectSolarReset, false);
    Serial.println("Solar Selected");
    
    writeLogEntry( LOGINFO, LOGSolarSelect, -1); 

    
    solarWind = 0;


}

bool selectWind()
{
    digitalWrite(selectWindSet, false);
    digitalWrite(selectWindSet, true);
    delay(100);
    digitalWrite(selectWindSet, false);
    Serial.println("Wind Selected");
    
    writeLogEntry( LOGINFO, LOGWindSelect, -1); 
 
    
    solarWind = 1;

  
}
