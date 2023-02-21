#ifndef WEBHANDLER_H  //include guard
#define WEBHANDLER_H
#include<ESPAsyncWebServer.h>  //AsyncCallbackJsonWebHandler class
#include<AsyncJson.h>          //JsonVariant class
#include<ArduinoJson.h>        //JsonObject class
#include"globals.h"
#include"structs.h"
#include"myConfig.h"
#include"myClock.h"
#include"httpFileProcessor.h"

class webHandler
{
  public:
  static AsyncCallbackJsonWebHandler* newHandler(String path, bool(*inner_function)(JsonObject&));

  static bool debugAction(JsonObject& jsonObj);
  static bool colorHAction(JsonObject& jsonObj);
  static bool colorMAction(JsonObject& jsonObj);
  static bool colorSAction(JsonObject& jsonObj);
  static bool colorBGAction(JsonObject& jsonObj);
  static bool brightnessMinAction(JsonObject& jsonObj);
  static bool brightnessMaxAction(JsonObject& jsonObj);
  static bool brightnessManualAction(JsonObject& jsonObj);
  static bool timeAction(JsonObject& jsonObj);
  static bool rule1Action(JsonObject& jsonObj);
  static bool rule2Action(JsonObject& jsonObj);
  static bool wifiAction(JsonObject& jsonObj);
  static bool ntpServerAction(JsonObject& jsonObj);
  static bool ntpManualAction(JsonObject& jsonObj);
};
#endif
