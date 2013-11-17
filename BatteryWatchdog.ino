// Filename WatchdogBattery.ino
// Version 1.0 09/17/13 JS MiloCreek
//
#define VERSIONNUMBER 1.0
// Normal run state
#define STATE0   0
//  Communicate with Pi and WD Timer
#define STATE1   1

#define STATE2   2
// Read i2c
#define STATE3   3

// Interrupt handlers




#include "Sleep_n0m1.h"
#include <Wire.h>

#include "Adafruit_INA219.h"
#include <Adafruit_AM2315.h>
#include "RTClib.h"
#include "MemoryFree.h"


int count = 0;

unsigned long sleepTime; //how long you want the arduino to sleep

// Pin Values
//#define relay 4
#define indicatorLED 35
#define PiRelay 29
#define PiBatteryVoltageChannel A3

RTC_DS1307 RTC;

int currentState;
int nextState;

volatile int requestFromPi;
volatile int watchDogState; // true - ok, false reboot
volatile int isPiOn;
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
  DateTime lastBoot;
  
// file includes
#include "utils.h"
#include "readthermistor.h"
#include "actions.h"
#include "statemachines.h"
#include "interrupthandlers.h"




Sleep sleep;

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
  
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  // RTC.adjust(DateTime(__DATE__, __TIME__));
  
  lastBoot = RTC.now();
  
  
  sleepTime=5000;
  currentState = STATE0;
  nextState = STATE0;
  watchDogState = true;
  requestFromPi = false;

  // do initialization and then start the state machine
 
  pinMode(indicatorLED, OUTPUT);
  pinMode(PiRelay, OUTPUT);  // turn enegizing off for current to flow - on to pi on


  digitalWrite(indicatorLED, false);  
  
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

  if (nextState == currentState)
  {
    // sleep
    delay(100);
    sleep.pwrDownMode();
    sleep.sleepDelay(sleepTime); 
  }
  currentState = nextState;
   
}
