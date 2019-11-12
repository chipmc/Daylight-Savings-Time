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


#include "Particle.h"                                 // Particle's libraries - new for product 

// In this example, I am going to hard code these values. 
// For deployments, use Particle vairables for these and set them with Particle functions
void setup();
void loop();
bool isDSTusa();
#line 13 "/Users/chipmc/Documents/Maker/Particle/Projects/Daylight-Savings-Time/src/Daylight-Savings-Time.ino"
float Timezone = -5.0;                                // Example Eastern Standard Time
float DSToffsetValue = 1.0;                           // Example EST - can set to zero for states that do not observice DST

unsigned long januaryMidnight = 1546318800;
unsigned long februaryFifteenthMidnight = 1550188800;
unsigned long halloweenMidnight = 1572494400;
unsigned long hourSeconds = 60*60;
unsigned long daySeconds = 24 * 60 * 60;
time_t testTime;


// These are UTC time test values that are based on the example EST timezone
char data[128];     // Where we will store the output to Publish


void setup() {
  Serial.begin();
  Time.zone(Timezone);
  Time.setDSTOffset(DSToffsetValue);
}


void loop() {
  for (int i=0; i<=30; i++) {
    testTime = februaryFifteenthMidnight + i * daySeconds;
    Time.setTime(testTime);                                           // Note this is UTC time not local.  

    // Now, we will see if the Timezone calculations are correct
    if (Time.isValid()) 
    { // We only want to run this if time is valid - otherwise random result
      isDSTusa() ? Time.beginDST() : Time.endDST();       
    }
    snprintf(data, sizeof(data),"%s - DST: %s", (const char*)Time.timeStr(), Time.isDST() ? "True" : "False");
    Serial.println(data);
    //Particle.publish("Test",data,PRIVATE);
    //delay(1000);
  }

  for (int i=0; i<=30; i++) {
    testTime = halloweenMidnight + i * daySeconds;
    Time.setTime(testTime);                                           // Note this is UTC time not local.  
    // Now, we will see if the Timezone calculations are correct
    if (Time.isValid()) 
    { // We only want to run this if time is valid - otherwise random result
      isDSTusa() ? Time.beginDST() : Time.endDST();       
    }
    snprintf(data, sizeof(data),"%s - DST: %s", (const char*)Time.timeStr(), Time.isDST() ? "True" : "False");
    Serial.println(data);
  }
  while (1) Particle.process();                                              // Keep from going unresponsive as no more main loop transits
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
    return true;
  }
  else if (month < 3 || month > 11)
  { // before March or after October is definetly standard time
    return false;
  }

  boolean beforeFirstSunday = (dayOfMonth - dayOfWeek < 0);
  boolean secondSundayOrAfter = (dayOfMonth - dayOfWeek > 7);

  if (beforeFirstSunday && !secondSundayOrAfter) return (month == 11);
  else if (!beforeFirstSunday && !secondSundayOrAfter) return false;
  else if (!beforeFirstSunday && secondSundayOrAfter) return (month == 3);

  int secSinceMidnightLocal = Time.now() % 86400;
  boolean dayStartedAs = (month == 10); // DST in October, in March not
  // on switching Sunday we need to consider the time
  if (secSinceMidnightLocal >= 2*3600)
  { //  In the US, Daylight Time is based on local time 
    return !dayStartedAs;
  }
  return dayStartedAs;
}
