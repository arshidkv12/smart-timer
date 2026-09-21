
#include <EEPROM.h>
#include "WebServer.h"
#include "func.h"
#include "watch.h"


unsigned long buttonLowTime = 0;
const int button            = 12;

void setup() {
    //Serial.begin(9600);
    pinMode(button, INPUT);
    pinMode(relay_pin, OUTPUT);

    EEPROM.begin(512);  
}


void loop() {
  
  unsigned long currentMillis = millis();

  delay(100);
  int input = digitalRead(button);
  if(input == 0 ){
      buttonLowTime = currentMillis;
      SetupWebServer();
      delay(200);
  }


  if( (  currentMillis - buttonLowTime ) < 600000 ){ //10 min
    dnsServer.processNextRequest();
    webServer.handleClient();
  }else{
    stopWebServer();
  }

  watchLoop();

}
