/************************************************************************************
* TBTracker-RX - roel@kroes.com
* 
* A mobile software platform for receiving LoRa transmissions and uploading those 
* to amateur.sondehub.org. The software is designed to run on the esp32 platform. 
* A TTGO T-Beam would be ideal. It has WiFi connectivity and a simple web interface
* 
* First adjust the settings in the settings file.
* 
* Be sure you run the latest version of the Arduino IDE.
*
* v0.0.7
* 03-02-2023: The link to Sondehub in the web interface now opens in a new window
* 03-02-2023: Software now works also without WiFi (data on Serial output or OLED display)
*
* v0.0.6
* 29-JAN-2023: Added a parser for the APRS packets to display on the Serial interface, webinterface and SSD1306 display
*
* v0.0.5
* 21-JAN-2023: Print length of received packet in the Serial monitor
* 21-JAN-2023: Check if the received packet is indeed a HAB telemetry packet
* 21-JAN-2023: Removed the RAW telemetry string from the Serial monitor to avoid double info and unreadable characters
* 21-JAN-2023: Added support for LoRa-APRS packets
* 21-JAN-2023: Added a packet Log trail in the web interface
*  
* v0.0.4
* 14-JAN-2023: Added support for LoRa Mode 5 (Explicit mode, Error coding 4:8, Bandwidth 41.7kHz, SF 11, Low data rate optimize off)
* 14-JAN-2023: Added support for LoRa Mode 3 (Explicit mode, Error coding 4:6, Bandwidth 250kHz,   SF 7, Low data rate optimize off)
* 20-JAN-2023: Added support for LoRa Mode 0 (Explicit mode, Error coding 4:8, Bandwidth 20.8kHz, SF 11, Low data rate optimize on)
* 20-JAN-2023: Added support for LoRa Mode 1 (Implicit mode, Error coding 4:5, Bandwidth 20.8kHz,  SF 6, Low data rate optimize off)
* 20-JAN-2023: Added support for showing and changing the LoRa Mode in the webinterface
* 20-JAN-2023: Solved several bugs
* 20-JAN-2023: Added autotune to the radio (based on the frquency error calculated by the radio)
*
* v0.0.3
* 06-JAN-2023: Added SNR, RSSI to the web interface
* 06-JAN-2023: Added time since latest packet to the web interface
* 06-JAN-2023: Made the Google Maps open in a new window
* 06-JAN-2023: Solved several reported bugs / unexpected behaviour
* 07-JAN-2023: Changed some UI language. (I apologize, English is not my native language)
*
* v0.0.2
* 23-DEC-2022: Added upload result to the web interface
* 23-DEC-2022: "comment" string is now disabled by default
* 23-DEC-2022: Try to determine if the packet received is an actual HAB packet or an invalid or unknown packet
* 23-DEC-2022: Changed the wording of "Test mode" in the webinterface
*
* v0.0.1: 
* 19-DEC-2022: Changed to x.y.z version numbering
* 19-DEC-2022: Moved version number to TBTracker-rx.ino from settings.h
* 19-DEV-2022: The OLED display will now show frequency updates
* 19-DEC-2022: Added a way to change the DEVFLAG in the webinterface 
*
* v0.1ß:
* 18-DEC-2022: Initial version, released in the Facebook HAB-NL group
************************************************************************************/
#include <RadioLib.h>
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>
#include "settings.h"

// TBTracker-RX version number
#define TBTRACKER_VERSION "V0.0.7"

// Struct to hold LoRA settings
struct TLoRaSettings
{
  uint8_t LoRaMode = LORA_MODE;
  float Frequency = LORA_FREQUENCY;
  float Bandwidth = LORA_BANDWIDTH;
  uint8_t SpreadFactor = LORA_SPREADFACTOR;
  uint8_t CodeRate = LORA_CODERATE;
  uint8_t SyncWord = LORA_SYNCWORD;
  uint8_t Power = LORA_POWER;
  uint16_t PreambleLength =  LORA_PREAMBLELENGTH;
  uint8_t Gain = LORA_GAIN;
} LoRaSettings;

// Struct to hold Time information
struct tm timeinfo;

// Keeps track of uploading your position to Sondehub
bool uploader_position_sent = false;

// Just a variable to calculate simple time difference
unsigned long timeCounter = 0;

// global counter for the number of valid packets we receive
unsigned long packetCounter = 0;

// Holder for the dev flag. If dev flag is true than data sent to Sondehub is not added to the database. Can be set in settings.h
bool devflag;

/************************************************************************************
* Struct and variable which contains the latest telemetry
************************************************************************************/
struct TTelemetry
{
  String raw;                   // Raw received telemetry
  float  uploader_position[3];  // position of yourself in GPS coordinates and altitude in meters [ -34.0, 138.0, 0 ]
  float snr;                    // Receiver metadata - SNR
  float rssi;                   // Receiver metadata - RSSI
  float frequency;              // Receiver Metadata - RX Frequency
  float frequency_error;        // Measured by Radio. Based on this value the radio will be retuned. 
  size_t rxPacketLen=0;         // Length of received packet
  String modulation;            // Modulation type
  unsigned long atmillis=0;     // Reported millis when packet was received by the radio
  char time_received[30];       // Date/Time the packet was received on the network (example: "2022-04-18T04:36:59.899304Z")
  char datetime[30];            // Date/time reported by the payload itself. Use todays UTC date if no date available. (example: "2022-04-18T04:36:58.000000Z")
  String payload_callsign;      // Callsign of the payload
  long  frame;                  // Optional - Frame number as reported by the payload
  float lat;                    // Position latitude as reported by the payload
  float lon;                    // Position longitude as reported by the payload
  float alt;                    // Altitude in meters as reported by the payload
  unsigned int sats;            // Number of satellites as reported by the payload
  float temp;                   // Measured temperature by the payload
  float batt;                   // Battery voltage measured by the payload
  float heading;                // compass heading as reported by payload
  float pressure;               // Pressure (hPa)
  float humidity;               // humidity (%) 
  float distance;               // Distance in km to payload
  float bearing;                // Bearing to payload
  String comment;               // Optional comment for upload to Sondehub
  String compass;               // Compass direction in terms of "N", "SW", ...
  String lastField;             // Contains info about what data is in the fields after the location and altitude
  bool extraFields;             // is true when there are custom fields at the end of the payload data
  bool uploadSondehub;          // is true when the telemetry should be uploaded to Sondehub
  String uploadResult;          // holds the latest upload result to Sondehub
} Telemetry;

/************************************************************************************
* Setting up all parts of the program
************************************************************************************/
void setup() 
{
  //disable brownout
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 

  Serial.begin(57600);
  
  devflag = DEVFLAG;
  if (devflag)
  {
     Serial.println(F("SOFTWARE IS IN DEVELOPMENT MODE, Data will not be shown on Sondehub. Change DEVFLAG in settings.h"));
  }
  
#if defined(USE_SSD1306)
  // Setup the SSD1306 display if there is any
  setupSSD1306();
#endif
  setupLoRa();
  setupWifi();
  updateTime();
  setupWebserver();
  // Sync the time keeper
  timeCounter = millis();

#if defined(USE_SSD1306)
    // Init the OLED display
    updateOLEDforFrequency();
#endif  

  // When there is no valid GPS postion, we will take the GPS coordinates from the settings file
  Telemetry.uploader_position[0] = UPL_LAT;
  Telemetry.uploader_position[1] = UPL_LON;
  Telemetry.uploader_position[2] = UPL_ALT;
  
  // Upload telemetry?
  if (UPLOAD_PAYLOAD_PACKET)
    Telemetry.uploadSondehub = true;
  else
    Telemetry.uploadSondehub = false;

#if defined(USE_GPS)
  // Setup the GPS if there is any 
  Serial2.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);   
#endif

}


/************************************************************************************
* As most events are interrupt driven, there is only a small loop
************************************************************************************/
void loop() 
{
  // Process received LoRa packets
  receiveLoRa();
  
#if defined(USE_GPS)  
  // Poll the GPS
  smartDelay(700);
#endif  

  // Keep track of the time for re-uploading your position
  if (millis()-timeCounter > 600000ul) 
  {
    uploader_position_sent = false;
    timeCounter = millis();
  }
  
  // Send your position to sondehub if enabled
  if (UPLOAD_YOUR_POSITION && !uploader_position_sent)
  {
    postStationToServer();
    uploader_position_sent = true;
  }

}
