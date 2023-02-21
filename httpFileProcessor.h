#ifndef HTTP_FILE_PROCESSOR_H //include guard
#define HTTP_FILE_PROCESSOR_H
#include<WiFi.h>    //WiFi connection
#include<TimeLib.h> //Getting system time
#include<FastLED.h> //CRGB struct
#include"globals.h"
#include"myConfig.h"
#include"myClock.h"

String colorToHTMLHex(CRGB col);
CRGB HTMLHexToColor(String hex);
  
String htmlProcessor(const String& var);
String cssProcessor(const String& var);
String jsProcessor(const String& var);

#endif
