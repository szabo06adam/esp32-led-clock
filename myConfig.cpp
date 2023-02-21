#include"myConfig.h"
#include"myClock.h"

bool myConfig::setTime(time_t t)
{
  ::setTime(t);
  rtc.setTime(second(), minute(), hour(), weekday(), day(), month(), year() - 2000);
  rtc.update();

#ifdef DEBUG
  Serial.println("system time vs rtc time:");
  Serial.println(String(second()) + " - " + String(rtc.getSecond()));
  Serial.println(String(minute()) + " - " + String(rtc.getMinute()));
  Serial.println(String(hour())   + " - " + String(rtc.getHour()));
  Serial.println(String(day())    + " - " + String(rtc.getDate()));
  Serial.println(String(month())  + " - " + String(rtc.getMonth()));
  Serial.println(String(year())   + " - " + String(rtc.getYear()));
#endif

  return second() == rtc.getSecond() &&
         minute() == rtc.getMinute() &&
         hour()   == rtc.getHour()   &&
         day()    == rtc.getDay()    &&
         month()  == rtc.getMonth()  &&
         CalendarYrToTm(year()) == y2kYearToTm(rtc.getYear());
}

void myConfig::readRule(Preferences& pref, timezone_rule& rule)
{
  //rule.abbr = pref.getString("abbr");
  /*If abbr is saved it can read it, but there is no point really...*/
  rule.mon  = pref.getString("mon");
  rule.week = pref.getString("week");
  rule.dow  = pref.getString("dow");
  rule.hour = pref.getUChar("hour");
  rule.offs = pref.getUInt("offs");
}

void myConfig::writeRule(Preferences& pref, timezone_rule& rule)
{
  //pref.putString("abbr", rule.abbr); //There's no reason to write abbr to flash.
  pref.putString("mon",  rule.mon);
  pref.putString("week", rule.week);
  pref.putString("dow",  rule.dow);
  pref.putUChar("hour",  rule.hour);
  pref.putUInt("offs",   rule.offs);
}

myConfig::myConfig()
{
  //initialise flash storage
  esp_err_t err = nvs_flash_init();
  ESP_ERROR_CHECK( err );
}

bool myConfig::readRule1()
{
  std::lock_guard<std::recursive_mutex> lock(Rule1.rule_mutex);
  Preferences pref;
  if (pref.begin(PREF_RULE1, true))
  {
    readRule(pref, Rule1);
    pref.end();
#ifdef DEBUG
    Serial.println(String("Read:\nTZ1:\t") + String(Rule1.mon) + ", " + String(Rule1.week) + ", " + String(Rule1.dow) + ", " + String(Rule1.hour) +
                   ", " + String(Rule1.offs));
#endif
    return true;
  }
  else
  {
#ifdef DEBUG
    Serial.println(String("Could not open ") + String(PREF_RULE1) + String(" preference partition."));
#endif
    return false;
  }
}

bool myConfig::readRule2()
{
  std::lock_guard<std::recursive_mutex> lock(Rule2.rule_mutex);
  Preferences pref;
  if (pref.begin(PREF_RULE2, true))
  {
    readRule(pref, Rule2);
    pref.end();
#ifdef DEBUG
    Serial.println(String("Read:\nTZ2:\t") + String(Rule2.mon) + ", " + String(Rule2.week) + ", " + String(Rule2.dow) + ", " + String(Rule2.hour) +
                   ", " + String(Rule2.offs));
#endif
    return true;
  }
  else
  {
#ifdef DEBUG
    Serial.println(String("Could not open ") + String(PREF_RULE2) + String(" preference partition."));
#endif
    return false;
  }
}

bool myConfig::readWiFi()
{
  std::lock_guard<std::recursive_mutex> lock(WifiCredentials.wifi_mutex);
  Preferences pref;
  if (pref.begin(PREF_WIFI, true))
  {
    WifiCredentials.ssid = pref.getString("ssid");
    WifiCredentials.pass = pref.getString("pass");
    pref.end();
#ifdef DEBUG
    Serial.println(String("Read:\nSSID:\t") + String(WifiCredentials.ssid) + String("\nPASS:\t") + String(WifiCredentials.pass));
#endif
    return true;
  }
  else
  {
#ifdef DEBUG
    Serial.println(String("Could not open ") + String(PREF_WIFI) + String(" preference partition."));
#endif
    return false;
  }
}

bool myConfig::readNTP()
{
  std::lock_guard<std::recursive_mutex> lock(NTP.server_mutex);
  Preferences pref;
  if (pref.begin(PREF_NTP, true))
  {
    NTP.server = pref.getString("serv");
    NTP.manual = pref.getBool("manu");
    pref.end();
#ifdef DEBUG
    Serial.println(String("Read:\nserver:\t") + String(NTP.server) + "\nmanual:\t" + String(NTP.manual));
#endif
    return true;
  }
  else
  {
#ifdef DEBUG
    Serial.println(String("Could not open ") + String(PREF_NTP) + String(" preference partition."));
#endif
    return false;
  }
}

bool myConfig::readColors()
{
  std::lock_guard<std::recursive_mutex> lock(Colors.color_mutex);
  Preferences pref;
  if (pref.begin(PREF_COLOR, true))
  {
    Colors.sec = CRGB(pref.getUChar("secR"), pref.getUChar("secG"), pref.getUChar("secB"));
    Colors.min = CRGB(pref.getUChar("minR"), pref.getUChar("minG"), pref.getUChar("minB"));
    Colors.h   = CRGB(pref.getUChar("hR"),   pref.getUChar("hG"),   pref.getUChar("hB"));
    Colors.bg  = CRGB(pref.getUChar("bgR"),  pref.getUChar("bgG"),  pref.getUChar("bgB"));
    pref.end();
#ifdef DEBUG
    Serial.println(String("Read:\nsec:\t") + String(Colors.sec.r) + "," + String(Colors.sec.green) + "," + String(Colors.sec.blue));
    Serial.println(String("min:\t") + String(Colors.min.r) + "," + String(Colors.min.green) + "," + String(Colors.min.blue));
    Serial.println(String("h:\t")   + String(Colors.h.r)   + "," + String(Colors.h.green)   + "," + String(Colors.h.blue));
    Serial.println(String("bg:\t")  + String(Colors.bg.r)  + "," + String(Colors.bg.green)  + "," + String(Colors.bg.blue));
#endif
    return true;
  }
  else
  {
#ifdef DEBUG
    Serial.println(String("Could not open ") + String(PREF_COLOR) + String(" preference partition."));
#endif
    return false;
  }
}

bool myConfig::readBrightness()
{
  //no need to lock with mutex, every member is atomic
  Preferences pref;
  if (pref.begin(PREF_BRIGHT, true))
  {
    Brightness.min = pref.getUShort("minB");
    Brightness.max = pref.getUShort("maxB");
    Brightness.manual = pref.getUChar("manu");
    pref.end();
#ifdef DEBUG
    Serial.println(String("Read:\nmin:\t") + String(Brightness.min) + "\nmax:\t" + String(Brightness.max) + "\nmanual:\t" + String(Brightness.manual));
#endif
    return true;
  }
  else
  {
#ifdef DEBUG
    Serial.println(String("Could not open ") + String(PREF_BRIGHT) + String(" preference partition."));
#endif
    return false;
  }
}

bool myConfig::setUTCTimeAndWrite(int Year, int Month, int Day, int Hour, int Minute, int Second)
{
  TimeElements tm;
  tm.Second = Second;
  tm.Minute = Minute;
  tm.Hour = Hour;
  tm.Day = Day;
  tm.Month = Month;
  tm.Year = CalendarYrToTm(Year);
  time_t time_to_set = makeTime(tm);

  return myConfig::setTime(time_to_set);
}

bool myConfig::setLocTimeAndWrite(int Year, int Month, int Day, int Hour, int Minute, int Second)
{
  TimeElements tm;
  tm.Second = Second;
  tm.Minute = Minute;
  tm.Hour = Hour;
  tm.Day = Day;
  tm.Month = Month;
  tm.Year = CalendarYrToTm(Year);
  time_t local_time_to_set = makeTime(tm);
#ifdef DEBUG
  Serial.println("Local timezone time_t created.");
#endif
  time_t time_to_set = Clock.getTimezone()->toUTC(local_time_to_set);
#ifdef DEBUG
  Serial.println("UTC time_t created:" + String((int64_t)time_to_set));
#endif

  return myConfig::setTime(time_to_set);
}

bool myConfig::writeRule1()
{
  std::lock_guard<std::recursive_mutex> lock(Rule1.rule_mutex);
  Preferences pref;
  if (pref.begin(PREF_RULE1, false))
  {
    writeRule(pref, Rule1);
    pref.end();
    return true;
  }
  else
    return false;
}

bool myConfig::writeRule2()
{
  std::lock_guard<std::recursive_mutex> lock(Rule2.rule_mutex);
  Preferences pref;
  if (pref.begin(PREF_RULE2, false))
  {
    writeRule(pref, Rule2);
    pref.end();
    return true;
  }
  else
    return false;
}

bool myConfig::writeWiFi()
{
  std::lock_guard<std::recursive_mutex> lock(WifiCredentials.wifi_mutex);
  Preferences pref;
  if (pref.begin(PREF_WIFI, false))
  {
    pref.putString("ssid", WifiCredentials.ssid);
    pref.putString("pass", WifiCredentials.pass);
    pref.end();
    return true;
  }
  else
    return false;
}

bool myConfig::writeNTP()
{
  std::lock_guard<std::recursive_mutex> lock(NTP.server_mutex);
  Preferences pref;
  if (pref.begin(PREF_NTP, false))
  {
    pref.putString("serv", NTP.server);
    pref.putBool("manu", NTP.manual);
    pref.end();
    return true;
  }
  else
    return false;
}

bool myConfig::writeColors()
{
  std::lock_guard<std::recursive_mutex> lock(Colors.color_mutex);
  Preferences pref;
  if (pref.begin(PREF_COLOR, false))
  {
    //Second R,G,B:
    pref.putUChar("secR", Colors.sec.red);
    pref.putUChar("secG", Colors.sec.green);
    pref.putUChar("secB", Colors.sec.blue);
    //Minute R,G,B:
    pref.putUChar("minR", Colors.min.red);
    pref.putUChar("minG", Colors.min.green);
    pref.putUChar("minB", Colors.min.blue);
    //Hour R,G,B:
    pref.putUChar("hR", Colors.h.red);
    pref.putUChar("hG", Colors.h.green);
    pref.putUChar("hB", Colors.h.blue);
    //Background R,G,B:
    pref.putUChar("bgR", Colors.bg.red);
    pref.putUChar("bgG", Colors.bg.green);
    pref.putUChar("bgB", Colors.bg.blue);
    pref.end();
    return true;
  }
  else
    return false;
}

bool myConfig::writeBrightness()
{
  //no need to lock with mutex, every member is atomic
  Preferences pref;
  if (pref.begin(PREF_BRIGHT, false))
  {
    pref.putUShort("minB", Brightness.min);
    pref.putUShort("maxB", Brightness.max);
    pref.putUChar("manu",  Brightness.manual);
    pref.end();
    return true;
  }
  else
    return false;
}

myConfig Config;
