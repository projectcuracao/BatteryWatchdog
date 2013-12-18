//
// state machines for Battery Watchdog
//

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
 Serial.print("-iPiS="); 
 Serial.print(isPiSignaledWD); 
 Serial.print("-WAS=");
 Serial.print(WAState);
 Serial.print("-LITP=");
 Serial.print(lastInterruptToPi);
                 
 
 Serial.println("");
 Serial.print("Time to WD:");
 if (enableWatchDog == false)
   Serial.println("WD Disabled");
 else
 Serial.println(watchDogTime - RTC.get());
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

int readNextLineFromPi(char returnString[], char *buffer)
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
    
    Serial.print(timeNow);
    Serial.println();
    Serial.print("FreeMemory=");
    Serial.println(freeMemory());
    Serial.print("PiBatteryVoltage =");
    Serial.println(getAnalogVoltage(PiBatteryVoltageChannel));
    
    displayLog();
    
    writeLogEntry( 1, 2, 3);
 
 if (requestFromPi == true)
 {  
   requestFromPi = false;
   digitalWrite(indicatorLED, false);  
   return STATE1;
 }
 

 #ifdef USEWATCHDOG
  if ((RTC.get() > watchDogTime ) && (enableWatchDog == true))
  {
    // we need to reboot machine
    
          if (watchDogState == true)
          {
            
              // try restart, then power cycle if no response
              watchDogState = false;
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
                watchDogTime = RTC.get() + watchDogTimeIncrement;
              }
              if (lastInterruptToPi == SHUTDOWN)
              {
                Serial.println("Shutdown-TURN PI OFF");
                turnPiOff();
                lastInterruptToPi = NOINTERRUPT;
                WAState = NOINTERRUPT;
                watchDogState = true;
                enableWatchDog = false;
                watchDogTime = RTC.get() + watchDogTimeIncrement;
              }
  
             
          }

  }
  else
  {
    // no watch dog expiration
    if ((WAState == REBOOT)  || (WAState == SHUTDOWN))
        Serial.println("---->watchdog expired.  Waiting to power cycle");
      else
        Serial.println("---->no watchdog expiration");
    
  }
#else
  Serial.println("---->WATCHDOG DISABLED");
#endif

   
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
   char buffer[50];

      
    tmElements_t tm;
    RTC.read(tm);

   // timeout value  msec
   long timeout = 10000;
   long startTime = millis();
   
   while (true)
   {
       int byteCount;
       buffer[0] = '\0';
     
  
 
       byteCount = Serial2.readBytesUntil('\n', buffer, 50);
       

     if (byteCount > 0)
     {
       buffer[byteCount] = 0;
     }

     Serial.print("count=");
     Serial.print(byteCount);
     Serial.print("command =");
     Serial.println(buffer);
     
     if (byteCount > 1)
     {
         // reset time out
         startTime = millis();
         // check for command
         
         Serial.print("inside bytecount buffer=");
         Serial.println(buffer);
         if (strcmp(buffer, "RD")  == 0)   // ready command - send back OK
         {
           Serial2.write("OK\n");
         }
         
         if (strcmp(buffer, "GB")  == 0)   // Goodbye, leave state
         {
           Serial2.write("OK\n");
           break;
         }
         
         if (strcmp(buffer, "GD")  == 0)   // Get Data
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
          Serial2.write(returnString);
           
         }
         
         
         if (strcmp(buffer, "GTH")  == 0)   // Get Outside Temp Humidity
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
         
         if (strcmp(buffer, "WA")  == 0)   // Why did you interrupt me?
         {
            char returnString[200];
           returnString[0] = '\0';
           
           sprintf(returnString, "%i", WAState);
           Serial.print("WAState=");
           Serial.print(WAState);
           
  

           Serial2.write(returnString);
           
         }
         
         if (strcmp(buffer, "WD")  == 0)   // Watchdog
         {


           Serial2.write("OK\n");
           
          watchDogState= true; // true - ok, false reboot
          watchDogTime=RTC.get() + watchDogTimeIncrement;
           
         }
         
         if (strcmp(buffer, "AWA")  == 0)   // Acknowledge the WA command
         {
            Serial2.write("OK\n");
            WAState = NOINTERRUPT;
            
            digitalWrite(interruptPi, false); // make sure the line is low now after acknowlegement
   
           
         }
         
         
         
         if (strcmp(buffer, "ST")  == 0)   // Set UTC time on Arduino
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
                     
                    if (getDate(returnString, tm) && getTime(returnString2, tm)) {

                      // and configure the RTC with this info
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
         
        if (strcmp(buffer, "STH")  == 0)   // Set thresholds on Arduino
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
            strcpy(subStr(returnString, ",", 7), PI_START_TIME);
            Serial.println(subStr(returnString, ",", 8));
            strcpy(subStr(returnString, ",", 8), PI_SHUTDOWN_TIME);

            Serial.println(subStr(returnString, ",", 9));
            PI_MIDNIGHT_WAKEUP_SECONDS_LENGTH = atof(subStr(returnString, ",", 9)); 
            Serial.println(subStr(returnString, ",", 10));
            PI_MIDNIGHT_WAKEUP_THRESHOLD  = atof(subStr(returnString, ",", 10));
           
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



