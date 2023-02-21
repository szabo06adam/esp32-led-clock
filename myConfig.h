#ifndef CONFIG_H  //include guard
#define CONFIG_H
#include<Preferences.h>       //Read/write preferences from/to flash
#include<SparkFunDS3234RTC.h> //RTC time setting/getting
#include<TimeLib.h>           //Setting/getting system time
#include"esp_system.h"        //Flash storage
#include"nvs_flash.h"         //Flash storage
#include"nvs.h"               //Flash storage
#include"globals.h"
#include"structs.h"

class myConfig{
  private:
  static bool setTime(time_t t);
  void readRule(Preferences& pref, timezone_rule& rule);
  void writeRule(Preferences& pref, timezone_rule& rule);
  
  public:
  myConfig();
  
  bool readRule1();
  bool readRule2();
  bool readWiFi();
  bool readNTP();
  bool readColors();
  bool readBrightness();
  
  static bool setUTCTimeAndWrite(int Year, int Month, int Day, int Hour, int Minute, int Second = 0);
  static bool setLocTimeAndWrite(int Year, int Month, int Day, int Hour, int Minute, int Second = 0);
  
  bool writeRule1();
  bool writeRule2();
  bool writeWiFi();
  bool writeNTP();
  bool writeColors();
  bool writeBrightness();
};

extern myConfig Config;
#endif
