/*                                                                                                        *\
|* This file is meant to be the only file where you have to change anything for a bit of personalisation. *|
|* Here you can disable/enable features, and change around some nubmers (like pins, or certain delays).   *|
|* Should you wish to make more substantial changes you are more than welcome to dig around in the code.  *|
\*                                                                                                        */

#ifndef GLOBALS_H //include guard
#define GLOBALS_H

//Options:
//#define DEBUG //Writes more information to serial
//#define RETURN_WIFI_PWD
/*Puts in the current WiFi password as value in the web page.
 *this is not recommended because the password will be 
 *readable in plaintext when inspecting the html source*/

//Configuration:
#define CLOCKFACE_UPDATE_FREQ_MS 500  /*The delay between callig the clock update function, thus updating
                                       *the clock hand positions.(Internal time gets updated regardless.)*/
#define SYNC_FREQUENCY 43200          //How often local time should be synced to the time source (seconds)
#define WIFI_CONN_ATTEMPT_MS 5000     /*How long should we try to connect to the given WiFi network before
                                       *deciding it is not available and switching to AP mode*/
#define WIFI_AP_IP_DELAY_MS 5000      //How long to wait before printing AP IP address
#define CLOCK_HOUR_PROGRESS_DIM 0.33f /*The leds representing the progress through the hour will have their rgb
                                       *values multiplied by this to make them dimmer than the full hour*/

#define HTTP_SERVER_PORT 80
/*80 is the default HTTP port. Leave it unchanged if you don't want to specify the port every time you
 *type in the server address in the borwser. There isn't really any reason to change this anyway, unless
 *you are running something else on this port, but if that's the case you probably know what you're doing.*/

#define NUM_LEDS 60       //The number of LEDs on the strip. By the nature of a clock this should be always 60.
#define LED_TYPE WS2812B  //The type of LED controller
#define COLOR_ORDER GRB   //Colour order the controller expects

#define PREF_COLOR "COLOR"  //Name of the partition where the colour configuration is stored (in flash mem)
#define PREF_BRIGHT "BRIGHT"//Name of the partition where the brightness configuration is stored (in flash mem)
#define PREF_RULE1 "TZR1"   //Name of the partition where the timezone rule 1 configuration is stored (in flash mem)
#define PREF_RULE2 "TZR2"   //Name of the partition where the timezone rule 2 configuration is stored (in flash mem)
#define PREF_WIFI "WiFi"    //Name of the partition where the WiFi credentials are stored (in flash mem)
#define PREF_NTP "NTP"      //Name of the partition where the NTP server addrress is stored (in flash mem)

//Pins:
/*These are the default SPI pins on my board, these are not for configuring, just for documenting*/
//#define MOSI 23 //Not used
//#define MISO 19 //Not used
//#define CLK 18  //Not used
//#define SS 5    //Not used

#define LED_DATA 25 //Pin where colour data will be sent to the LEDs. Digital output.
#define LIGHT_IN 34 /*Pin for brightness information (photoresistor). Analogue input. Commenting out this line
                     *disables automatic brighness control*/
#define RTC_PIN 17   //RTC Chip-select pin

#endif
