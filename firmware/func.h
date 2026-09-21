#ifndef FUNC_H
#define FUNC_H

const char* monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

char* formatDate(String &dateTime){

    
    char* datestring = (char*)malloc(26 * sizeof(char));
    int year, month, day, hour, minute;

    if (sscanf(dateTime.c_str(), "%d-%d-%dT%d:%d", &year, &month, &day, &hour, &minute) == 5) {
      
      snprintf_P(datestring, 
              26,
              PSTR("%s %02u %04u"),
                monthNames[ month - 1 ],
                day,
                year
             );
    }

    return datestring;
}

/**
 * Format Time
 */
char* formatTime(String &dateTime){

    char* datestring = (char*)malloc(26 * sizeof(char));
    int year, month, day, hour, minute;

    if (sscanf(dateTime.c_str(), "%d-%d-%dT%d:%d", &year, &month, &day, &hour, &minute) == 5) {
      
      snprintf_P(datestring, 
              26,
              PSTR("%02u:%02u:00"),
                hour,
                minute
             );
    }

    return datestring;
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[26];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    //Serial.print(datestring);
}

char* nextDay(const RtcDateTime& dt)
{

    char* datestring = (char*)malloc(26 * sizeof(char));
    
    snprintf_P(datestring, 
            26,
            PSTR("%s %02u %04u"),
              monthNames[ dt.Month() - 1],
              dt.Day()+1 ,
              dt.Year()
           );


    return datestring;
}


/**
 * Today
 */
char* today(const RtcDateTime& dt){

    char* datestring = (char*)malloc(26 * sizeof(char));

    
    snprintf_P(datestring, 
            26,
            PSTR("%s %02u %04u"),
              monthNames[ dt.Month() - 1 ],
              dt.Day(),
              dt.Year()
           );


    return datestring;
}

/* 
 * Function to save a string to EEPROM
 */
void saveStringToEEPROM(int address, const String& data) {
  int i;
  for (i = 0; i < data.length(); i++) {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.write(address + i, '\0');
}

/*
 * Function to read a string from EEPROM
 */
String readStringFromEEPROM(int address, int length) {
  String data = "";
  for (int i = 0; i < length; i++) {
    char c = EEPROM.read(address + i);
    if (c == '\0') {
      break; // Stop reading at the null terminator
    }
    data += c;
  }
  return data;
}

void free_esp_str(String &str){
   str = "";
}

#endif
