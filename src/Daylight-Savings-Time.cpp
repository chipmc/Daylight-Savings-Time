/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/chipmc/Documents/Maker/Particle/Projects/Daylight-Savings-Time/src/Daylight-Savings-Time.ino"
/*
 * Project Daylight-Savings-Time
 * Description: Code to Test Daylight Savings Time Conversions
 * Author: Chip McClelland
 * Date: 10-6-19
 */


#include "Particle.h"                               // Particle's libraries - new for product 

// In this example, I am going to hard code these values. 
// For deployments, use Particle vairables for these and set them with a functions

void setup();
void loop();
bool isDSTusa();
#line 14 "/Users/chipmc/Documents/Maker/Particle/Projects/Daylight-Savings-Time/src/Daylight-Savings-Time.ino"
float Timezone = -5.0;                              // Example Eastern Standard Time
float DSToffsetValue = 1.0;                          // Example EST - can set to zero for states that do not observice DST

// These are test values that are based on the example EST timezone
const int numberOfTestPoints = 10;
const unsigned long testTimesAndDates[numberOfTestPoints] = {1547532000,1551592800,1552629600,1560578400,1571119200,1572588000,1572667200,1572674400,1572696000,1576389600 };
char data[128];     // Where we will store the output to Publish


void setup() {
  // Put initialization like pinMode and begin functions here.

  Time.zone(Timezone);
  Time.setDSTOffset(DSToffsetValue);


  Particle.publish("Startup","Finished",PRIVATE);

}


void loop() {
  static unsigned int i=0;
  if (i >= numberOfTestPoints) i=0; 
  Time.setTime(testTimesAndDates[i]);                 // Note this is UTC time not local.  
  // Now, we will see if the Timezone calculations are correct
  isDSTusa() ? Time.beginDST() : Time.endDST();
  delay(1000);
  snprintf(data, sizeof(data),"The time is %s and the TimeZone is %s", (const char*)Time.timeStr(), Time.isDST() ? "True" : "False");
  Particle.publish("Test",data,PRIVATE);
  i++;
  delay(1000);

}


bool isDSTusa() { 
  // United States of America Summer Timer calculation (2am Local Time - 2nd Sunday in March/ 1st Sunday in November)
  // Adapted from @ScruffR's code posted here https://community.particle.io/t/daylight-savings-problem/38424/4
  // The code works in from months, days and hours in succession toward the two transitions
  int dayOfMonth = Time.day();
  int month = Time.month();
  int dayOfWeek = Time.weekday() - 1; // make Sunday 0 .. Saturday 6

  // By Month - inside or outside the DST window
  if (month >= 4 && month <= 10)  
  { // April to October definetly DST
    Particle.publish("isDSTusa","True - by month",PRIVATE);
    return true;
  }
  else if (month < 3 || month > 11)
  { // before March or after October is definetly standard time
    Particle.publish("isDSTusa","False - by month",PRIVATE);
    return false;
  }

  // So, only March and November are left - time to look at days
  // November first
  boolean firstSundayOrAfter = (dayOfMonth - dayOfWeek > 0);
  if (!firstSundayOrAfter)
  { // before switching Sunday
    Particle.publish("isDSTusa","November - by day",PRIVATE);
    return (month == 11); // November DST will be true, March not
  }

  // Now March
  boolean secondSundayOrAfter = (dayOfMonth - dayOfWeek > 8);
  if (secondSundayOrAfter)
  { // AFTER the switching Sunday
    Particle.publish("isDSTusa","March - by day",PRIVATE);
    return (month == 3); // for March DST is true, for October not
  }

  int secSinceMidnightLocal = Time.now() % 86400;
  boolean dayStartedAs = (month == 10); // DST in October, in March not
  // on switching Sunday we need to consider the time
  if (secSinceMidnightLocal >= 2*3600)
  { //  In the US, Daylight Time is based on local time 
    Particle.publish("isDSTusa","After 2am - by hour",PRIVATE);
    return !dayStartedAs;
  }
  Particle.publish("isDSTusa","Before 2am - by hour",PRIVATE);
  return dayStartedAs;
}
