#include"structs.h"

/*WiFi ssid, password*/
wifi_cred WifiCredentials;
/*LED colours for hour,minute,second hand (and background)*/
colors Colors;
/*LED brightness minimum, maximum points
 *according to incoming light strength*/
brightness Brightness;
/*NTP server*/
ntp NTP;
/*Timezone rules*/
timezone_rule Rule1;
timezone_rule Rule2;

std::map<String, month_t> month_map = {
  {"Jan", month_t::Jan},
  {"Feb", month_t::Feb},
  {"Mar", month_t::Mar},
  {"Apr", month_t::Apr},
  {"May", month_t::May},
  {"Jun", month_t::Jun},
  {"Jul", month_t::Jul},
  {"Aug", month_t::Aug},
  {"Sep", month_t::Sep},
  {"Oct", month_t::Oct},
  {"Nov", month_t::Nov},
  {"Dec", month_t::Dec}
};

std::map<String, week_t> week_map = {
  {"Last", week_t::Last},
  {"First", week_t::First},
  {"Second", week_t::Second},
  {"Third", week_t::Third},
  {"Fourth", week_t::Fourth}
};

std::map<String, dow_t> dow_map = {
  {"Mon", dow_t::Mon},
  {"Tue", dow_t::Tue},
  {"Wed", dow_t::Wed},
  {"Thu", dow_t::Thu},
  {"Fri", dow_t::Fri},
  {"Sat", dow_t::Sat},
  {"Sun", dow_t::Sun}
};

/*/-----------------------------------\*/
/*|         Struct Conversion         |*/
/*\-----------------------------------/*/

bool tzrEquals(timezone_rule& rule1, timezone_rule& rule2)
{
  std::lock(rule1.rule_mutex, rule2.rule_mutex);
  std::lock_guard<std::recursive_mutex> r1lock(rule1.rule_mutex, std::adopt_lock);
  std::lock_guard<std::recursive_mutex> r2lock(rule2.rule_mutex, std::adopt_lock);
  
  return rule1.mon == rule2.mon &&
         rule1.week == rule2.week &&
         rule1.dow == rule2.dow &&
         rule1.hour == rule2.hour &&
         rule1.offs == rule2.offs;
}

/*/-----------------------------------\*/
/*|         Struct Conversion         |*/
/*\-----------------------------------/*/

TimeChangeRule tzr_to_TCR(timezone_rule& rule)
{
  std::lock_guard<std::recursive_mutex> lock(rule.rule_mutex);
  
  TimeChangeRule TCR = {"def", week_map[rule.week], dow_map[rule.dow], month_map[rule.mon], rule.hour, rule.offs };
  const char* tmp = rule.abbr.c_str();
  strcpy(TCR.abbrev, tmp);
  return TCR;
}
