#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "FS.h"
#include "watch.h"


const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String message = ""; 
String current_date_time;
String start_time;
String stop_time;
boolean server_on = false;

void handle_home(){
  
  File file = SPIFFS.open("/home.html", "r");
  message = file.readString();
  file.close();
  
  if (webServer.method() == HTTP_POST) {
      current_date_time    = webServer.arg("current_date_time");
      start_time           = webServer.arg("start_time");
      stop_time            = webServer.arg("stop_time");
      char* current_date   = formatDate( current_date_time );
      char* current_time   = formatTime( current_date_time );

      resetWatchSetup();
      
      if( current_date_time.length() > 0 ){
          setupWatch(current_date, current_time);
           free(current_date);
           free(current_time);
      }

      start_time = start_time.length() < 8 ?  start_time + ":00" : start_time;
      stop_time  = stop_time.length() < 8 ?  stop_time + ":00" : stop_time;
      
      saveStringToEEPROM(0, start_time);
      saveStringToEEPROM(8, stop_time);
      EEPROM.commit();
  }

  String start_t_str = readStringFromEEPROM(0, 8);
  String end_t_str   = readStringFromEEPROM(8, 16);

  RtcDateTime now = Rtc.GetDateTime();

  message.replace("{{start_time}}", start_t_str);
  message.replace("{{stop_time}}", end_t_str);
  
  char datestring[26];

  snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            now.Day(),
            now.Month(),
            now.Year(),
            now.Hour(),
            now.Minute(),
            now.Second() );
      
  message.replace("{{time_now}}", String(datestring));


  webServer.send(200, "text/html", message);
}

void SetupWebServer(){
    //Serial.println( "+---+" );
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP("TimerSwitch");

    if(!SPIFFS.begin()){
      //Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  
    // modify TTL associated  with the domain name (in seconds)
    // default is 60 seconds
    dnsServer.setTTL(300);
    // set which return code will be used for all other domains (e.g. sending
    // ServerFailure instead of NonExistentDomain will reduce number of queries
    // sent by clients)
    // default is DNSReplyCode::NonExistentDomain
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  
    // start DNS server for a specific domain name
    dnsServer.start(DNS_PORT, "TimerSwitch.co", apIP);
  
    // simple HTTP server to see that DNS server is working
    webServer.onNotFound([]() {
      String message = "Not Found!\n\n";
      message += "URI: ";
      message += webServer.uri();
  
      webServer.send(200, "text/plain", message);
    });
    webServer.on("/", handle_home);
  
    webServer.begin();

    server_on = true;
}

void stopWebServer(){
  if( server_on ){
    dnsServer.stop();
    webServer.stop();
    WiFi.mode(WIFI_OFF);
    server_on = false;
  }
}
