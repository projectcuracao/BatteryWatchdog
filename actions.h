//
//
// actions file for hardware
//
//


void turnPiOff()
{

    digitalWrite(PiRelayReset, false);
    digitalWrite(PiRelayReset, true);
    delay(100);
    digitalWrite(PiRelayReset, false);
    Serial.println("Pi Turned OFF");
    digitalWrite(interruptPi, false); // make sure the line is low now
   
    isPiOn = false;
}

void turnPiOn()
{
    digitalWrite(interruptPi, false); // make sure the line is low now
    digitalWrite(PiRelaySet, false);
    digitalWrite(PiRelaySet, true);
    delay(100);
    digitalWrite(PiRelaySet, false);
    Serial.println("Pi Turned ON"); 
    isPiOn = true; 
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
   return true;
}

