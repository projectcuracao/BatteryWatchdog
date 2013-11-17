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
 Serial.print("-Pi="); 
 Serial.print(isPiOn); 
 Serial.println("");
  
}


void return2Digits(char returnString[], char *buffer, int digits)
{
  if(digits < 10)
    sprintf(returnString,"0%i", digits);
  else
    sprintf(returnString,"%i", digits);
    
  strcpy(returnString, buffer);
    

}

void buildTimeString(char returnString[], char *buffer, DateTime convertTime)
{
  
   
    
    char myBuffer[5];
    sprintf(myBuffer,"%i-", convertTime.year());
    strcat(returnString, myBuffer);

    return2Digits(myBuffer, myBuffer, convertTime.month());
    strcat(returnString, myBuffer);
    strcat(returnString, "-");
 
    return2Digits(myBuffer, myBuffer, convertTime.day());
    strcat(returnString, myBuffer);
    strcat(returnString, " ");
    
    return2Digits(myBuffer, myBuffer, convertTime.hour());
    strcat(returnString, myBuffer);
    strcat(returnString, ":");
    
    return2Digits(myBuffer, myBuffer, convertTime.minute());
    strcat(returnString, myBuffer);
    strcat(returnString, ":");
    
    return2Digits(myBuffer, myBuffer, convertTime.second());
    strcat(returnString, myBuffer);
  
  
  
}

void readStateValues()
{
    Adafruit_INA219 ina219x40(0x40);
  
  // state2 reads all I2C data
  Serial.println("Entering STATE2");
  
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
     
       char timeNow[20];
       timeNow[0] = '\0';
       buildTimeString(timeNow, timeNow, RTC.now());
    
       Serial.print("2-0-");
       Serial.print(timeNow);
       Serial.println();

       byteCount = Serial2.readBytesUntil('\n', myBuffer, 200);
       
      
       timeNow[0] = '\0';
       buildTimeString(timeNow, timeNow, RTC.now());
    
       Serial.print("2-1-");
       Serial.print(timeNow);
       Serial.println();

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
     

       timeNow[0] = '\0';
       buildTimeString(timeNow, timeNow, RTC.now());
    
       Serial.print("2-2-");
       Serial.print(timeNow);
       Serial.println();

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
    DateTime now = RTC.now();

    char timeNow[20];
    timeNow[0] = '\0';
    buildTimeString(timeNow, timeNow, now);
    
    Serial.print(timeNow);
    Serial.println();
    Serial.print("FreeMemory=");
    Serial.println(freeMemory());
    Serial.print("PiBatteryVoltage =");
    Serial.println(getAnalogVoltage(PiBatteryVoltageChannel));
    
 
 
 if (requestFromPi == true)
 {  
   requestFromPi = false;
   digitalWrite(indicatorLED, false);  
   return STATE1;
 }
   
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

   
   // timeout value  msec
   long timeout = 10000;
   long startTime = millis();
   
   while (true)
   {
       int byteCount;
       buffer[0] = '\0';
     
       char timeNow[20];
       timeNow[0] = '\0';
       buildTimeString(timeNow, timeNow, RTC.now());
    
       Serial.print("0-");
       Serial.print(timeNow);
       Serial.println();

       byteCount = Serial2.readBytesUntil('\n', buffer, 50);
       
      
       timeNow[0] = '\0';
       buildTimeString(timeNow, timeNow, RTC.now());
    
       Serial.print("1-");
       Serial.print(timeNow);
       Serial.println();

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
         
           char myBootDate[20];
           myBootDate[0] = '\0';
           buildTimeString(myBootDate, myBootDate, lastBoot);
      
           strcat(returnString, myBootDate);
           strcat(returnString, "',");
           
  

           
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
                     
                     RTC.adjust(DateTime(returnString, returnString2));
       
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
            Serial.println(subStr(returnString, ",", 2));
            Serial.println(subStr(returnString, ",", 3));

            Serial.println(subStr(returnString, ",", 4));
            Serial.println(subStr(returnString, ",", 5));
            Serial.println(subStr(returnString, ",", 6));
        
            Serial.println(subStr(returnString, ",", 7));
            Serial.println(subStr(returnString, ",", 8));
            Serial.println(subStr(returnString, ",", 9));
        
            Serial.println(subStr(returnString, ",", 10));

        

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
       
       char timeNow[20];
       timeNow[0] = '\0';
       buildTimeString(timeNow, timeNow, RTC.now());
    
       Serial.print("2-");
       Serial.print(timeNow);
       Serial.println();

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



int state3(int state)
{

  
  
   return STATE0; 
}


