// Filename WatchdogBattery.ino
// Version 1.0 09/17/13 JS MiloCreek
//
#define VERSIONNUMBER 1.1
// Normal run state
#define STATE0   0
//  Communicate with Pi and WD Timer
#define STATE1   1
// if PiOff, check for restart conditions
// if PiOn, check for shutdown conditions

#define STATE2   2

#define STATE3   3

// Interrupt handlers

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


int count = 0;

unsigned long sleepTime; //how long you want the arduino to sleep

// Pin Values
//#define relay 4
#define indicatorLED 35
#define PiRelaySet 29
#define PiRelayReset 27
#define PiBatteryVoltageChannel A3
#define interruptPi 22

DS1307RTC MYRTC;

int currentState;
int nextState;

volatile int requestFromPi;
volatile int watchDogState; // true - ok, false reboot
time_t  watchDogTime;
long watchDogTimeIncrement;
volatile int isPiOn;
int lastInterruptToPi;
int enableWatchDog;   // disable watchdog during pi off times
// log stuff

struct LogEntry {
  time_t entryTime;
  int entryType;
  int entryData0;
  int entryData1;
  bool entryRead;
};

int LogEntryNextItem;

#define LOGENTRYTABLESIZE 10

LogEntry LogEntryArray[LOGENTRYTABLESIZE];

long piOnTime;
long piOffTime;
long piMidnightOnTime;
long piMidnightOffTime;

bool piOnTimeState;
bool piOffTimeState;
bool piMidnightOnTimeState;
bool piMidnightOffTimeState;

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
  
  
  const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
  
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
#include "statemachines.h"


Sleep sleep;

void Repeats(){
  Serial.println("----------------------------40 second timer");         
}


void setup()
{
  
  Serial.begin(57600);           // set up Serial library at 9600 bps
  
  Serial.print(F("ArduinoBatteryWatchdog "));
  Serial.print(VERSIONNUMBER);
  Serial.println(F(" 10/27/13")); 
  Serial.print(F("Compiled at:"));
  Serial.print (F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println();
  

    tmElements_t tm;
    
  if (! RTC.read(tm)) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    
    if (getDate(__DATE__, tm) && getTime(__TIME__, tm)) {

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
    else
    {
      Serial.println("getDate false");
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
  lastBoot = RTC.get();
  setTime(RTC.get());
  setSyncProvider(RTC.get);
  Serial.print("timeStatus =");
  Serial.println(timeStatus());
  

  
  RTC.read(tm);
  
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
  
  WAState = NOINTERRUPT;
  sleepTime=5000;
  currentState = STATE0;
  nextState = STATE0;
  watchDogState = true;
  watchDogTimeIncrement = 16L * 60L; // 16 minutes
  //watchDogTimeIncrement = 2 * 60; // 2 minutes
  watchDogTime = RTC.get()+watchDogTimeIncrement;
  requestFromPi = false;
  isPiSignaledWD = false;
  lastInterruptToPi = NOINTERRUPT;
  
 
  setAlarmTimes();  // set up my alarms
  




  // do initialization and then start the state machine
 
  pinMode(indicatorLED, OUTPUT);
  pinMode(PiRelayReset, OUTPUT);  // turn enegizing off for current to flow - pulse to pi off
  pinMode(PiRelaySet, OUTPUT);  // turn enegizing off for current to flow - pulse to pi on
  digitalWrite(PiRelayReset, false);
  digitalWrite(PiRelaySet, false);
  
  // set up interrupt to pi
  pinMode(interruptPi, OUTPUT);
  digitalWrite(interruptPi, false);
  digitalWrite(indicatorLED, false);

  LogEntryNextItem = 0; // clear table
  initializeLogTable(); 
  
  turnPiOn();
  attachInterrupt(4, piRequestInterrupt, RISING);
  
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
  
  if (nextState == currentState)
  {
    // sleep
    delay(1000);
    sleep.pwrDownMode();
    sleep.sleepDelay(sleepTime); 
  }
  currentState = nextState;
   
}

