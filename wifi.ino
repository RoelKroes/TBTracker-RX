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
    Serial.print("[WiFi} Connecting to WiFi, please wait.");
#if defined(USE_SSD1306)
    displayOled(0,0,"Connecting to WiFi...");
#endif
    

    while (WiFiMulti.run() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("[WiFi} WiFi connected to: "); Serial.println(WiFi.SSID()); 
    Serial.print("[WiFi} IP address: "); Serial.println(WiFi.localIP());
#if defined(USE_SSD1306)
    displayClear();
    displayOled(0,0,"IP: ");
    displayOled(30,0,WiFi.localIP().toString().c_str());
    displayOled(0,15,"Waiting for packets");
    displayOled(0,30,String(LoRaSettings.Frequency,3).c_str());
#endif    

}


/************************************************************************************
* Format the local time in UTC
************************************************************************************/
void formatLocalTime()
{
  
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
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
