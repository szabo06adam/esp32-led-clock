# ESP32 LED Clock
This project imitates an analogue clock by using an LED strip with 60 individually addressable LEDs on it.  
The timekeeping happens with the help of an RTC (DS3234) and/or an NTP server.  
Certain parameters of the clock are configurable through a web site hosted on the ESP32.  
The clock also features automatic brightness control if a photoresistor is available.  

## Main features
- Accurate timekeeping
- [Automatic brightness scaling](#brightness-scaling) based on incoming light
- Configurable parameters through a web server
  - Current date and time
  - Time zone (based on minutes offset from UTC and dates when to change to/from summer time)
  - [Colour of clock hands](#colours) (hour, minute, second, and even the background)
  - WiFi parameters (SSID, password)
  - NTP server (server address, and whether to use NTP or always sync to RTC)
  - Brightness scaling (enable/disable automatic brightness, and where the minimum and maximum brightness of the LEDs should be based on the incoming light)
- Persistent configuration: user settings are saved/loaded to/from the flash memory, so you can set everything once and never touch it again
- Automatically connect to specified WiFi, [or creates an access point](#wifi-apstation-mode)
- Show progress through hour (explained [below](#hour-progress))

### Colours
Not only the current hand positions light up, but also everything before them.  
If several hands are past an LED, then whatever hand colour has the highest priority will show.  
Colour priority:  
1. Hour/hour progress
2. Minute
3. Second  

Except for the current actual hand position, which is reversed (so current minute will show over hour, and current second will show over both hour and minute).  
Example: if it's 01:05:05 then all hands would physically be pointing towards 5 minute (5th LED).  
In this case hour would show on LED 5, hour progress on LED 6, minute from 1 to 5 and second also from 1 to 5.  
This will result in:  
LED 1-4 showing the minute colour.  
LED 5   showing the second colour.  
LED 6   showing the hour progress colour.  
So for this second the actual hour hand would not be visible.  

### Brightness scaling
A photoresistor can be used to get a voltage reading based on the strength of incoming light. This will get converted to a number between 0 and 4095, where 0 is darkness and 4095 is the maximum light the resistor can change its resistance for.  
You can decide where the minimum and maximum brightness of the LEDs should be on this scale. Between those two values a linear scaling happens.  

### WiFi AP/Station mode
If the ESP cannot connect to the network with the given parameters (SSID, password) it will create one with those values.

### Hour progress
Since the continuous analogue movement of an hour hand cannot be replicated with only five discrete positions per each hour (and because I like it) I opted to choose the following system:  
The exact hour always lights up (unless "obscured by an other hand", detailed above), and the following four LEDs are used to indicate which 15 minute period of an hour it currently is. So from 0 to 14 minutes only the first hour progress lights up, from 15 to 29 the first and the second, an so on...  
Hour progress colour is the hour colour but one third the brightness (well, the RGB values are multiplied by 1/3), this can be changed in globals.h by editing the CLOCK_HOUR_PROGRESS_DIM macro.

## Requirements
- ESP32 (necessitated not only by wireless functionality, but some parts of the code utilise both cores of the MCU)
- Individually addressable LED strip with 60 LEDs that [can communicate with FastLED](https://github.com/FastLED/FastLED#supported-led-chipsets)
- DS3234 RTC (currently no other type of RTC is supported. Also it is technically not a hard requirement, the clock will function without any RTC, but it obviously won't be able to keep the time when not being powered)
- WiFi connection (soft requirement, the NTP time synchronisation won't work without an internet connection, but the configuration web page will be available through AP mode, if not connected to an existing network)
- Photoresistor (soft requirement, you can disable the brightness scaling functionality and set a constant brightness)  
## Images
Sorry but at this time I haven't managed to source a house for it yet, so I don't have any pictures of my own, because everything is in pieces with a wire jungle.  
If done well it should look something like the example linked [below](#inspired-by).
## Credits/Kudos
While the project is entirely my own doing and all the uploaded code is mine, I didn't come up with (most of) the idea itself.  
### Inspired by
https://www.instructables.com/Bamboo-digital-clock/  
The base of this project, an LED strip controlled by an Arduino, timekeeping with an RTC.  
### Libraries
Of course I also utilised public libraries in my code for controlling the LEDs, connecting to WiFi, etc...  
Here is the complete list of the used external libraries:  
- [Time](https://github.com/PaulStoffregen/Time) - timekeeping
- [Timezone](https://github.com/JChristensen/Timezone) - adjusting internal UTC time for local time zone, handling summer/standard time changes
- [FastLED](https://github.com/FastLED/FastLED) - controlling the connected LEDs
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) - hosting the configuration website
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) - handling the configuration changes
- [SparkFun DS3234 Arduino Library](https://github.com/sparkfun/SparkFun_DS3234_RTC_Arduino_Library) - interfacing with the RTC
- [NTPClient](https://github.com/arduino-libraries/NTPClient) - communication with the NTP server  
## Instructions
- Get the necessary libraries with the Arduino IDE library manager
- Open globals.h with the Arduino IDE or a text editor and change the settings to suit your needs.
- Either:
  - create a sketch based on the given [example](./FLASH_INIT.md) to initialise the flash storage, and set the configuration with your chosen values (this way is recommended if you already have used the flash to store persistent information)
  - just upload the main sketch and you will be able to change the default values later using the web page (I guess if you're content with the default values you don't even need to visit the site)
- Open led_clock.ino with the Arduino IDE and upload the sketch to your connected device
- Upload the contents of the data directory to SPIFFS either by:
  - using the [Arduino ESP32FS plugin](https://github.com/me-no-dev/arduino-esp32fs-plugin) in the IDE (recommended)
  - using esptool.py ([documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/spiffs.html))
- Restart your device, still connected to your computer and watch serial monitor for the IP address (it is typically 192.168.4.1 in AP mode, and it is based on your network settings in station mode)
- If you didn't change the default WiFi settings it should create a new network with the default SSID and no password
- Connect to the same WiFi with another device and access the configuration web page by typing in the previous IP in a browser (it might warn you that the site is not secure, which it isn't, but it is only accessible on local network, it's fine)
- If you haven't before, change the settings to your liking  
(changing WiFi SSID and password require restarting the device, all other options should get applied within seconds of sending)  
## Tinkering
You are welcome to make any changes you want, and if you feel like your edit/addition has a place in the main repo create a PR.  
The main thing you want to change is probably the web page, it does its job, but isn't very pretty... (sorry, I don't like web development)  
For exactly this reason I made the web page modular, the page, the stylesheet and the script are in separate files, so you don't have to read hundreds of lines of spaghetti to change the background colour.  
All components of the web site are in the data directory, you only need to change those and upload it to SPIFFS again, no need to recompile and upload the code. (unless you made feature changes of course, but in that case you probably don't need to be told what to do)  
You can write placeholder strings in the html, css and js files that can be replaced by the web server. What value replace what placeholder can be viewed in httpFileProcessor.cpp, this is also where you can expand and add your own ones.  
Should you wish to change something in the code, have a look at globals.h, where every option is explained. If you want to change something not else, have a look in the source files, I tried to group logically similar functions together, I also made an effort to keep each file at a maximum of ~250 lines to make them readable.  
