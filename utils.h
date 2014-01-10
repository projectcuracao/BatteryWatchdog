
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


bool getTime(const char *str, tmElements_t tm)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str, tmElements_t tm)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}


