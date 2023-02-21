# Initialising flash storage and setting custom settings
You will have to create a new sketch with the Arduino IDE, since one directory cannot contain two sketches.  
After that you're going to have to copy/symlink the following files from the main sketch to it:
- globals.h
- structs.h
- structs.cpp  

You can write something like this, or copy the code below to your new sketch:
```
#include<Preferences.h>
#include<SparkFunDS3234RTC.h>
#include<nvs_flash.h>
#include"structs.h"

/*Preferences*/
Preferences preferences;

void setup() {
  Serial.begin(115200);

  /*setting custom values:*/
  WifiCredentials.ssid = "your SSID";
  WifiCredentials.pass = "your password";
  //you can change other preferences too, but I recommend using the webpage for that.
  
  nvs_flash_erase();  //erase the NVS partition
  nvs_flash_init();   //initialise the NVS partition

  Serial.println("Saving WiFi pref...");
  preferences.begin(PREF_WIFI, false);
  preferences.putString("ssid", WifiCredentials.ssid);
  preferences.putString("pass", WifiCredentials.pass);
  preferences.end();
  Serial.println("Saved.");
  Serial.println();

  Serial.println("Saving colour pref...");
  preferences.begin(PREF_COLOR, false);
  //Second R,G,B:
  preferences.putUChar("secR", Colors.sec.red);
  preferences.putUChar("secG", Colors.sec.green);
  preferences.putUChar("secB", Colors.sec.blue);
  //Minute R,G,B:
  preferences.putUChar("minR", Colors.min.red);
  preferences.putUChar("minG", Colors.min.green);
  preferences.putUChar("minB", Colors.min.blue);
  //Hour R,G,B:
  preferences.putUChar("hR", Colors.h.red);
  preferences.putUChar("hG", Colors.h.green);
  preferences.putUChar("hB", Colors.h.blue);
  //Background R,G,B:
  preferences.putUChar("bgR", Colors.bg.red);
  preferences.putUChar("bgG", Colors.bg.green);
  preferences.putUChar("bgB", Colors.bg.blue);
  preferences.end();
  Serial.println("Saved.");
  Serial.println();
  
  //Brightness min, max:
  preferences.begin(PREF_BRIGHT, false);
  preferences.putUShort("minB", Brightness.min);
  preferences.putUShort("maxB", Brightness.max);
  preferences.putUChar("manu", Brightness.manual);
  preferences.end();
  Serial.println("Saved.");
  Serial.println();

  Serial.println("Saving NTP pref...");
  preferences.begin(PREF_NTP, false);
  preferences.putString("serv", NTP.server);
  preferences.putBool("manu", NTP.manual);
  preferences.end();
  Serial.println("Saved.");
  Serial.println();

  //Timezone info here
  Serial.println("Saving timezone pref...");
  preferences.begin(PREF_RULE1, false);
  //preferences.putString("abbr", abbr);
  preferences.putString("mon",  Rule1.mon);
  preferences.putString("week", Rule1.week);
  preferences.putString("dow",  Rule1.dow);
  preferences.putUChar("hour",  Rule1.hour);
  preferences.putUInt("offs",   Rule1.offs);
  preferences.end();
  Serial.println("Saved 1/2.");
  preferences.begin("TZR2", false);
  //preferences.putString("abbr", Rule2.abbr);
  preferences.putString("mon",  Rule2.mon);
  preferences.putString("week", Rule2.week);
  preferences.putString("dow",  Rule2.dow);
  preferences.putUChar("hour",  Rule2.hour);
  preferences.putUInt("offs",   Rule2.offs);
  preferences.end();
  Serial.println("Saved 2/2.");
  Serial.println();

  Serial.println("Done saving preferences to flash memory.");

  rtc.begin(RTC_PIN);
  rtc.autoTime();
  //or:
  //rtc.setTime(second, minute, hour, day_of_week, day, month, year);
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  //It will flash the builtin LED when done writing
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
}
```
