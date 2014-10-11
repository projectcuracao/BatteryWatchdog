
// Filename WatchdogBattery.ino
// Version 1.0 09/17/13 JS MiloCreek
//
#define VERSIONNUMBER 2.0
// Normal run state
#define STATE0   0
//  Communicate with Pi and WD Timer
#define STATE1   1
// if PiOff, check for restart conditions
// if PiOn, check for shutdown conditions

#define STATE2   2

#define STATE3   3

// Interrupt handlers


#define LOGCRITICAL 50
#define LOGERROR 40
#define LOGWARNING 30
#define LOGINFO 20
#define LOGDEBUG 10
#define LOGNOTSET 0


// defines for WA command
#define NOINTERRUPT 0
#define NOREASON 1
#define SHUTDOWN 2
#define GETLOG 3
#define ALARM1 4
#define ALARM2 5
#define ALARM3 6
#define REBOOT 7

int WAState;

#define USEWATCHDOG



#include "Sleep_n0m1.h"
#include <Wire.h>

#include <DS1307RTC.h>
#include <Time.h>
// #include <TimeAlarms.h>
#include "Adafruit_INA219.h"
#include <Adafruit_AM2315.h>

#include "MemoryFree.h"
#include "average.h"


int count = 0;

unsigned long sleepTime; //how long you want the arduino to sleep

// Pin Values
//#define relay 4
#define indicatorLED 35
#define PiRelaySet 29
#define PiRelayReset 27
#define selectSolarReset 43
#define selectWindSet 41
#define PiBatteryVoltageChannel A3
#define UnregulatedWindVoltageChannel A7
#define UnregulatedWindVoltageMultiplier 0.275
#define RegulatedWindVoltageChannel A5
#define RegulatedWindVoltageMultiplier 0.721
#define PiSolarVoltageChannel A9
#define PiSolarVoltageMultiplier 0.720
#define interruptPi 22

DS1307RTC MYRTC;

int currentState;
int nextState;

volatile int requestFromPi;
volatile int watchDogState; // true - ok, false reboot
time_t  watchDogTime;
long watchDogTimeIncrement;
volatile int isPiOn;
int shouldPiBeOn;
int lastInterruptToPi;
int enableWatchDog;   // disable watchdog during pi off times
int enableShutdowns;  //enable/disable all shutdonwms

int solarWind; // 0 for solar, 1 for wind
// log stuff

#include <Adafruit_FRAM_I2C.h>

Adafruit_FRAM_I2C fram     = Adafruit_FRAM_I2C();

struct LogEntry {
  time_t entryTime;
  int entryType;
  int entryData0;
  int entryData1;
  int entryRead;
};

int LogEntryNextItem;

#define LOGENTRYTABLESIZE 50

LogEntry LogEntryArray[LOGENTRYTABLESIZE];

long piOnTime;
long piOffTime;
long piMidnightOnTime;
long piMidnightOffTime;
long piVoltageShutdownTime;
long piSunriseTime;
long piSunsetTime;
long arduinoDeadManSwitchTime;

bool piOnTimeState;
bool piOffTimeState;
bool piMidnightOnTimeState;
bool piMidnightOffTimeState;
bool piSunriseTimeState;
bool piSunsetTimeState;

bool piVoltageShutdownTimeState;

bool arduinoDeadManSwitchState;

bool piVoltageStartupThresholdOK;
bool piVoltageShutdownThresholdOK;

volatile int isPiSignaledWD;
//
//
// State Values
//
//
  float Arduinoshuntvoltagex40 = 0;
  float Arduinobusvoltagex40 = 0;
  float Arduinocurrent_mAx40 = 0;
  float Arduinoloadvoltagex40 = 0;
  
  float SolarCellshuntvoltagex41 = 0;
  float SolarCellbusvoltagex41 = 0;
  float SolarCellcurrent_mAx41 = 0;
  float SolarCellloadvoltagex41 = 0; 
  
  float Batteryshuntvoltagex44 = 0;
  float Batterybusvoltagex44 = 0;
  float Batterycurrent_mAx44 = 0;
  float Batteryloadvoltagex44 = 0;
  
  float am2315Humidity= 0;
  float am2315Temperature =0;
  float batteryThermistor = 0;
  time_t lastBoot;
  
  float PiBatteryVoltage = 0.0;
  
  float UnregulatedWindVoltage = 0.0;
  float RegulatedWindVoltage = 0.0;
  float PiSolarVoltage = 0.0;
  
  float readArduinoSL = false;
  
  const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

#define CNT 10
float runningPiVoltage[CNT];


int RESET_WATCHDOG = 26;
int WATCHDOG_OUTPUT = 32;




void ResetWatchdog()
{
    pinMode(RESET_WATCHDOG, OUTPUT);
    delay(200);
    pinMode(RESET_WATCHDOG, INPUT);
    Serial.println("Watchdog Reset");
    
}
  
/*  
AlarmID_t PiOnAlarmID;
AlarmID_t PiOffAlarmID;
AlarmID_t PiOnMidnightAlarmID;
AlarmID_t PiOffMidnightAlarmID;
*/




// file includes
#include "EnvironConstants.h"
#include "utils.h"

#include "logging.h"

#include "readthermistor.h"
#include "actions.h"

#include "interrupthandlers.h"

#include "picontrol.h"

#include "LogFram.h"
#include "statemachines.h"




Sleep sleep;

void Repeats(){
  Serial.println("----------------------------40 second timer");         
}


void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


float windSpeedMin;
float windSpeedMax;
float windGustMin;
float windGustMax;
float windDirectionMin;
float windDirectionMax;


float currentWindSpeed;
float currentWindGust;

float rainTotal;

// weather
#include "SDL_Weather_80422.h"

#define pinLED     13   // LED connected to digital pin 13
#define pinAnem    18  // Anenometer connected to pin 18 - Int 5
#define pinRain    2   // Rain Bucket connecto to pin 19 - Int 4
#define intAnem    5
#define intRain    0

// for mega, have to use Port B - only Port B works.
/*
 Arduino Pins         PORT
 ------------         ----
 Digital 0-7          D
 Digital 8-13         B
 Analog  0-5          C
*/


// initialize SDL_Weather_80422 library
SDL_Weather_80422 weatherStation(pinAnem, pinRain, intAnem, intRain, A0, SDL_MODE_INTERNAL_AD);

#include "elapsedMillis.h"

elapsedMillis timeElapsed; //declare global if you don't want it reset every time loop runs

void setup()
{
  
  Serial.begin(9600);           // set up Serial library at 9600 bps
  
  Serial.print(F("ArduinoBatteryWatchdog "));
  Serial.print(VERSIONNUMBER);
  Serial.println(F(" 10/27/13")); 
  Serial.print(F("Compiled at:"));
  Serial.print (F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println();
  
  
    if (fram.begin()) {  // you can stick the new i2c addr in here, e.g. begin(0x51);
    Serial.println("Found I2C FRAM");
    
    initializeFRAMTable(104); // initialize with 104 size
    

    
    
  } else {
    Serial.println("No I2C FRAM found ... check your connections\r\n");
    //while (1);
  }

    tmElements_t tm;
    
  if (! RTC.read(tm)) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    
 //   if (getDate(__DATE__, tm) && getTime(__TIME__, tm))
    {
      char Month[12];
      int Day, Year;
      uint8_t monthIndex;

      sscanf(__DATE__, "%s %d %d", Month, &Day, &Year);
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

      sscanf(__TIME__, "%d:%d:%d", &Hour, &Min, &Sec);
 
      tm.Hour = Hour;
      tm.Minute = Min;
      tm.Second = Sec; 
       char timeNow[20];
       timeNow[0] = '\0';
       buildTimeString(timeNow, timeNow, tm);
       
       Serial.print("tm=");
       Serial.println(timeNow);
       
       Serial.print("Ok, Time = ");
       Serial.write(tm.Hour);
       Serial.write(':');
       Serial.write(tm.Minute);
       Serial.write(':');
       Serial.write(tm.Second);
       Serial.print(", Date (D/M/Y) = ");
       Serial.print(tm.Day);
       Serial.write('/');
       Serial.print(tm.Month);
       Serial.write('/');
       Serial.print(tmYearToCalendar(tm.Year));
       Serial.println();
       // and configure the RTC with this info
       RTC.write(tm);
       
       Serial.println("getDate true");
                   
    }

    

  }
  
 /*     if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
*/
  Serial.print("timeStatus =");
  Serial.println(timeStatus());
  lastBoot = readRTCreliably();
  
  setTime(readRTCreliably());
  setSyncProvider(RTC.get);
  Serial.print("timeStatus =");
  Serial.println(timeStatus());
  Serial.println("----internal arduino clock");
  digitalClockDisplay();
   Serial.println("----internal arduino clock");
   
  RTC.read(tm);
  
 /*   Serial.write(tm.Hour);
    Serial.write(':');
    Serial.write(tm.Minute);
    Serial.write(':');
    Serial.write(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
    */
  
  WAState = NOINTERRUPT;
  sleepTime=5000;
  currentState = STATE0;
  nextState = STATE0;
  watchDogState = true;
  watchDogTimeIncrement = 16L * 60L; // 16 minutes
  //watchDogTimeIncrement = 2 * 60; // 2 minutes
  watchDogTime = readRTCreliably()+watchDogTimeIncrement;
  enableWatchDog = true;
  
 Serial.print("Time to WD:");
 if (enableWatchDog == false)
   Serial.println("WD Disabled");
 else
 {
   Serial.println(watchDogTime - readRTCreliably());  
 }
  requestFromPi = false;
  isPiSignaledWD = false;
  lastInterruptToPi = NOINTERRUPT;
  enableShutdowns = true;
  piVoltageStartupThresholdOK = true;
  piVoltageShutdownThresholdOK = true;
  setAlarmTimes();  // set up my alarms
  piVoltageShutdownTime = readRTCreliably();

  readArduinoSL = false;
 

  
  // load average array for PiVoltage for smoothing
  int i;
  for (i=0; i < CNT; i++)
  {
    
    runningPiVoltage[i] = getAnalogVoltage(PiBatteryVoltageChannel);
    delay(500);
    Serial.print("volt=");
    Serial.println(runningPiVoltage[i]);
    
  }

  PiBatteryVoltage = rollingAverage(runningPiVoltage, CNT, getAnalogVoltage(PiBatteryVoltageChannel));

  setpiVoltageStartupThresholdsOK(PiBatteryVoltage);
  
  UnregulatedWindVoltage = getAnalogVoltage(UnregulatedWindVoltageChannel)/UnregulatedWindVoltageMultiplier;
  RegulatedWindVoltage = getAnalogVoltage(RegulatedWindVoltageChannel)/RegulatedWindVoltageMultiplier;
  PiSolarVoltage = getAnalogVoltage(PiSolarVoltageChannel)/PiSolarVoltageMultiplier;

Serial.print("UnregulatedWindVoltage=");
Serial.println(UnregulatedWindVoltage);
Serial.print("RegulatedWindVoltage=");
Serial.println(RegulatedWindVoltage);
Serial.print("PiSolarVoltage=");
Serial.println(PiSolarVoltage);



  // do initialization and then start the state machine
 
  pinMode(indicatorLED, OUTPUT);
  pinMode(PiRelayReset, OUTPUT);  // turn enegizing off for current to flow - pulse to pi off
  pinMode(PiRelaySet, OUTPUT);  // turn enegizing off for current to flow - pulse to pi on
  digitalWrite(PiRelayReset, false);
  digitalWrite(PiRelaySet, false);
  
  pinMode(selectSolarReset, OUTPUT);  // turn enegizing off for current to flow - pulse to pi off
  pinMode(selectWindSet, OUTPUT);  // turn enegizing off for current to flow - pulse to pi on
  digitalWrite(selectSolarReset, false);
  digitalWrite(selectWindSet, false);
  
  // set up interrupt to pi
  pinMode(interruptPi, OUTPUT);
  digitalWrite(interruptPi, false);
  digitalWrite(indicatorLED, false);

  LogEntryNextItem = 0; // clear table
  initializeLogTable();
 
 
  writeLogEntry( LOGINFO, LOGArduinoReboot, -1); 

  // turnPiOn();
  justTurnPiOn();
  
  solarWind = 0;
  selectSolar();
 
  attachInterrupt(4, piRequestInterrupt, RISING);
  
  timeElapsed = 0;
  

  
}



void loop() {
 
 
  
  // set up state machine
  
  printStateVariables();
  
  switch (currentState)
  {
     case  STATE0:
     
       nextState = state0(currentState);
     break;
     
     case STATE1:
     
        nextState = state1(currentState);   
     break;
     
     case STATE3:
     
        nextState = state3(currentState);   
     break;
     
     default: 
     
     break;
    
    
    
    
    
    
  }

  // check for alarms
  
  checkForAlarms();
  
  
  Serial.print("timeElapsed=");
  Serial.println(timeElapsed);
  
  // Weather sampling
  
   if (timeElapsed > 300000) // every 5 minutes record it
  {
    
			
    
    Serial.print("time=");
    Serial.println(millis());
    Serial.print("micro time=");
    Serial.println(micros());
    currentWindSpeed = weatherStation.current_wind_speed()/1.6;
    currentWindGust = weatherStation.get_wind_gust()/1.6;
    
    float oldRain = rainTotal;
    rainTotal = rainTotal + weatherStation.get_current_rain_total()*0.03937;
    if (oldRain < rainTotal)
    {
     Serial.println("It is Raining\0");
    }
      
      
    timeElapsed = 0;
    Serial.println();
    Serial.println("Weather-------------------------");
    Serial.print(" currentWindSpeed=");
    Serial.print(currentWindSpeed);
  
    Serial.print(" \tcurrentWindGust=");
    Serial.print (currentWindGust);
  
    Serial.print(" \tWind Direction=");
    Serial.print(weatherStation.current_wind_direction());
  
   
    Serial.print(" \t\tCumulative Rain = ");
    Serial.println(rainTotal);
    
    Serial.println("Weather-------------------------");
    
    // now write the record to FRAM
    
    
    char returnString[100];
    returnString[0] = '\0';
           
    tmElements_t tm;
    RTC.read(tm);

    char floatString[15];

    char timeNow[20];
    timeNow[0] = '\0';
    buildTimeString(timeNow, timeNow, tm);
          
    sprintf(returnString, "%s,", timeNow);
           
    floatString[0] = '\0';
    dtostrf(currentWindSpeed,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
 
    floatString[0] = '\0';
    dtostrf(currentWindGust,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
 
    floatString[0] = '\0';
    dtostrf(weatherStation.current_wind_direction(),6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
 
    floatString[0] = '\0';
    dtostrf(weatherStation.current_wind_direction_voltage(),6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
 
    floatString[0] = '\0';
    dtostrf(rainTotal,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
 
    floatString[0] = '\0';
    dtostrf(UnregulatedWindVoltage,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
 
    floatString[0] = '\0';
    dtostrf(RegulatedWindVoltage,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
 
    floatString[0] = '\0';
    dtostrf(PiSolarVoltage,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
    

    floatString[0] = '\0';
    dtostrf(SolarCellcurrent_mAx41,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
    
   
    floatString[0] = '\0';
    sprintf(floatString, "%i", solarWind);
    strcat(returnString, floatString);
    
    
    writeFramEntry(0,  returnString);
    

    displayFram();

    
    
  }
  
  if (nextState == currentState)
  {
    // sleep
    delay(1000);
 //   sleep.pwrDownMode();
 //   sleep.sleepDelay(sleepTime); 
    delay(4000);   // since we aren't sleeping anymore
    setTime(readRTCreliably());
    
    Serial.print("MLtimeStatus =");
    Serial.println(timeStatus());
    Serial.println("ML----internal arduino clock");
    digitalClockDisplay();
    Serial.println("ML----internal arduino clock");
   
   
    // update weather
   currentWindSpeed = weatherStation.current_wind_speed()/1.6;

 
 
  
  }
  currentState = nextState;
   
   
   // dead man switch  
   
   // pat the watchdog
    
     
    ResetWatchdog();	
    
    tmElements_t tm;
    char myBootDate[25];
    myBootDate[0] = '\0';
    breakTime(lastBoot, tm);
    buildTimeString(myBootDate, myBootDate, tm);
   
    Serial.print("LB:");
    Serial.print(myBootDate);
    Serial.println();
  
   
   
}

