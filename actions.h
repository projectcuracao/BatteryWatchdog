//
//
// actions file for hardware
//
//


void turnPiOff()
{
  digitalWrite(PiRelay, true);
  Serial.println("Pi Turned OFF");
  isPiOn = false;
}

void turnPiOn()
{
  
    digitalWrite(PiRelay, false);
    Serial.println("Pi Turned ON"); 
   isPiOn = true; 
}


//
// support files for Pi Battery Monitor
//

float getAnalogVoltage(int pinNumber)
{
 
     return 5.0*(analogRead(pinNumber) / 1024.0);
}
