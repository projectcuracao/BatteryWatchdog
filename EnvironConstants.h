//
//
// contains constants to run the battery watchdog
//
//
// defaults

//float PI_BATTERY_SHUTDOWN_THRESHOLD = 3.66; // 20%
float PI_BATTERY_SHUTDOWN_THRESHOLD = 3.70; // 20%
float PI_BATTERY_STARTUP_THRESHOLD = 3.95; // 30%

float INSIDE_TEMPERATURE_PI_SHUTDOWN_THRESHOLD = 49.33; // 110
float INSIDE_TEMPERATURE_PI_STARTUP_THRESHOLD = 37.78; // 1000


float INSIDE_HUMIDITY_PI_SHUTDOWN_THRESHOLD = 98.0;
float INSIDE_HUMIDITY_PI_STARTUP_THRESHOLD = 93.0;

char PI_SHUTDOWN_TIME[] = "06:10:00";
char PI_START_TIME[] = "10:10:00";

//char PI_SUNRISE_TIME[] = "10:00:00"; //  06:00 curacao time
//char PI_SUNSET_TIME[] =  "02:00:00"; //  18:00 curacao time

char PI_SUNRISE_TIME[] = "15:00:00"; //  07:00 Coeur d'Alene time
char PI_SUNSET_TIME[] =  "03:00:00"; //  19:00  Coeur d'Alene time
//char PI_SUNSET_TIME[] =  "22:59:00"; //  19:00  Coeur d'Alene time


//char PI_START_TIME[] = "04:45:00";
//char PI_SHUTDOWN_TIME[] = "04:00:00";


//char PI_START_TIME[] = "14:00:00";
//char PI_SHUTDOWN_TIME[] = "05:00:00";
//float PI_MIDNIGHT_WAKEUP_SECONDS_LENGTH = 1200.0;   // one hour
float PI_MIDNIGHT_WAKEUP_SECONDS_LENGTH = 3600.0;   // one hour

float PI_MIDNIGHT_WAKEUP_THRESHOLD = 3.971;   // 60%

float WIND_TURBINE_MINIMUM_UNREGULATED_VOLTAGE = 9.00;  // 9V to turn regulator

