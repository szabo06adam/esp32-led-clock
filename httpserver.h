#ifndef HTTPSERVER_H  //include guard
#define HTTPSERVER_H
#include<WiFi.h>               //WiFi connection
#include<SPIFFS.h>             //File reading from flash memory
#include<ESPAsyncWebServer.h>  //Serving web server
#include<AsyncJson.h>          //JSON parsing
#include<ArduinoJson.h>        //JSON parsing
#include<FastLED.h>            //CRGB struct
#include"globals.h"
#include"structs.h"
#include"myConfig.h"
#include"webHandler.h"
#include"httpFileProcessor.h"

class httpserver
{
  private:
  AsyncWebServer server = AsyncWebServer(HTTP_SERVER_PORT);

  public:
  httpserver();

  bool start();
  void stop();
  
};

extern httpserver HTTPServer;
#endif
