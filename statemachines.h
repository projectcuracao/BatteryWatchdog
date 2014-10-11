//
// state machines for Battery Watchdog
//



void selectSolarOrWind()
{
  
  // right now, we just put wind on at night and solar during day
  long CurrentTime = readRTCreliably();
  
  
  
  
  
  
}
// print state

void printStateVariables()
{
 
 Serial.print("CS=");
 Serial.print(currentState);
 Serial.print("-NS=");
 Serial.print(nextState);
 Serial.print("-PR=");
 Serial.print(requestFromPi);
 Serial.print("-WD=");
 Serial.print(watchDogState);
Serial.print("-WDE=");
 Serial.print(enableWatchDog);
 Serial.print("-Pi="); 
 Serial.print(isPiOn);
Serial.print("-SBPiOn="); 
 Serial.print(shouldPiBeOn);  
 Serial.print("-iPiS="); 
 Serial.print(isPiSignaledWD); 
 Serial.print("-WAS=");
 Serial.print(WAState);
 Serial.print("-LITP=");
 Serial.print(lastInterruptToPi);
 Serial.print("-PiBV=");
 Serial.print(PiBatteryVoltage);
                 
 
 Serial.println("");
 Serial.print("Time to WD:");
 if (enableWatchDog == false)
   Serial.println("WD Disabled");
 else
 {
   Serial.println(watchDogTime - readRTCreliably());
 }
}


void readStateValues()
{
    Adafruit_INA219 ina219x40(0x40);
  
  // reads all I2C data
  Serial.println("Entering readStateValues");
  
  // USB Arduino Power
  ina219x40.begin();


  Arduinoshuntvoltagex40 = ina219x40.getShuntVoltage_mV();
  Arduinobusvoltagex40 = ina219x40.getBusVoltage_V();
  Arduinocurrent_mAx40 = ina219x40.getCurrent_mA();
  Arduinoloadvoltagex40 = Arduinobusvoltagex40 + (Arduinoshuntvoltagex40 / 1000);
  Serial.println("Arduino");
  Serial.print("x40Bus Voltage:   "); Serial.print(Arduinobusvoltagex40); Serial.println(" V");
  Serial.print("x40Shunt Voltage: "); Serial.print(Arduinoshuntvoltagex40); Serial.println(" mV");
  Serial.print("x40Load Voltage:  "); Serial.print(Arduinoloadvoltagex40); Serial.println(" V");
  Serial.print("x40Current:       "); Serial.print(Arduinocurrent_mAx40); Serial.println(" mA");
  Serial.println("");
  
  Adafruit_INA219 ina219x41(0x41);
  

  //Solar Cell Power
  ina219x41.begin();


  SolarCellshuntvoltagex41 = ina219x41.getShuntVoltage_mV();
  SolarCellbusvoltagex41 = ina219x41.getBusVoltage_V();
  
  SolarCellcurrent_mAx41 = -ina219x41.getCurrent_mA();  // since wire is backwards on board
  
  SolarCellloadvoltagex41 = SolarCellbusvoltagex41 + (SolarCellshuntvoltagex41 / 1000);
  Serial.println("Solar Cell");
  Serial.print("x41Bus Voltage:   "); Serial.print(SolarCellbusvoltagex41); Serial.println(" V");
  Serial.print("x41Shunt Voltage: "); Serial.print(SolarCellshuntvoltagex41); Serial.println(" mV");
  Serial.print("x41Load Voltage:  "); Serial.print(SolarCellloadvoltagex41); Serial.println(" V");
  Serial.print("x41Current:       "); Serial.print(SolarCellcurrent_mAx41); Serial.println(" mA");
  Serial.println("");
  
  
  // Battery cell
  Adafruit_INA219 ina219x44(0x44);
  

  
  ina219x44.begin();


  Batteryshuntvoltagex44 = ina219x44.getShuntVoltage_mV();
  Batterybusvoltagex44 = ina219x44.getBusVoltage_V();
  Batterycurrent_mAx44 = -ina219x44.getCurrent_mA();
  Batteryloadvoltagex44 = Batterybusvoltagex44 + (Batteryshuntvoltagex44 / 1000);
  Serial.println("Battery");
  Serial.print("x44Bus Voltage:   "); Serial.print(Batterybusvoltagex44); Serial.println(" V");
  Serial.print("x44Shunt Voltage: "); Serial.print(Batteryshuntvoltagex44); Serial.println(" mV");
  Serial.print("x44Load Voltage:  "); Serial.print(Batteryloadvoltagex44); Serial.println(" V");
  Serial.print("x44Current:       "); Serial.print(Batterycurrent_mAx44); Serial.println(" mA");
  Serial.println("");
  
  
  Adafruit_AM2315 am2315;
  am2315.begin();
  /*  
  am2315Humidity = am2315.readHumidity();
  delay(1000);
  am2315Temperature = am2315.readTemperature();
  delay(1000);
  */
  // read twice, sensor doesn't wake up from power mode without it
  
  am2315Humidity = am2315.readHumidity();
  delay(1000);
  am2315Temperature = am2315.readTemperature();
  
  Serial.print("Hum: "); Serial.println(am2315Humidity);
  Serial.print("Temp: "); Serial.println(am2315Temperature);
  
  batteryThermistor = readThermistor(A0);

  
}

//
//
// Serial Routines
//
//
#define OK 0
#define TIMEOUT 1

int readNextLineFromPi(char returnString[], char *buffer2)
{
  
     
   char myBuffer[200];
   // timeout value  msec
   long timeout = 10000;
   long startTime = millis();
   
   while (true)
   {
     int byteCount;
       myBuffer[0] = '\0';
     
     

       byteCount = Serial2.readBytesUntil('\n', myBuffer, 200);


     if (byteCount > 0)
     {
       myBuffer[byteCount] = 0;
     }

     Serial.print("count=");
     Serial.print(byteCount);
     Serial.print("Line =");
     Serial.println(myBuffer);
     
     if (byteCount > 1)
     {
         // reset time out
         startTime = millis();
         // check for command
         strcat(returnString, myBuffer); 
         return OK;
     }
     



       // check time out
      if (millis() > (startTime + timeout))
      {
        // we timed out
        Serial.print("startTime =");
        Serial.print(startTime);
        Serial.print(" millis()");
        Serial.print(millis());
        
        Serial.print(" timeout - no characters before:");
        Serial.print(timeout);
        Serial.println("msec");
        return TIMEOUT;
      }
   }
  
  
}

//
//
// State Variables


// initial state is STATE0

// run state
int state0(int state)
{
    Serial.println("Entering STATE0");
    tmElements_t tm;
    RTC.read(tm);


    char timeNow[20];
    timeNow[0] = '\0';
    buildTimeString(timeNow, timeNow, tm);

    Serial.print("---------------------->");
    Serial.print(timeNow);
    Serial.println();
    Serial.print("FreeMemory=");
    Serial.println(freeMemory());
    Serial.print("Instant PiBatteryVoltage =");
    float temp;
    delay(10);
    temp = getAnalogVoltage(PiBatteryVoltageChannel);
    Serial.println(temp);
    delay(10);
    delay(100);
    Serial.print("Rolling PiBatteryVoltage =");
    Serial.println(PiBatteryVoltage);
    Serial.print("Instant PiBatteryVoltage=");
    temp = getAnalogVoltage(PiBatteryVoltageChannel);
    Serial.println(temp);
    
    PiBatteryVoltage = rollingAverage(runningPiVoltage, CNT, temp);

     //displayLog();
    
    // multiple channel reading (from http://forum.arduino.cc/index.php?topic=54976.0)
    setpiVoltageStartupThresholdsOK(PiBatteryVoltage);
    delay(10);
    UnregulatedWindVoltage = getAnalogVoltage(UnregulatedWindVoltageChannel);
    delay(10);
    UnregulatedWindVoltage = getAnalogVoltage(UnregulatedWindVoltageChannel)/UnregulatedWindVoltageMultiplier;
    delay(10);
    RegulatedWindVoltage = getAnalogVoltage(RegulatedWindVoltageChannel);
    delay(10);
    RegulatedWindVoltage = getAnalogVoltage(RegulatedWindVoltageChannel)/RegulatedWindVoltageMultiplier;
    delay(10);
    PiSolarVoltage = getAnalogVoltage(PiSolarVoltageChannel);
    delay(10);
    PiSolarVoltage = getAnalogVoltage(PiSolarVoltageChannel)/PiSolarVoltageMultiplier;
    delay(10);


    
    Serial.print("Instant UnregulatedWindVoltage=");
    Serial.println(UnregulatedWindVoltage);
    Serial.print("Instant RegulatedWindVoltage=");
    Serial.println(RegulatedWindVoltage);
    Serial.print("Instant PiSolarVoltage=");
    Serial.println(PiSolarVoltage);
    //Serial.print("WindTurbineVoltageToMPH=");
    //Serial.println(convertWindTurbineVoltageToMPH(UnregulatedWindVoltage));

  



 
 if (requestFromPi == true)
 {  
   requestFromPi = false;
   digitalWrite(indicatorLED, false);  
   return STATE1;
 }
 
 // if the shutdown threshold has been hit, force watchdog to fire and shutdown orderly
 
 // check for battery voltage shutdown
 if ((piVoltageShutdownThresholdOK == false) && (isPiOn == true))
 {
      
      Serial.println("Shutdown Pi from battery low");
      piVoltageShutdownTime = readRTCreliably() % 86400 + 200L;  // set alarm two hundred seconds in the future
      
      // tell pi to shutdown
      watchDogState = false;
      WAState = SHUTDOWN;
      lastInterruptToPi = SHUTDOWN;
      Serial.println("---->try shutdown, then Turn Off");
      doInterruptPi();
      
      
      writeLogEntry( LOGINFO, LOGPiOffLowVoltage, 1);
         
 }
 
 // check to see if pi should be on and turn on if voltage is OK (recovery during day from low battery)
 
 if ((shouldPiBeOn == true) && (isPiOn == false))
 {
   // check for recovery voltage
   if (piVoltageStartupThresholdOK == true)
   {
      Serial.println("---->Voltage Recovery, Turn On");
      PiBoot();
     writeLogEntry( LOGINFO, LOGPiOnVoltageRecovery, 1);
   }
   
 }
 

 #ifdef USEWATCHDOG
  if ((readRTCreliably() > watchDogTime ) && (enableWatchDog == true))
  {
    // we need to reboot machine
    
          if (watchDogState == true)
          {
            
              // try restart, then power cycle if no response
              watchDogState = false;

              writeLogEntry( LOGINFO, LOGWatchDogTriggered, 1); 
              WAState = REBOOT;
              lastInterruptToPi = REBOOT;
              Serial.println("---->try reboot, then power cycle if no response");
              doInterruptPi();
              // give us ten minutes to wait
              Serial.println("---->giving five minutes for pi to reboot and send watchdog");
              watchDogTime = watchDogTime + 300L;
            }
          else
          {
              // already rebooted
              Serial.println("---->watchdog has already expired or Pi is being turned off");
              // last Interrupt to PI was reboot or shutdown and we havin't started watchdog yet, so power cycle
              // if was reboot, then power cycle Pi
              // if was shutdown, turn it off and disable watchdog until power cycle on
              
              if (lastInterruptToPi == REBOOT)
              {
                Serial.println("Reboot-POWER CYCLE PI");
               
                turnPiOff();
                delay(10000);
                turnPiOn();
                lastInterruptToPi = NOINTERRUPT;
                WAState = NOINTERRUPT;
                watchDogState = true;
                enableWatchDog = true;
                watchDogTime = readRTCreliably() + watchDogTimeIncrement;
              }
              if (lastInterruptToPi == SHUTDOWN)
              {
                Serial.println("Shutdown-TURN PI OFF");
                turnPiOff();
                lastInterruptToPi = NOINTERRUPT;
                WAState = NOINTERRUPT;
                watchDogState = true;
                enableWatchDog = false;
                watchDogTime = readRTCreliably() + watchDogTimeIncrement;
              }
  
             
          }

  }
  else
  {
    // no watch dog expiration
    if ((WAState == REBOOT)  || (WAState == SHUTDOWN))
    {
        Serial.print("---->watchdog expired or disabled. enableWatchDog =");
        Serial.println(enableWatchDog);
        Serial.println("  Waiting to power cycle");
    }
      else
        Serial.println("---->no watchdog expiration");
    
  }
#else
  Serial.println("---->WATCHDOG DISABLED");
#endif

   // Now determine the wind or sun
   selectSolarOrWind();
   
   return STATE0; 
}

//
//
//
// fufill request from pi state1
//
//
//
//



int state1(int state)
{
   Serial.println("Entering STATE1");
   
   // get the command from the Pi
   byte number = 0;
   
   Serial2.begin(9600);
   char buffer2[50];

      
    tmElements_t tm;
    RTC.read(tm);

   // timeout value  msec
   long timeout = 10000;
   long startTime = millis();
   
   while (true)
   {
       int byteCount;
       buffer2[0] = '\0';
     
  
 
       byteCount = Serial2.readBytesUntil('\n', buffer2, 50);
       

     if (byteCount > 0)
     {
       buffer2[byteCount] = 0;
     }

     Serial.print("count=");
     Serial.print(byteCount);
     Serial.print("command =");
     Serial.println(buffer2);
     
     if (byteCount > 1)
     {
         // reset time out
         startTime = millis();
         // check for command
         

         if (strcmp(buffer2, "RD")  == 0)   // ready command - send back OK
         {
           Serial2.write("OK\n");
           readArduinoSL = false;

         }
      
         if (strcmp(buffer2, "GB")  == 0)   // Goodbye, leave state
         {
           Serial2.write("OK\n");
           
           if (readArduinoSL == true)
             convertLogTriedToReadToSuccess();
           else
             convertLogTriedToReadToNotRead();
             
           readArduinoSL = false;
           
           break;
         }
         
         if (strcmp(buffer2, "GD")  == 0)   // Get Data
         {
           
           readStateValues();
           
           char floatString[15];
           char returnString[200];
           
           returnString[0] = '\0';
           floatString[0] = '\0';
           dtostrf(Arduinoloadvoltagex40,6,2,floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");
           
           floatString[0] = '\0';
           dtostrf(Arduinocurrent_mAx40,6,2,floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");

           
           floatString[0] = '\0';
           dtostrf(SolarCellloadvoltagex41,6,2,floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");

           
           floatString[0] = '\0';
           dtostrf(SolarCellcurrent_mAx41,6,2,floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");

           
           floatString[0] = '\0';
           dtostrf(Batteryloadvoltagex44,6,2,floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");

           
           floatString[0] = '\0';
           dtostrf(Batterycurrent_mAx44,6,2,floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");

           
           floatString[0] = '\0';
           dtostrf(am2315Humidity,6,2,floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");

           
           floatString[0] = '\0';
           dtostrf(am2315Temperature,6,2,floatString);
           strcat(returnString, floatString);
           
           // last reboot
           strcat(returnString, ",'");
         
           char myBootDate[25];
           myBootDate[0] = '\0';
           breakTime(lastBoot, tm);
           buildTimeString(myBootDate, myBootDate, tm);
      
           strcat(returnString, myBootDate);
           strcat(returnString, "',");
    
           Serial.print("LB:");
           Serial.print(myBootDate);
           Serial.println();

  

           
           floatString[0] = '\0';
           dtostrf(batteryThermistor,6,2,floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");
           
           
           floatString[0] = '\0';
           sprintf(floatString, "%i", freeMemory());
           Serial.print("freeMemory =");
           Serial.println(floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");           
           
           floatString[0] = '\0';
           dtostrf(UnregulatedWindVoltage,6,2,floatString);
           Serial.print("UnregulatedWindVoltage =");
           Serial.println(floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");
           
           
           floatString[0] = '\0';
           dtostrf(RegulatedWindVoltage,6,2,floatString);
           Serial.print("RegulatedWindVoltagey =");
           Serial.println(floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");

          
           floatString[0] = '\0';
           sprintf(floatString, "%i", solarWind);
           Serial.print("solarWind =");
           Serial.println(floatString);
           strcat(returnString, floatString);
           
           strcat(returnString, ",");
           floatString[0] = '\0';
           dtostrf(PiBatteryVoltage,6,2,floatString);
           strcat(returnString, floatString);

 
           Serial2.write(returnString);
           
         } // end of GD
         
         
    

           
 
         
         if (strcmp(buffer2, "SL")  == 0)   // Get Data
         {
  
           char returnString[200];
           
           returnString[0] = '\0';
          
          
          readArduinoSL = true;
          int unreadCount;
           // send count
           unreadCount = returnUnreadCount();
           sprintf(returnString, "%i\n", unreadCount);
           Serial.print("unreadCount=");
           Serial.println(unreadCount);

           Serial2.write(returnString);
           // loop through log entries
          
           int index;
           index = fetchNextUnreadEntry(); 
           while (index != -1)
           {
             char buffer2[30];
             buffer2[0] = '\0';
             returnString[0] = '\0';
             tmElements_t tm;
             breakTime(LogEntryArray[index].entryTime, tm);
             buildTimeString(buffer2, buffer2, tm );
   
             sprintf(returnString, "%s,%i,%i,%i\n", buffer2, LogEntryArray[index].entryType, LogEntryArray[index].entryData0, LogEntryArray[index].entryData1);
             Serial2.write(returnString); 
             index = fetchNextUnreadEntry();        
             
            }
         } // end of SL
         
         
          if (strcmp(buffer2, "CLF")  == 0)   // Clear FRAM Log
         {
           initializeFRAMTable(104); // initialize with 104 size
           
         }  // end of CLF

         
         if (strcmp(buffer2, "SLF")  == 0)   // Send FraM Log
         {
  
           char returnString[200];
           
           returnString[0] = '\0';
          
          
       
          int unreadCount;
           // send count
           unreadCount = returnFramUnreadCount();
           sprintf(returnString, "%i\n", unreadCount);
           Serial.print("Fram unreadCount=");
           Serial.println(unreadCount);

           Serial2.write(returnString);
           // loop through log entries
          
           int index;
           index = FramfetchNextUnreadEntry(); 
           while (index != -1)
           {
             char buffer2[110];
             buffer2[0] = '\0';
             returnString[0] = '\0';

             readFramItem (buffer2, buffer2, index, 1);
   
             sprintf(returnString, "%s\n", buffer2);
             Serial2.write(returnString); 
             index = FramfetchNextUnreadEntry();        
             
            }
         } // end of SLF

         if (strcmp(buffer2, "GTH")  == 0)   // Get Outside Temp Humidity
         {
           
           readStateValues();
           
           char floatString[15];
           char returnString[200];
           
 
           returnString[0] = '\0';
           floatString[0] = '\0';
           dtostrf(am2315Temperature,6,2,floatString);
           strcat(returnString, floatString);
           strcat(returnString, ",");

           
           floatString[0] = '\0';
           dtostrf(am2315Humidity,6,2,floatString);
           
  
           strcat(returnString, floatString);
 
          Serial2.write(returnString);
           
         }
         
         if (strcmp(buffer2, "WA")  == 0)   // Why did you interrupt me?
         {
            char returnString[200];
           returnString[0] = '\0';
           
           sprintf(returnString, "%i", WAState);
           Serial.print("WAState=");
           Serial.print(WAState);
           
  

           Serial2.write(returnString);
           
         }
         
         if (strcmp(buffer2, "WD")  == 0)   // Watchdog
         {


           Serial2.write("OK\n");
           if ((watchDogState == false) && (lastInterruptToPi == SHUTDOWN))
           {
             // don;t reset WD.  We are in the middle of a shutdown
           }
           else
           {
             // reset Watchdog
             
            watchDogState= true; // true - ok, false shutdown
            watchDogTime=readRTCreliably() + watchDogTimeIncrement;
           }
           
         }
         
         if (strcmp(buffer2, "SS")  == 0)   // Watchdog
         {


           Serial2.write("OK\n");
           
           selectSolar();
           
         }
         
         if (strcmp(buffer2, "SW")  == 0)   // Watchdog
         {


           Serial2.write("OK\n");
           
           selectWind();
           
         }
         
         if (strcmp(buffer2, "AWA")  == 0)   // Acknowledge the WA command
         {
            Serial2.write("OK\n");
            WAState = NOINTERRUPT;
            
            digitalWrite(interruptPi, false); // make sure the line is low now after acknowlegement
   
           
         }
         
         
         
         if (strcmp(buffer2, "ST")  == 0)   // Set UTC time on Arduino
         {
           

           char returnString[200];
           char returnString2[200];
           
           Serial2.write("OK\n");

           returnString[0] = '\0';
  
           int result =  readNextLineFromPi(returnString, returnString);
           
           if (result == OK)
           {
                 Serial2.write("OK\n");
       
                 returnString2[0] ='\0';
                 
                 result =  readNextLineFromPi(returnString2, returnString2);
           
            
                 if (result == OK)
                 {
                     Serial2.write("OK\n");
                     
                     Serial.print("sentTime=");
                     Serial.print(returnString);
                     Serial.print(" ");
                     Serial.println(returnString2);
                     tmElements_t tm;
                     
 //                   if (getDate(returnString, tm) && getTime(returnString2, tm))
                    {
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  sscanf(returnString, "%s %d %d", Month, &Day, &Year);
  Serial.print("MDY=");
  Serial.print(Month);
  Serial.print(Day);
  Serial.println(Year);
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  

  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  
    int Hour, Min, Sec;

  sscanf(returnString2, "%d:%d:%d", &Hour, &Min, &Sec);
 
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec; 


                      // and configure the RTC with this info
                        Serial.print("tm2=");
                        Serial.print(tm.Day);
                        Serial.print("/");
                        Serial.print(tm.Month);
                        Serial.print("/");
                        Serial.println(tm.Year);  
                        Serial.print(tm.Hour);
                        Serial.print(":");
                        Serial.print(tm.Minute);
                        Serial.print(":");
                        Serial.println(tm.Second);
                        
                      char timeNow[30];
                      timeNow[0] = '\0';
                      buildTimeString(timeNow, timeNow, tm);
                      Serial.print("builtTime=");
                      Serial.println(timeNow);
 
                      RTC.write(tm);
                   
                     }
  

       
                     returnString[0] ='\0';
                     returnString2[0] ='\0';
                 }
                 else
                 {
                     Serial2.write("FAILED\n");
                     returnString[0] ='\0';
                     returnString2[0] ='\0';
             
                 }
           }
           else
           {
                 Serial2.write("FAILED\n");
                 returnString[0] ='\0';
                 returnString2[0] ='\0';
             
           }

           
         } // end of ST
 
          if (strcmp(buffer2, "RT")  == 0)   // Read UTC time on Arduino
         {
           


           char returnString[200];
           returnString[0] = '\0';
           
           tmElements_t tm;
           RTC.read(tm);


           char timeNow[20];
           timeNow[0] = '\0';
           buildTimeString(timeNow, timeNow, tm);
          
           sprintf(returnString, "%s", timeNow);
  
  

           Serial2.write(returnString);

           
         } // end of RT
         
        if (strcmp(buffer2, "STH")  == 0)   // Set thresholds on Arduino
         {
           

           char returnString[200];

           
           Serial2.write("OK\n");

           returnString[0] = '\0';
  
           int result =  readNextLineFromPi(returnString, returnString);
           
           if (result == OK)
           {
             Serial.print("STH - received=");
             Serial.println(returnString);
             Serial2.write("OK\n");
             
             // process the data
  

            // first "3.706, 3.839, 49.330, 37.780, 98.000, 93.000, '10:00:00', '22:30:00', 3600.000, 3.971\n")
            

            Serial.println(subStr(returnString, ",", 1));
            PI_BATTERY_SHUTDOWN_THRESHOLD = atof(subStr(returnString, ",", 1));
            Serial.println(subStr(returnString, ",", 2));
            PI_BATTERY_STARTUP_THRESHOLD = atof(subStr(returnString, ",", 2)); 
            Serial.println(subStr(returnString, ",", 3));
            INSIDE_TEMPERATURE_PI_SHUTDOWN_THRESHOLD = atof(subStr(returnString, ",", 3)); 
            Serial.println(subStr(returnString, ",", 4));
            INSIDE_TEMPERATURE_PI_STARTUP_THRESHOLD = atof(subStr(returnString, ",", 4)); 
            Serial.println(subStr(returnString, ",", 5));
            INSIDE_HUMIDITY_PI_SHUTDOWN_THRESHOLD = atof(subStr(returnString, ",", 5)); 
            Serial.println(subStr(returnString, ",", 6));
            INSIDE_HUMIDITY_PI_STARTUP_THRESHOLD = atof(subStr(returnString, ",", 6)); 
            
            Serial.println(subStr(returnString, ",", 7));
            strcpy(PI_START_TIME, subStr(returnString, ",", 7));
            Serial.print("Incoming PI_START_TIME=");
            Serial.println(subStr(returnString, ",", 7));
            Serial.print("New PI_START_TIME=");
            Serial.println(PI_START_TIME);
            Serial.println(subStr(returnString, ",", 8));

            strcpy(PI_SHUTDOWN_TIME, subStr(returnString, ",", 8));

            Serial.println(subStr(returnString, ",", 9));
            PI_MIDNIGHT_WAKEUP_SECONDS_LENGTH = atof(subStr(returnString, ",", 9)); 
            Serial.println(subStr(returnString, ",", 10));
            PI_MIDNIGHT_WAKEUP_THRESHOLD  = atof(subStr(returnString, ",", 10));
            Serial.println(subStr(returnString, ",", 11));
            enableShutdowns  = atoi(subStr(returnString, ",", 11));
         
            // Ok based on these constants, let's update the alarms
 
 
            setAlarmTimes();  // set up my alarms
            
        

           }
           else
           {
                 Serial2.write("FAILED\n");
                 returnString[0] ='\0';

             
           }

           
         } // end of STH


     }
     else
     {
       


       // check time out
      if (millis() > (startTime + timeout))
      {
        // we timed out
        Serial.print("startTime =");
        Serial.print(startTime);
        Serial.print(" millis()");
        Serial.print(millis());
        
        Serial.print(" timeout - no characters before:");
        Serial.print(timeout);
        Serial.println("msec");
        break;
      }
     }

   }
  Serial2.end(); 
  requestFromPi = false;
  digitalWrite(indicatorLED, false);  
  
  return STATE0; 
}



int state2(int state)
{

  
  
   return STATE0; 
}



int state3(int state)
{

  
  
   return STATE0; 
}



