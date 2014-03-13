
//
// Utilities
//



void setpiVoltageStartupThresholdsOK(float voltage)
{
  
  if (voltage > PI_BATTERY_STARTUP_THRESHOLD)
    piVoltageStartupThresholdOK = true;
    else
    piVoltageStartupThresholdOK = false;
    
  if (voltage > PI_BATTERY_SHUTDOWN_THRESHOLD)
    piVoltageShutdownThresholdOK = true;
    else
    piVoltageShutdownThresholdOK = false;
}
/*
Equation: y = (c3 * x^3) + (c2 * x^2) + (c1 * x^1) + b

Where:
y = Wind Turbine Output Voltage
x = Wind Speed in MPH

The coefficients are:

c3	-0.000224491
c2	0.012549184
c1	0.255523699
b	0.247342657
*/
float convertWindTurbineVoltageToMPH(float x)
{
#define c3	-0.000224491
#define c2	0.012549184
#define c1	0.255523699
#define b	0.247342657

  if (x < 0.01f)
    return 0.0;
  
  return (c3 * pow(x,3)) + (c2 * pow(x,2)) + (c1 * x) + b;

}


#define MAX_STRING_LEN 200
// Function to return a substring defined by a delimiter at an index
char* subStr (char* str, char *delim, int index) {
   char *act, *sub, *ptr;
   static char copy[MAX_STRING_LEN];
   int i;

   // Since strtok consumes the first arg, make a copy
   strcpy(copy, str);

   for (i = 1, act = copy; i <= index; i++, act = NULL) {
      //Serial.print(".");
      sub = strtok_r(act, delim, &ptr);
      if (sub == NULL) break;
   }
   return sub;

}

void return2Digits(char returnString[], char *buffer2, int digits)
{
  if(digits < 10)
    sprintf(returnString,"0%i", digits);
  else
    sprintf(returnString,"%i", digits);
    
  strcpy(returnString, buffer2);
    

}



void buildTimeString(char returnString[], char *buffer2, tmElements_t convertTime)
{
  
   
    
    char myBuffer[5];
    sprintf(myBuffer,"%i-", tmYearToCalendar(convertTime.Year));
    strcat(returnString, myBuffer);

    return2Digits(myBuffer, myBuffer, convertTime.Month);
    strcat(returnString, myBuffer);
    strcat(returnString, "-");
 
    return2Digits(myBuffer, myBuffer, convertTime.Day);
    strcat(returnString, myBuffer);
    strcat(returnString, " ");
    
    return2Digits(myBuffer, myBuffer, convertTime.Hour);
    strcat(returnString, myBuffer);
    strcat(returnString, ":");
    
    return2Digits(myBuffer, myBuffer, convertTime.Minute);
    strcat(returnString, myBuffer);
    strcat(returnString, ":");
    
    return2Digits(myBuffer, myBuffer, convertTime.Second);
    strcat(returnString, myBuffer);
  
  
  
}


/* time_t cvt_TIME(char const *time) { 
    char s_month[5];
    int month, day, year;
    struct tm t = {0};
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    sscanf(time, "%s %d %d", s_month, &day, &year);

    month = (strstr(month_names, s_month)-month_names)/3;

    t.tm_mon = month;
    t.tm_mday = day;
    t.tm_year = year - 1900;
    t.tm_isdst = -1;

    return mktime(&t);
}
*/




