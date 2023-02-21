#include"httpserver.h"

httpserver::httpserver()
{
  
}

bool httpserver::start()
{
  if(WiFi.getMode() != WIFI_MODE_AP && WiFi.status() != WL_CONNECTED)
    return false;

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return false;
  }
    
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, htmlProcessor);
  });
  
  // Route for style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", String(), false, cssProcessor);
  });

  // Route for script.js file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", String(), false, jsProcessor);
  });

  /*POST Requests:*/
  #ifdef DEBUG
  AsyncCallbackJsonWebHandler* DebugHandler = webHandler::newHandler("/pref/debug", webHandler::debugAction);
  server.addHandler(DebugHandler);
  #endif
  
  //LED preferences:
  AsyncCallbackJsonWebHandler* HColHandler = webHandler::newHandler("/pref/color/h", webHandler::colorHAction);
  server.addHandler(HColHandler);
  
  AsyncCallbackJsonWebHandler* MColHandler = webHandler::newHandler("/pref/color/m", webHandler::colorMAction);
  server.addHandler(MColHandler);

  AsyncCallbackJsonWebHandler* SColHandler = webHandler::newHandler("/pref/color/s", webHandler::colorSAction);
  server.addHandler(SColHandler);

  AsyncCallbackJsonWebHandler* BGColHandler = webHandler::newHandler("/pref/color/bg", webHandler::colorBGAction);
  server.addHandler(BGColHandler);

  AsyncCallbackJsonWebHandler* MinBrightHandler = webHandler::newHandler("/pref/brightness/min", webHandler::brightnessMinAction);
  server.addHandler(MinBrightHandler);

  AsyncCallbackJsonWebHandler* MaxBrightHandler = webHandler::newHandler("/pref/brightness/max", webHandler::brightnessMaxAction);
  server.addHandler(MaxBrightHandler);

  AsyncCallbackJsonWebHandler* ManuBrightHandler = webHandler::newHandler("/pref/brightness/manu", webHandler::brightnessManualAction);
  server.addHandler(ManuBrightHandler);

  //Time and date settings:
  AsyncCallbackJsonWebHandler* TimeHandler = webHandler::newHandler("/pref/time", webHandler::timeAction);
  server.addHandler(TimeHandler);
  
  //Timezone preferences:
  AsyncCallbackJsonWebHandler* STDHandler = webHandler::newHandler("/pref/tz/std", webHandler::rule1Action);
  server.addHandler(STDHandler);
  
  AsyncCallbackJsonWebHandler* DSTHandler = webHandler::newHandler("/pref/tz/dst", webHandler::rule2Action);
  server.addHandler(DSTHandler);

  //WiFi preferences:
  AsyncCallbackJsonWebHandler* WiFiHandler = webHandler::newHandler("/pref/wifi", webHandler::wifiAction);
  server.addHandler(WiFiHandler);

  //NTP preferences:
  AsyncCallbackJsonWebHandler* NTPServHandler = webHandler::newHandler("/pref/ntp/serv", webHandler::ntpServerAction);
  server.addHandler(NTPServHandler);

  AsyncCallbackJsonWebHandler* NTPManuHandler = webHandler::newHandler("/pref/ntp/manu", webHandler::ntpManualAction);
  server.addHandler(NTPManuHandler);

  server.begin();

  return true;
}

void httpserver::stop()
{
  server.end();
}

httpserver HTTPServer;
