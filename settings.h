/***********************************************************************************
* SETTINGS
* 
* It is important that you work through this file and and change the parameters
* when needed so.
*
* TBTracker-RX - Roel Kroes - roel@kroes.com
* Download latest version and instructions:  https://github.com/RoelKroes/TBTracker-RX
************************************************************************************/
//============================================================================
// Comment out the line below if you do NOT want Serial console output
// If ALLOWDEBUG is defined, the program will output data to the serial
// console for debugging and general info
// It is recommended you leave this defined when testing your tracker
//============================================================================
#define ALLOWDEBUG

// Development flag
// Set to false if you want the software to upload and store your telemetry in the sondehub database
// Set to true if you only want to upload the telemetry but not store it in the database (recommended when testing)
// If DEVFLAG is set to true, the Serial output will still let you know if an upload to Sondehub succeeded
#define DEVFLAG false

//============================================================================
// Define which radiochip you use
// Comment out the one you do not use
//============================================================================
#define USE_SX127X

//============================================================================
// 
//                **** IMPORTANT - READ THIS ****
// 
// TBTracker-RX needs an ESP32 with 2 cores. One core is used for the main
// program and the other core is used for uploading packets to Sondehub
// 
// With the latest libraries and board definitions, the program will not fit 
// anymore in the old ESP32 boards like the T-Beam with the default memory 
// partition scheme. 
// 
// To get around this issue select "HUGE APP" in the board options in the Arduino IDE
// However, some board definitions do not allow changing the partition scheme.
// In that case, select one of the ESP32 DEV modules like "ESP32 DEV Module" as 
// your board and choose the "HUGE APP" option.
// Be sure to set the correct PIN numbers for your specific board
//============================================================================
// If a pin is not connected, set it to -1
// Define the SPI pin numbers
#define SCK 5
#define MISO 19
#define MOSI 27
#define CS 18
// Define the pin numbers for the radio chip
#define PIN_NSS   18 
#define PIN_DIO0  26
#define PIN_RESET 23  
#define PIN_DIO1  -1
#define PIN_BUSY  -1

//************************************************************************
// PIN NUMBERS for optional GPS
// The GPS will use the Serial2 of the ESP32.
// Change the pins if needed
// Leave the define when you have a GPS connected to your esp32
// If you do not have a GPS connected, comment out the line: "#define USE_GPS"
//************************************************************************
#define USE_GPS
#define GPS_RX 12
#define GPS_TX 15
#define GPS_BAUD 9600

// Uncomment to print GPS Debuging info
// #define GPS_DEBUG

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
* THIS SECTION IS NEW AS OF V0.0.11
* i2c PIN and PMU settings 
* These are used if you have a Power Management chip present.
* The PMU chips are usually built in the new T-BEAM v1.1 and v1.2
************************************************************************************/
#define I2C_SDA         21
#define I2C_SCL         22
// The middle button of your T-BEAM v1.1 or v1.2 (not used currently)
// #define PMU_IRQ_BTN     38

/***********************************************************************************
* WIFI SETTINGS
*  
* you can define three SSID's with password here.
* For example: your home network, you phone's mobile hotspot and maybe another network
* You need to specify at least 1 SSID and password
* All the values are case sensitive!
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
#define UPL_ALT 14
// Set this to false if you don't want your postion uploaded to the sondenhub map
#define UPLOAD_YOUR_POSITION true
// Set this to false if you don't want the packet you received from the payload to upload to Sondehub
#define UPLOAD_PAYLOAD_PACKET true


/***********************************************************************************
* LORA SETTINGS
*  
* Change when needed
************************************************************************************/
#define LORA_FREQUENCY  432.662      // Frequency the radio chip is listening
#define LORA_BANDWIDTH 125.0         // Do not change, change LORA_MODE instead
#define LORA_SPREADFACTOR 9          // Do not change, change LORA_MODE instead
#define LORA_CODERATE 7              // Do not change, change LORA_MODE instead
#define LORA_SYNCWORD 0x12           // for sx1278
#define LORA_POWER 17                // in dBm between 2 and 17. 10 = 10mW (recommended), currently not used
#define LORA_PREAMBLELENGTH 8
#define LORA_GAIN 0
#define LORA_FREQ_OFFSET 0.0         // Frequency deviation in MHz. Will be added to the LoRa frequency. Should be a float and can be negative.

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
