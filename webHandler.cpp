#include"webHandler.h"

AsyncCallbackJsonWebHandler* webHandler::newHandler(String path, bool(*inner_function)(JsonObject&))
{
  return new AsyncCallbackJsonWebHandler(path, [inner_function](AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject jsonObj = json.as<JsonObject>();
    if(inner_function(jsonObj))
      request->send(200);
    else
      request->send(500);
  });
}

#ifdef DEBUG
bool webHandler::debugAction(JsonObject& jsonObj)
{
  const char* debug = jsonObj["debug"];
  Serial.println("debug json: " + String(debug));
  return true;
}
#endif

bool webHandler::colorHAction(JsonObject& jsonObj)
{
  std::lock_guard<std::recursive_mutex> lock(Colors.color_mutex);
  const char* hcolor = jsonObj["hcolor"];
  CRGB color = HTMLHexToColor(String(hcolor));
  Colors.h = color;
  return Config.writeColors();
}

bool webHandler::colorMAction(JsonObject& jsonObj)
{
  std::lock_guard<std::recursive_mutex> lock(Colors.color_mutex);
  const char* mcolor = jsonObj["mcolor"];
  CRGB color = HTMLHexToColor(String(mcolor));
  Colors.min = color;
  return Config.writeColors();
}

bool webHandler::colorSAction(JsonObject& jsonObj)
{
  std::lock_guard<std::recursive_mutex> lock(Colors.color_mutex);
  const char* scolor = jsonObj["scolor"];
  CRGB color = HTMLHexToColor(String(scolor));
  Colors.sec = color;
  return Config.writeColors();
}

bool webHandler::colorBGAction(JsonObject& jsonObj)
{
  std::lock_guard<std::recursive_mutex> lock(Colors.color_mutex);
  const char* bgcolor = jsonObj["bgcolor"];
  CRGB color = HTMLHexToColor(String(bgcolor));
  Colors.bg = color;
  return Config.writeColors();
}

bool webHandler::brightnessMinAction(JsonObject& jsonObj)
{
  int brightness = jsonObj["minbright"];
  Brightness.min = brightness;
  return Config.writeBrightness();
}

bool webHandler::brightnessMaxAction(JsonObject& jsonObj)
{
  int brightness = jsonObj["maxbright"];
  Brightness.max = brightness;
  return Config.writeBrightness();
}

bool webHandler::brightnessManualAction(JsonObject& jsonObj)
{
  uint8_t brightness = jsonObj["manubright"];
  Brightness.manual = brightness;
  return Config.writeBrightness();
}

bool webHandler::timeAction(JsonObject& jsonObj)
{
  int    Year = jsonObj["year"];
  int8_t Month= jsonObj["mon"];
  int8_t Day  = jsonObj["day"];
  int8_t Hour = jsonObj["hour"];
  int8_t Minute=jsonObj["min"];
  int8_t Second=jsonObj["sec"];
  
  #ifdef DEBUG
  Serial.println("Year:\t" + String(Year));
  Serial.println("Month:\t"+ String(Month));
  Serial.println("Day:\t"  + String(Day));
  Serial.println("Hour:\t" + String(Hour));
  Serial.println("Minute:\t"+String(Minute));
  Serial.println("Second:\t"+String(Second));
  #endif
  
  return Config.setLocTimeAndWrite(Year, Month, Day, Hour, Minute, Second);
}

bool webHandler::rule1Action(JsonObject& jsonObj)
{
  std::lock_guard<std::recursive_mutex> lock(Rule1.rule_mutex);
  Rule1.abbr = String((const char*)(jsonObj["abbr"]));
  Rule1.mon  = String((const char*)(jsonObj["mon"]));
  Rule1.week = String((const char*)(jsonObj["week"]));
  Rule1.dow  = String((const char*)(jsonObj["dow"]));
  uint8_t h = jsonObj["hour"];
  int offs = jsonObj["offs"];
  Rule1.hour = h;
  Rule1.offs = offs;
  return Config.writeRule1();
}

bool webHandler::rule2Action(JsonObject& jsonObj)
{
  std::lock_guard<std::recursive_mutex> lock(Rule2.rule_mutex);
  Rule2.abbr = String((const char*)(jsonObj["abbr"]));
  Rule2.mon  = String((const char*)(jsonObj["mon"]));
  Rule2.week = String((const char*)(jsonObj["week"]));
  Rule2.dow  = String((const char*)(jsonObj["dow"]));
  uint8_t h = jsonObj["hour"];
  int offs = jsonObj["offs"];
  Rule2.hour = h;
  Rule2.offs = offs;
  bool success = Config.writeRule2();
  Clock.updateTimezone();
  return success;
}

bool webHandler::wifiAction(JsonObject& jsonObj)
{
  std::lock_guard<std::recursive_mutex> lock(WifiCredentials.wifi_mutex);
  WifiCredentials.ssid = String((const char*)(jsonObj["ssid"]));
  WifiCredentials.pass = String((const char*)(jsonObj["pass"]));
  return Config.writeWiFi();
}

bool webHandler::ntpServerAction(JsonObject& jsonObj)
{
  std::lock_guard<std::recursive_mutex> lock(NTP.server_mutex);
  NTP.server = String((const char*)(jsonObj["ntpserv"]));
  return Config.writeNTP();
}

bool webHandler::ntpManualAction(JsonObject& jsonObj)
{
  bool manu = jsonObj["ntpmanu"];
  #ifdef DEBUG
  if(manu)
    Serial.println("Time sync set to manual.");
  else
    Serial.println("Time sync set to NTP.");
  #endif
  NTP.manual = manu;
  if(manu && WiFi.status() == WL_CONNECTED)
    setSyncProvider(myClock::timeFromRTC);
  else
    setSyncProvider(myClock::timeFromNTP);
  return Config.writeNTP();
}
