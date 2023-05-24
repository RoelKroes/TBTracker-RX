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
    Serial.println(); Serial.println();

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
    Serial.print("[WiFi] Scanning WiFi networks...");
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done.");
     if (n == 0) 
     {
      Serial.println("no networks found");
     } 
     else 
     {
      Serial.print(n); Serial.println(" networks found");
      for (int i = 0; i < n; ++i) 
      {
        // Print SSID and RSSI for each network found
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
        delay(10);
      }
    }

    // Try to connect to the stongest know access point
    Serial.print(F("[WiFi} Connecting to WiFi, please wait."));
#if defined(USE_SSD1306)
    displayClear();
    displayOled(0,0,"Connecting to WiFi...");
#endif
    int loopCounter = 0;
    while (WiFiMulti.run() != WL_CONNECTED && loopCounter < 25) 
    {
      delay(500);
      Serial.print(".");
      loopCounter++;
    }
    
    if (WiFi.status() == WL_CONNECTED)
    {
       Serial.println("");
       Serial.print(F("[WiFi} WiFi connected to: ")); Serial.println(WiFi.SSID()); 
       Serial.print(F("[WiFi} IP address: ")); Serial.println(WiFi.localIP());
       WiFi.setAutoReconnect(true);
       WiFi.persistent(true);
    }
    else
    {
      Serial.println();
      Serial.println(F("Could not connect to WiFi. No uploading possible."));
    }
}


/************************************************************************************
* Format the local time in UTC
************************************************************************************/
void formatLocalTime()
{
  
  if(!getLocalTime(&timeinfo))
  {
    Serial.println(F("Failed to obtain time"));
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
