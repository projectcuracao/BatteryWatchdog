

// Arduino Logging

// define logging constants
// entryData0

#define LOGNoEvent 0
#define LOGPiShutdown 1
#define LOGPiStartup 2
#define LOGPiOff 3
#define LOGPiOn 4
// LOGPiPower has voltage as entryData1
#define LOGPiPower 5
// LOGPiInterrupt has reason as entryData1
#define LOGPiInterrupt 6
#define LOGThresholdsSet 7
// LOGSensorFail has sensor # (i2c address) as entryData1
#define LOGSensorFail 8
// LOGAlarmTriggered has alarm # (as in the check alarm routine) as entryData1
#define LOGAlarmTriggered 9
#define LOGDeadManSwitchTriggered 10
#define LOGArduinoReboot 11
#define LOGWatchDogTriggered 12
// LOGAlarmTriggered has alarm # (as in the check alarm routine) as entryData2
#define LOGAlarmDisabled 13
#define LOGPiOnOveruledVoltage 14

#define LOGPiOffLowVoltage 15
#define LOGPiOnVoltageRecovery 16

#define LOGSolarSelect 17
#define LOGWindSelect 18
#define LOGVoltageCancelledRecovery 19



void initializeLogTable()
{
  
  int i;
  
  for (i=0; i < LOGENTRYTABLESIZE; i++)
  {
     LogEntryArray[i].entryTime = 0;
     LogEntryArray[i].entryType = 0;
     LogEntryArray[i].entryData0 = 0;
     LogEntryArray[i].entryData1 = 0;
     LogEntryArray[i].entryRead = false;
        
    
  }

  
}

void writeLogEntry( int entryType, int entryData0, int entryData1)
{

     LogEntryArray[LogEntryNextItem].entryTime = RTC.get();
     LogEntryArray[LogEntryNextItem].entryType = entryType;
     LogEntryArray[LogEntryNextItem].entryData0 = entryData0;
     LogEntryArray[LogEntryNextItem].entryData1 = entryData1;
     LogEntryArray[LogEntryNextItem].entryRead = false;
     
    LogEntryNextItem++;
    if (LogEntryNextItem >= LOGENTRYTABLESIZE)
      LogEntryNextItem = 0;

}

int returnUnreadCount()
{

  int i;
  int count;
  count = 0;
  
  for (i=0; i < LOGENTRYTABLESIZE; i++)
  {
    
    if ((LogEntryArray[i].entryTime > 0) && (LogEntryArray[i].entryRead == false))
    {
      count++;
   

      
    }

  }
  return count;   // no unread elements
  

  
  
}

int fetchNextUnreadEntry()
{

  int i;
  
  for (i=0; i < LOGENTRYTABLESIZE; i++)
  {
    
    if ((LogEntryArray[i].entryTime > 0) && (LogEntryArray[i].entryRead == false))
    {
      LogEntryArray[i].entryRead = true;
      return i;
      
    }

  }
  return -1;   // no unread elements

  
}

void displayLog()
{
    int i;
    for (i=0; i < LOGENTRYTABLESIZE; i++)
    {
      char buffer2[30];
      buffer2[0] = 0;
      if (LogEntryArray[i].entryTime > 0)
      {
         Serial.print("LogEntry: ");
         Serial.print(i);
         Serial.print(":");
         tmElements_t tm;
         breakTime(LogEntryArray[i].entryTime, tm);
         
         buildTimeString(buffer2, buffer2, tm );
         Serial.print(buffer2);
         Serial.print(" :");
         Serial.print(LogEntryArray[i].entryType);
         Serial.print(":");
         Serial.print(LogEntryArray[i].entryData0);
         Serial.print(":");
         Serial.print(LogEntryArray[i].entryData1);
         Serial.print(":");
         Serial.print(LogEntryArray[i].entryRead);
         Serial.println();
      }
      
    }
  
}
