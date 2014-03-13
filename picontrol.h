// alarms for turning pi on and off


// set up the alarms

void setAlarmTimes()
{
  
 piOnTime = 0;
 piOffTime = 0;
 piMidnightOnTime = 0;
 piMidnightOffTime = 0; 
 piOnTimeState = false;
 piOffTimeState = false;
 piMidnightOnTimeState = false;
 piMidnightOffTimeState = false;
 piVoltageShutdownTimeState = false;
 piSunriseTimeState = false;
 piSunsetTimeState = false;
 
  // set up alarms for pi control off and off
  int myhour;
  int myminute;
  Serial.print("PI_START_TIME=");
  Serial.println(PI_START_TIME);  
  sscanf(PI_START_TIME, "%d:%d:", &myhour, &myminute);
  Serial.print("myhour=");
  Serial.println(myhour);
  Serial.print("myminute=");
  Serial.println(myminute);
  
  piOnTime = myhour*3600L+myminute*60L;
  Serial.print("piOnTime=");
  Serial.println(piOnTime);
  
  Serial.print("PI_SHUTDOWN_TIME=");
  Serial.println(PI_SHUTDOWN_TIME);  
 
  sscanf(PI_SHUTDOWN_TIME, "%d:%d:", &myhour, &myminute);
  Serial.print("myhour=");
  Serial.println(myhour);
  Serial.print("myminute=");
  Serial.println(myminute);
  
  piOffTime = myhour*3600L+myminute*60L;
  Serial.print("piOffTime=");
  Serial.println(piOffTime);

   // midnight alarm 00:00 local
   
  //piMidnightOnTime = 8*3600L;   // 08:00 CDA time, 04:00 CURACAO time
  piMidnightOnTime = 4*3600L+600L;   // 08:00 CDA time, 04:00 CURACAO time
  Serial.print("piMidnightOnTime=");
  Serial.println(piMidnightOnTime);
  
  piMidnightOffTime = piMidnightOnTime + PI_MIDNIGHT_WAKEUP_SECONDS_LENGTH  ;   // 08:00 CDA time
  Serial.print("piMidnightOffTime=");
  Serial.println(piMidnightOffTime);

  sscanf(PI_SUNRISE_TIME, "%d:%d:", &myhour, &myminute);
  Serial.print("myhour=");
  Serial.println(myhour);
  Serial.print("myminute=");
  Serial.println(myminute);
  
  piSunriseTime = myhour*3600L+myminute*60L;
 
  sscanf(PI_SUNSET_TIME, "%d:%d:", &myhour, &myminute);
  Serial.print("myhour=");
  Serial.println(myhour);
  Serial.print("myminute=");
  Serial.println(myminute);
  
  piSunsetTime = myhour*3600L+myminute*60L;

 

  
}

bool checkForAlarmTime(time_t checkTime, bool *checkState, time_t timeNow, char *name)
{

  
  time_t daySeconds;
  daySeconds = timeNow % 86400;
  /*
  Serial.print("daySeconds=");
  Serial.println(daySeconds);
  
  Serial.print("checkTime=");
  Serial.println(checkTime);
  
  Serial.print("checkState=");
  Serial.println(*checkState);
  */
  Serial.print(name);
  Serial.print(" timeToAlarm=");
  time_t interval;

  if (daySeconds > checkTime)
  {
        interval = daySeconds - checkTime;
        interval = 86400 - interval;
  }
  else
  {
    interval = checkTime - daySeconds;
  }

  Serial.println(interval); 

  
  // first check to see if time to turn off
  if ((daySeconds > checkTime + 60) && (*checkState == true))
  {
      // shut the alarm off
      *checkState == false;
      
      setAlarmTimes();
      return false;
  }
  
  // ignore alarm if alarm time is past more than 50 seconds
  if (daySeconds >  checkTime + 60)
  {
 
      //Serial.println("checkForAlarmTime - Time Past - No Alarm Triggered");
      return false;
  }
  
  if ((daySeconds > checkTime) && (*checkState == false))
  {
      Serial.println("checkForAlarmTime - Alarm Triggered");
      *checkState = true;
      return true;
    
  }

  //Serial.println("checkForAlarmTime - No Alarm Triggered");
  return false;
  
  
  
}

// functions to be called when an alarm triggers:




void PiBoot(){

  Serial.println("---->Pi Boot");    
  // try restart, then power cycle if no response
  watchDogState = false;
 

  turnPiOn();

  
  WAState = NOINTERRUPT;
  digitalWrite(interruptPi, false); // make sure the line is low now
  sleepTime=5000;
  currentState = STATE0;
  nextState = STATE0;
  watchDogState = true;
  watchDogTimeIncrement = 16L * 60L; // 16 minutes
  //watchDogTimeIncrement = 2 * 60; // 16 minutes
  watchDogTime = readRTCreliably()+watchDogTimeIncrement;
  requestFromPi = false;
  isPiSignaledWD = false;
  lastInterruptToPi = NOINTERRUPT;

}



void PiShutdown(){

  Serial.println("---->Evening Turn Pi Off");           
 
  // shutdown pi

  watchDogState = false;
  WAState = SHUTDOWN;
  lastInterruptToPi = SHUTDOWN;
  Serial.println("---->try shutdown, then power cycle if no response");
  doInterruptPi();
  watchDogTimeIncrement = 3L * 60L; // give Pi 3 minutes to respond and shutdown
  watchDogTime = readRTCreliably()+watchDogTimeIncrement;


}

void PiVoltageShutdown(){

  Serial.println("---->Voltage Turn Pi Off");           
 
  // shutdown pi
  
  justTurnPiOff();

  watchDogState = false;
  WAState = SHUTDOWN;
  lastInterruptToPi = SHUTDOWN;
  Serial.println("---->Voltage Shutdown");
  doInterruptPi();

  enableWatchDog = false;

}


void checkpiVoltageShutdownTime(time_t timeNow)
{
  
  bool alarmTriggered;
  
  alarmTriggered = checkForAlarmTime(piVoltageShutdownTime, &piVoltageShutdownTimeState, timeNow, "PiVoltageShutdown\t" );
  
  if (alarmTriggered == true)
  {
    if (enableShutdowns == false)
    {
      writeLogEntry( LOGINFO, LOGAlarmDisabled, 1); 
    }
    else
    {

      Serial.println("checkpiVoltageShutdownTime - Alarm Triggered");
      writeLogEntry( LOGINFO, LOGAlarmTriggered, ALARMpiVoltageShutdownTime); 
      // check to see if battery has recovered in last 200 seconds
      
      if (piVoltageShutdownThresholdOK == true)
      {
          // cancel this shutdown

          writeLogEntry( LOGINFO, LOGVoltageCancelledRecovery, 4); 
      }
      else
      {      
              PiVoltageShutdown();
      }
      
    }
      piVoltageShutdownTime = readRTCreliably(); // far in future
      piVoltageShutdownTimeState = false;

  }


  
  
}




void checkForPiStartup(time_t timeNow)
{
  
  bool alarmTriggered;
  
  alarmTriggered = checkForAlarmTime(piOnTime, &piOnTimeState, timeNow, "PiStartup\t\t" );
  
  if (alarmTriggered == true)
  {
    if (enableShutdowns == false)
    {
      writeLogEntry( LOGINFO, LOGAlarmDisabled, ALARMPiStartup); 
    }
    else
    {

      Serial.println("checkForPiStartup - Alarm Triggered");
      writeLogEntry( LOGINFO, LOGAlarmTriggered, ALARMPiStartup); 
      PiBoot();
    }

  }


  
  
}



// alarm functions

void checkForPiShutdown(time_t timeNow)
{
  
    
  bool alarmTriggered;
  
  alarmTriggered = checkForAlarmTime(piOffTime, &piOffTimeState, timeNow, "PiShutdown\t\t" );
  
  if (alarmTriggered == true)
  {
    if (enableShutdowns == false)
    {
      writeLogEntry( LOGINFO, LOGAlarmDisabled, ALARMPiShutdown); 
    }
    else
    {
      Serial.println("checkForPiShutdown - Alarm Triggered");
      writeLogEntry( LOGINFO, LOGAlarmTriggered, ALARMPiShutdown); 
      PiShutdown();
    }

   }

}

void checkForPiMidnightStartup(time_t timeNow)
{
  
    
  bool alarmTriggered;
  
  alarmTriggered = checkForAlarmTime(piMidnightOnTime, &piMidnightOnTimeState, timeNow, "PiMidnightStartup\t" );
  
  if (alarmTriggered == true)
  {
    if (enableShutdowns == false)
    {
      writeLogEntry( LOGINFO, LOGAlarmDisabled, ALARMpiMidnightOnTime); 
    }
    else
    {
      Serial.println("piMidnightOnTimeState - Alarm Triggered");
      writeLogEntry( LOGINFO, LOGAlarmTriggered, ALARMpiMidnightOnTime); 
      PiBoot();
    }

  }

  
  
  
}

void checkForPiMidnightShutdown(time_t timeNow)
{
  
    
  bool alarmTriggered;
  
  alarmTriggered = checkForAlarmTime(piMidnightOffTime, &piMidnightOffTimeState, timeNow, "PiMidnightShutdown\t" );
  
  if (alarmTriggered == true)
  {
    if (enableShutdowns == false)
    {
      writeLogEntry( LOGINFO, LOGAlarmDisabled, ALARMpiMidnightOffTime); 
    }
    else
    {
      Serial.println("piMidnightOffTimeState - Alarm Triggered");
      writeLogEntry( LOGINFO, LOGAlarmTriggered, ALARMpiMidnightOffTime); 
      PiShutdown();
    }
 
  }

}

void checkForPiSunriseTime(time_t timeNow)
{
  
    
  bool alarmTriggered;
  
  alarmTriggered = checkForAlarmTime(piSunriseTime, &piSunriseTimeState, timeNow, "PiSunrise\t" );
  
  if (alarmTriggered == true)
  {

      Serial.println("piSunriseState - Alarm Triggered");
      writeLogEntry( LOGINFO, LOGAlarmTriggered, ALARMpiSunrise); 
      
      selectSolar(); 
 
  }

}

void checkForPiSunsetTime(time_t timeNow)
{
  
    
  bool alarmTriggered;
  
  alarmTriggered = checkForAlarmTime(piSunsetTime, &piSunsetTimeState, timeNow, "PiSunset\t" );
  
  if (alarmTriggered == true)
  {

      Serial.println("piSunsetState - Alarm Triggered");
      writeLogEntry( LOGINFO, LOGAlarmTriggered, ALARMpiSunset); 
       
      selectWind(); 
 
  }

}


void checkForAlarms()
{
  time_t timeNow;
  timeNow = readRTCreliably();
  checkForPiShutdown(timeNow);
  checkForPiStartup(timeNow);
  checkForPiMidnightStartup(timeNow);  
  checkForPiMidnightShutdown(timeNow);
  checkpiVoltageShutdownTime(timeNow);
  checkForPiSunriseTime(timeNow);
  checkForPiSunsetTime(timeNow);

  
}


