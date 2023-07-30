/***********************************************************************************
* SETTINGS
* 
* It is important that you work through this file and and change the parameters
* when needed so.
*
* Download latest version and instructions:  https://github.com/RoelKroes/TBTracker-RX
************************************************************************************/
// Development flag
// Set to false if you want the software to upload and store your telemetry in the sondehub database
// Set to true if you only want to upload the telemetry but not store it in the database (recommended when testing)
// If DEVFLAG is set to true, the Serial output will still let you know if an upload to Sondehub succeeded
#define DEVFLAG false

/***********************************************************************************
* WIFI SETTINGS
*  
* you can define three SSID's with password here.
* For example: your home network, you phone's mobile hotspot and maybe another network
* You need to specify at least 1 SSID and password 
************************************************************************************/
#define WIFI_SSID_1  "MySSID"
#define WIFI_PASSWORD_1 "MyPassword"
 
#define WIFI_SSID_2  "MyMobileHotspot"
#define WIFI_PASSWORD_2 "MyOtherPassword"

#define WIFI_SSID_3  ""
#define WIFI_PASSWORD_3 ""

/************************************************************************
* UPLOADER INFO
*  
* Change this!
* This is what is shown on Sondehub
************************************************************************************/
// Callsign to show up on Sondehub as the "Receiver"
#define CALLSIGN "MYCALL"
// Contact e-mail (optional)
#define UPLOADER_EMAIL "somebody@somedomain.org"
// Antenna used (optional)
#define ANTENNA_USED "GP"
// Radio used (optional)
#define RADIO_USED "T-Beam"
// Display the mobile flag (car icon) on the map
#define I_AM_MOBILE false
// Display a custom comment on sondehub. Just comment it out if you do not want a custom comment
// #define PAYLOAD_COMMENT "TBTracker-RX test"
// Latidude, longitude and altitude in meters of the uploader
// Fill this in if you are not using a GPS
#define UPL_LAT 52.0000
#define UPL_LON 5.0000
#define UPL_ALT 10
// Set this to false if you don't want your postion uploaded to the sondenhub map
#define UPLOAD_YOUR_POSITION true
// Set this to false if you don't want the packet you received from the payload to upload to Sondehub
#define UPLOAD_PAYLOAD_PACKET false

/************************************************************************
* PIN NUMBERS for SX1278
*  
* Change if needed
************************************************************************************/
// Below are the settings for a TTGO T-BEAM v1. Yours might be different!
#define PIN_NSS   18 
#define PIN_DIO0  26
#define PIN_RESET 23  
#define PIN_DIO1  33

/************************************************************************
* PIN NUMBERS for optional GPS
*  
* The GPS will use the Serial2 of the ESP32.
* 
* Change the pins if needed
************************************************************************************/
// Remove or comment out the next line if your ESP32 does NOT has a GPS chip
// Leave the define when you have a GPS connected to your esp32
#define USE_GPS
// Pin numbers for the optional GPS chip.
// Older verions of the  T-Beams use 12,15
// Newer versions of the T-Beam use 12,34.
// yours might be different!
#define GPS_RX 34
#define GPS_TX 12
#define GPS_BAUD 9600
// Uncomment to print GPS Debuging info
//#define GPS_DEBUG
// GPS on a T-beam can get turned off, use this:  https://github.com/eriktheV-king/TTGO_T-beam_GPS-reset/tree/master to reset if no output

/************************************************************************
* Parameters for the optional SSD1306 OLED panel which can be mounted
* to a TTGO T-Beam or any other ESP32
*   
* Change the if needed
************************************************************************************/
// Comment these lines out if you do not use a SSD1306 panel
#define USE_SSD1306
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; Usually 0x3D or 0x3C
/***********************************************************************************
* LORA SETTINGS
*  
* Change when needed
************************************************************************************/
#define LORA_FREQUENCY  434.126      // Frequency the radio chip is listening
#define LORA_BANDWIDTH 125.0         // Do not change, change LORA_MODE instead
#define LORA_SPREADFACTOR 9          // Do not change, change LORA_MODE instead
#define LORA_CODERATE 7              // Do not change, change LORA_MODE instead
#define LORA_SYNCWORD 0x12           // for sx1278
#define LORA_POWER 10                // in dBm between 2 and 17. 10 = 10mW (recommended), currently not used
#define LORA_PREAMBLELENGTH 8
#define LORA_GAIN 0


// HAB modes
// 0 = (normal for telemetry)  Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on
// 1 = (normal for SSDV)       Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off
// 2 = (normal for repeater)   Explicit mode, Error coding 4:8, Bandwidth 62.5kHz,  SF 8, Low data rate optimize off
// 3 = (normal for fast SSDV)  Explicit mode, Error coding 4:6, Bandwidth 250kHz,   SF 7, Low data rate optimize off
// 4 = Test mode not for normal use.   - NUT SUPPORTED 
// 5 = (normal for calling mode)   Explicit mode, Error coding 4:8, Bandwidth 41.7kHz, SF 11, Low data rate optimize off
// 99 = (LoRa APRS)                Explicit mode, Error coding 4:5, Bandwidth 125KHz,  SF 12 experimental - Only receiving, no i-gating, or uploading. Requires Frequency set to 433.775

// Default tracker mode = 2
// If you set the radio to mode 99, be sure to set the frequency to 433.775, which is the standard for LoRa-APRS
#define LORA_MODE 2  // Mode 2 is usually used for simple telemetry data for pico balloons
#define LORA_MODULATION "LoRa Mode 2" // This string will be visible in Sondehub

/***********************************************************************************
* FLASH PIN settings
*  
* This pin will be set HIGH for 300ms when a new packet is received
* You can for example attach a LED to it. 
* It can be any pin you find suitable on your board
*
* Uncomment and change the pin number when you do need a flash pin
************************************************************************************/
// #define FLASH_PIN 14

/***********************************************************************************
* THIS SECTION IS NEW AS OF V0.0.11
* i2c PIN and PMU settings 
* These are used if you have a Power Management chip present.
* The PMU chips are usually built in the new T-BEAM v1.1 and v1.2
*
* These pins normally need no change
************************************************************************************/
//#define I2C_SDA         21
//#define I2C_SCL         22
// The middle button of your T-BEAM v1.1 or v1.2
// #define PMU_IRQ_BTN     38

/***********************************************************************************
* TIME SETTINGS
*  
* Change when needed
************************************************************************************/
const char* ntpServer = "pool.ntp.org";    // URL for NTP server(s). We need an accurate date and time.

/***********************************************************************************
* JSON SETTINGS
*  
* This will probably need no change. 
* These are the URL's where the JSONS are uploaded to.
************************************************************************************/
String JSON_URL = "https://api.v2.sondehub.org/amateur/telemetry";
String JSON_URL_LISTENERS = "https://api.v2.sondehub.org/amateur/listeners";
