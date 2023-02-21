#ifndef STRUCTS_H //include guard
#define STRUCTS_H
#include<map>         //std::map
#include<atomic>      //std::atomic
#include<mutex>
#include<FastLED.h>   //CRGB structure
#include<Timezone.h>  //TimeChangeRule class
#include"globals.h"

struct wifi_cred
{
  std::recursive_mutex wifi_mutex;
  String ssid = "ssid";
  String pass = "pass";
};

struct colors
{
  std::recursive_mutex color_mutex;
  CRGB sec = CRGB::Blue;
  CRGB min = CRGB::Green;
  CRGB h   = CRGB::Red;
  CRGB bg  = CRGB::Black;
};

struct brightness
{
  std::atomic<uint16_t> min { 0 };
  std::atomic<uint16_t> max { 4096 };
  std::atomic<uint8_t>  manual { 0 };
};

struct timezone_rule
{
  std::recursive_mutex rule_mutex;
  String abbr = "DST";
  String mon  = "Jan";
  String week = "First";
  String dow  = "Mon";
  std::atomic<uint8_t>  hour { 0 };
  std::atomic<int>      offs { 0 };
};

struct ntp
{
  std::recursive_mutex server_mutex;
  String server = "pool.ntp.org"; //good default, no need to change it, but feel free if you want to
  std::atomic<bool> manual { false };
};

bool tzrEquals(timezone_rule& rule1, timezone_rule& rule2);
TimeChangeRule tzr_to_TCR(timezone_rule& rule);

extern wifi_cred WifiCredentials;
extern colors Colors;
extern brightness Brightness;
extern ntp NTP;
extern timezone_rule Rule1;
extern timezone_rule Rule2;
#endif
