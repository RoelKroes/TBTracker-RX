/***********************************************************************************
 * TBTracker-RX uses Wifi to send JSON data to Sondehub
 * 
 * You can add multiple WiFi networks here.
 * TBTracker-RX will choose the strongest one
 ***********************************************************************************/
#include "time.h"
#include <WiFiMulti.h>

WiFiMulti WiFiMulti;

/************************************************************************************
* WIFI setup
************************************************************************************/
void setupWifi()
{
    toSerialConsole(F("\n\n"));

#if defined(USE_SSD1306)
    displayClear();
    displayOled(0,0,"Searching for WiFi...");
#endif

    // Set WiFi in Station mode
    WiFi.mode(WIFI_STA);

    // Add the WiFi networks as defined in the settings file
    if (WIFI_SSID_1 != "") WiFiMulti.addAP(WIFI_SSID_1, WIFI_PASSWORD_1);
    if (WIFI_SSID_2 != "") WiFiMulti.addAP(WIFI_SSID_2, WIFI_PASSWORD_2);
    if (WIFI_SSID_3 != "") WiFiMulti.addAP(WIFI_SSID_3, WIFI_PASSWORD_3);

    // WiFi network Scan
    toSerialConsole(F("[WiFi] Scanning WiFi networks..."));
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    toSerialConsole(F("scan done.\n"));
     if (n == 0) 
     {
      toSerialConsole(F("no networks found\n"));
     } 
     else 
     {
      toSerialConsole(n); toSerialConsole(F(" networks found\n"));
      for (int i = 0; i < n; ++i) 
      {
        // Print SSID and RSSI for each network found
        toSerialConsole(i + 1);
        toSerialConsole(": ");
        toSerialConsole(WiFi.SSID(i));
        toSerialConsole(" (");
        toSerialConsole(WiFi.RSSI(i));
        toSerialConsole(")");
        toSerialConsole((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" \n":"*\n");
        delay(10);
      }
    }

    // Try to connect to the stongest know access point
    toSerialConsole(F("[WiFi} Connecting to WiFi, please wait."));
#if defined(USE_SSD1306)
    displayClear();
    displayOled(0,0,"Connecting to WiFi...");
#endif
    int loopCounter = 0;
    while (WiFiMulti.run() != WL_CONNECTED && loopCounter < 25) 
    {
      delay(500);
      toSerialConsole(F("."));
      loopCounter++;
    }
    
    if (WiFi.status() == WL_CONNECTED)
    {
       toSerialConsole(F("\n"));
       toSerialConsole(F("[WiFi} WiFi connected to: ")); toSerialConsole(WiFi.SSID()); toSerialConsole(F("\n"));
       toSerialConsole(F("[WiFi} IP address: ")); toSerialConsole(WiFi.localIP().toString()); toSerialConsole(F("\n"));
       WiFi.setAutoReconnect(true);
       WiFi.persistent(true);
    }
    else
    {
      toSerialConsole(F("\nCould not connect to WiFi. No uploading possible."));
    }
}


/************************************************************************************
* Format the local time in UTC
************************************************************************************/
void formatLocalTime()
{
  
  if(!getLocalTime(&timeinfo))
  {
    toSerialConsole(F("Failed to obtain time\n"));
    return;
  }
  strftime(Telemetry.time_received,sizeof(Telemetry.time_received),"%Y-%m-%dT%T.000000Z",&timeinfo);
}


/************************************************************************************
* Get time information from the NTP server
************************************************************************************/
void updateTime()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    //init and get the time. Use UTC
   configTime(0, 0, ntpServer);
  }
}
