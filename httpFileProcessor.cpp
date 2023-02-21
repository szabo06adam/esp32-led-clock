#include"httpFileProcessor.h"

String colorToHTMLHex(CRGB col)
{
  int col_val = (col.red << 16) + (col.green << 8) + col.blue;
  String val_string = String(col_val, HEX);
  String out = "#";
  for(int i = val_string.length(); i < 6; i++)
    out.concat("0");  //Padding zeroes to make the string 6+1(#) long
  out.concat(val_string);
  return out;
}

CRGB HTMLHexToColor(String hex)
{
  int val = (int)strtol(hex.substring(1).c_str(), NULL, 16);
  return CRGB((val & 0xff0000) >> 16, (val & 0xff00) >> 8, (val & 0xff));
}

String htmlProcessor(const String& var)
{
  if(var == "DATETIME")
  {
    time_t current = Clock.getLocalTZTime();
    uint32_t Year = year(current);
    uint32_t Month = month(current);
    uint32_t Day = day(current);
    uint32_t Hour = hour(current);
    uint32_t Minute = minute(current);
    uint32_t Second = second(current);
    
    String out = String(Year);
    out.concat("-");
    if(Month < 10)
      out.concat("0");
    out.concat(Month);
    out.concat("-");
    if(Day < 10)
      out.concat("0");
    out.concat(Day);
    out.concat("T");
    if(Hour < 10)
      out.concat("0");
    out.concat(Hour);
    out.concat(":");
    if(Minute < 10)
      out.concat("0");
    out.concat(Minute);
    out.concat(":");
    if(Second < 10)
      out.concat("0");
    out.concat(Second);
    
    return out;
  }
  else if (var == "DSTBOX")
  {
    if(tzrEquals(Rule1, Rule2))
      return "";
    else
      return "checked";
  }
  else if (var == "R1MON")
  {
    return Rule1.mon;
  }
  else if (var == "R2MON")
  {
    return Rule2.mon;
  }
  else if (var == "R1WEEK")
  {
    return Rule1.week;
  }
  else if (var == "R2WEEK")
  {
    return Rule2.week;
  }
  else if (var == "R1DOW")
  {
    return Rule1.dow;
  }
  else if (var == "R2DOW")
  {
    return Rule2.dow;
  }
  else if (var == "R1H")
  {
    return String(Rule1.hour);
  }
  else if (var == "R2H")
  {
    return String(Rule2.hour);
  }
  else if (var == "R1OFFS")
  {
    return String(Rule1.offs);
  }
  else if (var == "R2OFFS")
  {
    return String(Rule2.offs);
  }
  else if (var == "WIFISSID")
  {
    return String(WifiCredentials.ssid);
  }
  else if (var == "WIFIPASS")
  {
    #ifdef RETURN_WIFI_PWD
    return String(WifiCredentials.pass); //You shouldn't return the password if you don't want it to be visible in plaintext in html
    #else
    return "";
    #endif
  }
  else if (var == "NTPBOX")
  {
    if(NTP.manual)
      return "";
    else
      return "checked";
  }
  else if (var == "NTPSERV")
  {
    return String(NTP.server);
  }
  else if (var == "HCOLOR")
  {
    return colorToHTMLHex(Colors.h);
  }
  else if (var == "MCOLOR")
  {
    return colorToHTMLHex(Colors.min);
  }
  else if (var == "SCOLOR")
  {
    return colorToHTMLHex(Colors.sec);
  }
  else if (var == "BGCOLOR")
  {
    return colorToHTMLHex(Colors.bg);
  }
  else if (var == "MINBRIGHT")
  {
    return String(Brightness.min);
  }
  else if (var == "MAXBRIGHT")
  {
    return String(Brightness.max);
  }
  else if (var == "MANUBRIGHT")
  {
    return String(Brightness.manual);
  }
  else if (var == "BRBOX")
  {
    if(Brightness.manual > 0)
      return "checked";
    else
      return "";
  }
  else
  {
    return "unknown variable";
  }
}

String cssProcessor(const String& var)
{
  if (var == "VARIABLE")
  {
    return "value";
  }
  else
  {
    return "unknown variable";
  }
}

String jsProcessor(const String& var)
{
  if (var == "IP")
  {
    return WiFi.localIP().toString();
  }
  else if (var == "R1MON")
  {
    return Rule1.mon;
  }
  else if (var == "R2MON")
  {
    return Rule2.mon;
  }
  else if (var == "R1WEEK")
  {
    return Rule1.week;
  }
  else if (var == "R2WEEK")
  {
    return Rule2.week;
  }
  else if (var == "R1DOW")
  {
    return Rule1.dow;
  }
  else if (var == "R2DOW")
  {
    return Rule2.dow;
  }
  else
  {
    return "unknown variable";
  }
}
