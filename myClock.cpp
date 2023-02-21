#include"myClock.h"
#include<cmath>

uint8_t myClock::scaleBrightness(const int light)
{
  if(light < Brightness.min)
    return 0;
  if(light > Brightness.max)
    return 0xff;
  return (uint8_t)(std::round(((float)light / Brightness.max)  * 0xff));
}

myClock::myClock()
{
  
}

void myClock::begin()
{
  pinMode(LIGHT_IN, INPUT);
  pinMode(LED_DATA, OUTPUT);

  updateTimezone();

  rtc.begin(RTC_PIN);

  setSyncProvider(timeFromRTC);
  setSyncInterval(SYNC_FREQUENCY);
  
  FastLED.addLeds<LED_TYPE, LED_DATA, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  if(Brightness.manual)
    FastLED.setBrightness(Brightness.manual);
  else
    FastLED.setBrightness(20);  
    /*20 so it wont be too bright if the photodiode malfunctions in some way. If it works
     *it will override this setting at the first clock tick() anyway.*/
}

void myClock::updateTimezone()
{
  delete TZ;

  std::lock(Rule1.rule_mutex, Rule2.rule_mutex);
  std::lock_guard<std::recursive_mutex> r1lock(Rule1.rule_mutex, std::adopt_lock);
  std::lock_guard<std::recursive_mutex> r2lock(Rule2.rule_mutex, std::adopt_lock);
  
  if(tzrEquals(Rule1, Rule2))
    TZ = new Timezone(tzr_to_TCR(Rule1));
  else
    TZ = new Timezone(tzr_to_TCR(Rule1), tzr_to_TCR(Rule2));
}

Timezone* myClock::getTimezone()
{
  return TZ;
}

time_t myClock::timeFromRTC()
{
  rtc.update();
  TimeElements tm;
  tm.Second = rtc.second();
  tm.Minute = rtc.minute();
  tm.Hour = rtc.hour();
  tm.Day = rtc.date();
  tm.Month = rtc.month();
  tm.Year = 2000 + rtc.year() - 1970;
  return makeTime(tm);
}

time_t myClock::timeFromNTP()
{
  std::lock_guard<std::recursive_mutex> lock(NTP.server_mutex);
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, NTP.server.c_str());
  timeClient.begin();
  timeClient.update();
  time_t current = timeClient.getEpochTime();
  timeClient.end();
  return current;
}

time_t myClock::getLocalTZTime()
{
  return TZ->toLocal(now());
}

void myClock::Blank()
{
  std::lock_guard<std::recursive_mutex> lock(Colors.color_mutex);
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = Colors.bg;
}

void myClock::DrawSec()
{
  int s = second(getLocalTZTime());

  std::lock_guard<std::recursive_mutex> lock(Colors.color_mutex);
  for (int i = 0; i <= s; i++)
    leds[i] = Colors.sec;
}

void myClock::DrawMin()
{
  int m = minute(getLocalTZTime());

  std::lock_guard<std::recursive_mutex> lock(Colors.color_mutex);
  for (int i = 0; i <= m; i++)
    leds[i] = Colors.min;
}

void myClock::DrawHour()
{
  int m = minute(getLocalTZTime());
  int h = hour(getLocalTZTime()) % 12;

  std::lock_guard<std::recursive_mutex> lock(Colors.color_mutex);

  CRGB hourProgress = {floor(Colors.h.red *   CLOCK_HOUR_PROGRESS_DIM),
                       floor(Colors.h.green * CLOCK_HOUR_PROGRESS_DIM),
                       floor(Colors.h.blue *  CLOCK_HOUR_PROGRESS_DIM)};

  float minPercent = (m + 1) / 60.0;

  leds[h * 5] = Colors.h; //The exact hour always lights up
  for (int i = 0; i < ceil(4 * minPercent); i++)
    leds[h * 5 + 1 + i] = hourProgress;
    /*The following four positions light up depending on how much has passed from 00 minutes. (1 position for 15 minutes)
     *this is because I cannot really imitate the continous movement of the hour hand with only 5 discrete positions and
     *because I like it this way :) */
}

void myClock::tick()
{
  if(Brightness.manual) //Every non-zero number is truthy
  {
    FastLED.setBrightness(Brightness.manual);
  }
  else
  {
    #ifdef LIGHT_IN
    int lightV = analogRead(LIGHT_IN);
    uint8_t bright = scaleBrightness(lightV);
  #ifdef DEBUG    
    Serial.println("Incoming light: " + String(lightV) + "\tLED brightness: " + String(bright));
  #endif //DEBUG
    FastLED.setBrightness(bright);
    #endif //LIGHT_IN
  }

  Blank();

  DrawSec();
  DrawMin();
  DrawHour();

  std::lock_guard<std::recursive_mutex> lock(Colors.color_mutex);

  //leds[hour(getLocalTZTime())] = Colors.h;  //No need, because DrawHour() was the last to run anyway
  leds[minute(getLocalTZTime())] = Colors.min;
  leds[second(getLocalTZTime())] = Colors.sec;

  // "send" the leds array out to the LED strip
  FastLED.show();

#ifdef DEBUG
  time_t current_rtc = timeFromRTC();
  time_t current_sys = now();

  time_t local_rtc = TZ->toLocal(current_rtc);
  time_t local_sys = TZ->toLocal(current_sys);

  Serial.println("RTC utc: " + String(year(current_rtc)) + "-" + String(month(current_rtc)) + "-" + String(day(current_rtc)) + "T" + String(hour(current_rtc)) + ":" + String(minute(current_rtc)) + ":" + String(second(current_rtc)) + "\t" +
                 "RTC loc: " + String(year(local_rtc)) + "-" + String(month(local_rtc)) + "-" + String(day(local_rtc)) + "T" + String(hour(local_rtc)) + ":" + String(minute(local_rtc)) + ":" + String(second(local_rtc)));

  Serial.println("SYS utc: " + String(year(current_sys)) + "-" + String(month(current_sys)) + "-" + String(day(current_sys)) + "T" + String(hour(current_sys)) + ":" + String(minute(current_sys)) + ":" + String(second(current_sys)) + "\t" +
                 "SYS loc: " + String(year(local_sys)) + "-" + String(month(local_sys)) + "-" + String(day(local_sys)) + "T" + String(hour(local_sys)) + ":" + String(minute(local_sys)) + ":" + String(second(local_sys)));
#endif
}

myClock::~myClock()
{
  //not that the clock object would ever get destroyed, this project is a clock after all...
  delete TZ;
}

myClock Clock;
