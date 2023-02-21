#ifndef MYCLOCK_H //include guard
#define MYCLOCK_H
#include<FastLED.h>           //Controlling the LED strip (clock hands)
#include<TimeLib.h>           //Setting/getting system time
#include<SparkFunDS3234RTC.h> //RTC time setting/getting
#include<Timezone.h>          //unix time (utc) to local timezone conversion
#include<WiFi.h>              //for NTPClient.h
#include<NTPClient.h>         //NTP Client
#include"globals.h"
#include"structs.h"
#include"myConfig.h"

class myClock /*clock was already taken by a function in ctime.h*/{
  private:
    CRGB leds[NUM_LEDS];
    Timezone* TZ = nullptr;

    uint8_t scaleBrightness(const int light);

  public:
    myClock();

    void begin();

    void updateTimezone();

    Timezone* getTimezone();

    static time_t timeFromRTC();
    static time_t timeFromNTP();
    time_t getLocalTZTime();

    void Blank();
    void DrawSec();
    void DrawMin();
    void DrawHour();

    void tick();

    ~myClock();
};

extern myClock Clock;
#endif
