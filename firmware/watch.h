#ifndef WATCH_H
#define WATCH_H

#include <RtcDS1302.h>
#include "func.h"

ThreeWire myWire(13,5,4); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

String todayDate = "";
String toDay_str_t = "";
String start_t_str;
String end_t_str;
boolean start_clock = false;

RtcDateTime start_t;
RtcDateTime end_t;

unsigned long previousMillis = 0;   
const unsigned long interval = 10000;  //10 sec
const int relay_pin          = 14;

void resetWatchSetup(){
  todayDate = "";
}

void startClock(){
  if( start_clock == false ){
    Rtc.Begin();
    start_clock = true;
  }
}

void setupWatch(char* current_date, char* current_time){

    startClock();

    RtcDateTime currentRtcDateTime = RtcDateTime(current_date, current_time);
   
    Rtc.SetDateTime(currentRtcDateTime);

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        //Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(currentRtcDateTime);
    }

    if (Rtc.GetIsWriteProtected())
    {
        //Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        //Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }
 
}


/**
 * WatchLoop
 */
void watchLoop(){
    unsigned long currentMillis = millis();

    if( previousMillis == 0 ){
       previousMillis = millis();
    }


    if (currentMillis - previousMillis >= interval) {  
        previousMillis = currentMillis;

        RtcDateTime now = Rtc.GetDateTime();
        char* toDay_str = today(now);
        toDay_str_t = String( toDay_str );


        if(  toDay_str_t !=  todayDate  ){ // once per day
           todayDate   = String( toDay_str );
           start_t_str = readStringFromEEPROM(0, 8);
           end_t_str   = readStringFromEEPROM(8, 16);
        }

        start_t = RtcDateTime(toDay_str, start_t_str.c_str());
        end_t   = RtcDateTime(toDay_str, end_t_str.c_str());
        
        if( ( start_t.TotalSeconds() < now.TotalSeconds() ) &&  
            ( end_t.TotalSeconds() > now.TotalSeconds() ) ){
              digitalWrite(relay_pin, HIGH);
        }else{
              digitalWrite(relay_pin, LOW);
        }

        free(toDay_str);

        
        
    }
}

#endif
