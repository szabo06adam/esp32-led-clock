//Includes:
#include <WiFi.h>               //WiFi connection
#include <freertos/FreeRTOS.h>  //Multithreading
#include <thread>               //Multithreading
#include <esp_pthread.h>        //Multithreading
#include <chrono>               //types for this_thread::sleep_for()
#include "globals.h"            //Global macros, settings
#include "structs.h"            //Structs where we store the preferences
#include "myClock.h"            //Displaying clock hands, timekeeping related features
#include "myConfig.h"           //Reading/saving configuration from/to flash
#include "httpserver.h"         //HTTP web server

/*/-----------------------------------\*/
/*|           Thread funcs            |*/
/*\-----------------------------------/*/

esp_pthread_cfg_t create_config(const char *name, int core_id, int stack_size, int priority)
{
    auto cfg = esp_pthread_get_default_config();
    cfg.thread_name = name;
    cfg.pin_to_core = core_id;
    cfg.stack_size = stack_size;
    cfg.prio = priority;
    return cfg;
} //https://github.com/espressif/esp-idf/blob/6f0bea38cdbc0b6286f1d7c3a53178cdf108fc1a/examples/cxx/pthread/main/cpp_pthread.cpp

/*/-----------------------------------\*/
/*|               Setup               |*/
/*\-----------------------------------/*/

void setup()
{
  Serial.begin(115200);

  #ifdef DEBUG
  printConfig();            //Printing current config (before loading anything)
  #endif

  if(!Config.readRule1())
    Serial.println("Couldn't read timezone (1/2) preferences!");
  if(!Config.readRule2())
    Serial.println("Couldn't read timezone (2/2) preferences!");
  if(!Config.readColors())
    Serial.println("Couldn't read colour preferences!");
  if(!Config.readBrightness())
    Serial.println("Couldn't read brightness preferences!");
  if(!Config.readWiFi())
    Serial.println("Couldn't read WiFi preferences!");
  if(!Config.readNTP())
    Serial.println("Couldn't read NTP preferences!");
  
  #ifdef DEBUG
  printConfig();            //Printing current config (after loading saved values)
  #endif

  auto clock_cfg = create_config("Clock tick-tock thread", 0, 10000, 1);
  esp_pthread_set_cfg(&clock_cfg);
  std::thread clock_tick(TickTock);
  #ifdef DEBUG
  Serial.println("DEBUG:\tsetup(): created \"Clock tick-tock thread\".");
  #endif

  auto wifi_cfg = create_config("WiFi connect thread", 1, 10000, 10);
  esp_pthread_set_cfg(&wifi_cfg);
  std::thread connect_wifi(connectToWifi);
  #ifdef DEBUG
  Serial.println("DEBUG:\tsetup(): created \"WiFi connect thread\".");
  #endif

  connect_wifi.join(); //Don't begin NTP sync and HTTP server threads until we connect to WiFi network.

  auto ntp_cfg = create_config("NTP begin thread", 1, 10000, 5);
  esp_pthread_set_cfg(&ntp_cfg);
  std::thread begin_ntp(beginNTPSync);
  #ifdef DEBUG
  Serial.println("DEBUG:\tsetup(): created \"NTP begin thread\".");
  #endif

  auto http_cfg = create_config("Start HTTP server thread", 1, 10000, 1);
  esp_pthread_set_cfg(&http_cfg);
  std::thread start_http(startHTTPServ);
  #ifdef DEBUG
  Serial.println("DEBUG:\tsetup(): created \"Start HTTP server thread\".");
  #endif

  clock_tick.detach();
  start_http.detach();
  begin_ntp.join();
  
  #ifdef DEBUG
  Serial.println("DEBUG:\tsetup() finished.");
  #endif
}

/*/-----------------------------------\*/
/*|              Tasks                |*/
/*\-----------------------------------/*/

void connectToWifi()
{
  #ifdef DEBUG
  Serial.println("DEBUG:\tWiFi connect thread on Core " + (String)xPortGetCoreID() + " started.");
  #endif

  Serial.println();
  std::lock_guard<std::recursive_mutex> lock(WifiCredentials.wifi_mutex);
  
  //NEW
  WiFi.mode(WIFI_STA);
  WiFi.begin(WifiCredentials.ssid.c_str(), WifiCredentials.pass.c_str());
  std::this_thread::sleep_for(std::chrono::milliseconds(WIFI_CONN_ATTEMPT_MS));
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Couldn't find specified network, starting AP...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WifiCredentials.ssid.c_str(), WifiCredentials.pass.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(WIFI_AP_IP_DELAY_MS));
    Serial.println();
    Serial.println("AP created");
    Serial.println("IP address: ");
    Serial.println(WiFi.softAPIP().toString());
  }
  else
  {
    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP().toString());
  }

  #ifdef DEBUG
  Serial.println("DEBUG:\tWiFi connect thread on Core " + (String)xPortGetCoreID() + " finished.");
  #endif
}

void beginNTPSync()
{
  #ifdef DEBUG
  Serial.println("DEBUG:\tNTP begin thread on Core " + (String)xPortGetCoreID() + " started.");
  #endif
  
  if(WiFi.status() == WL_CONNECTED && !NTP.manual)
  {
    setSyncProvider(myClock::timeFromNTP);
  }

  #ifdef DEBUG
  Serial.println("DEBUG:\tNTP begin thread on Core " + (String)xPortGetCoreID() + " finished.");
  #endif
}

void TickTock()
{
  #ifdef DEBUG
  Serial.println("DEBUG:\tClock tick-tock thread on Core " + (String)xPortGetCoreID() + " started.");
  #endif

  Clock.begin();
  
  for (;;)  //Infinite loop
  {
    Clock.tick();

    std::this_thread::sleep_for(std::chrono::milliseconds{CLOCKFACE_UPDATE_FREQ_MS});
  }
  #ifdef DEBUG
  Serial.println("DEBUG:\tClock tick-tock thread on Core " + (String)xPortGetCoreID() + " finished.");
  /*This will never run, but consistency is nice.*/
  #endif
}

void startHTTPServ()
{
  #ifdef DEBUG
  Serial.println("DEBUG:\tStart HTTP server thread on Core " + (String)xPortGetCoreID() + " started.");
  Serial.println("WiFi mode: " + String(WiFi.getMode()));
  #endif

  if(WiFi.getMode() == WIFI_MODE_AP || WiFi.status() == WL_CONNECTED)
  {
    #ifdef DEBUG
    Serial.println("Starting HTTP server...");
    #endif
    HTTPServer.start();
  }

  #ifdef DEBUG
  Serial.println("DEBUG:\tHTTP server thread on Core " + (String)xPortGetCoreID() + " finished.");
  #endif
}

#ifdef DEBUG
void printConfig()
{
  Serial.println("Printing config from RAM...");
  //WiFi
  Serial.print("WiFi:\nSSID:\t");
  Serial.print(WifiCredentials.ssid);
  Serial.print("\tPASS:\t");
  Serial.println(WifiCredentials.pass);
  //NTP
  Serial.println("NTP:\nserv:\t" + String(NTP.server) + "\tmanu:\t" + String(NTP.manual));
  //Timezone rule 1
  Serial.print("TZ1:\n");
  Serial.print(Rule1.mon);
  Serial.print(", ");
  Serial.print(Rule1.week);
  Serial.print(", ");
  Serial.print(Rule1.dow);
  Serial.print(", ");
  Serial.print(Rule1.hour);
  Serial.print(", ");
  Serial.println(Rule1.offs);
  //Timezone rule 2
  Serial.print("TZ2:\n");
  Serial.print(Rule2.mon);
  Serial.print(", ");
  Serial.print(Rule2.week);
  Serial.print(", ");
  Serial.print(Rule2.dow);
  Serial.print(", ");
  Serial.print(Rule2.hour);
  Serial.print(", ");
  Serial.println(Rule2.offs);
  //Colours
  Serial.println(String("sec:\t") + String(Colors.sec.r) + "," + String(Colors.sec.green) + "," + String(Colors.sec.blue));
  Serial.println(String("min:\t") + String(Colors.min.r) + "," + String(Colors.min.green) + "," + String(Colors.min.blue));
  Serial.println(String("h:\t")   + String(Colors.h.r)   + "," + String(Colors.h.green)   + "," + String(Colors.h.blue));
  Serial.println(String("bg:\t")  + String(Colors.bg.r)  + "," + String(Colors.bg.green)  + "," + String(Colors.bg.blue));
  //Brightness
  Serial.println(String("Bright:\nmin:\t") + String(Brightness.min) + "\nmax:\t" + String(Brightness.max) + "\nmanual:\t" + String(Brightness.manual));
}
#endif

/*/-----------------------------------\*/
/*|               Loop                |*/
/*\-----------------------------------/*/

void loop() { }
