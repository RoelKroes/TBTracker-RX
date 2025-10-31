/************************************************************************************
* JSON related stuff
************************************************************************************/

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "WiFi.h"

/************************************************************************************
* JSON structure setup for sondehub. 
*
*  Example:
* {
*  "software_name": "ttnhabbridge", # Receiving software name
*  "software_version": "0.0.1", # Receiving software version
*  "uploader_callsign": "foobar",                 # Mandatory - TTN station name?
*  "uploader_position": [ -34.0, 138.0, 0 ],  # Optional - TTN station location, if available
*  "uploader_radio": "???",  # Optional - Any other details 
*  "uploader_antenna": "???", # Optional - other rx details
*  "snr": 11.79, # Optional - Receiver metadata - SNR
*  "frequency": 434.201003, # Optional - Receiver Metadata - RX Frequency
*  "modulation": "LoRaWAN - TTNv3", # Optional, but recommended - Modulation type
*  "time_received": "2022-04-18T04:36:59.899304Z", # Time the packet was received on the TTN network
*  "datetime": "2022-04-18T04:36:58.000000Z", # Date/time reported by the payload itself. Use todays UTC date if no date available.
*  "payload_callsign": "CALLSIGN_HERE", # Callsign of the payload
*  "frame": 6, # Optional - Frame number 
*  "lat": -34.1, # Mandatory - Position
*  "lon": 138.1,
*  "alt": 100.0,
*  "temp": 30, # Some examples of optional fields 
*  "sats": 0,
*  "batt": 3.15,
* }
*
*
* Station info JSON
*{
  "software_name": "string",
  "software_version": "string",
  "uploader_callsign": "string",
  "uploader_position": [
    0,
    0,
    0
  ],
  "uploader_antenna": "string",
  "uploader_contact_email": "string",
  "mobile": true
}
*
************************************************************************************/

/************************************************************************************
* Upload your current position to the Sondehub server
************************************************************************************/
void postStationToServer()
{
  
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient https;   
#if defined(USE_GPS)    
    processGPSData();
#endif

    https.begin(JSON_URL_LISTENERS);  
    https.addHeader("Content-Type", "application/json");         
    https.addHeader("accept", "text/plain");

    DynamicJsonDocument doc(350); 
    // Add values in the document
    doc["software_name"] = "TBTracker-RX";
    doc["software_version"] = TBTRACKER_VERSION;
    doc["uploader_callsign"] = CALLSIGN;
    doc["uploader_antenna"] = ANTENNA_USED;
    doc["uploader_radio"] = RADIO_USED;
    doc["uploader_contact_email"] = UPLOADER_EMAIL;
    doc["mobile"] = I_AM_MOBILE; 
    JsonArray uploader_position = doc.createNestedArray("uploader_position");
    // Add the uploader position to the JSON by populating the nested array
    uploader_position.add(Telemetry.uploader_position[0]);
    uploader_position.add(Telemetry.uploader_position[1]);
    uploader_position.add(Telemetry.uploader_position[2]);
 
    String json;
    serializeJson(doc, json);

    // Use the lines below for debugging. Be sure that the total length of the JSON
    // doen NOT exceed 350
    // Serial.println(); Serial.print("JSON length: "); Serial.println(json.length());
    // Serial.println(json);
    
    Serial.println();     
    Serial.println();
    Serial.println(F("Uploading your position to Sondehub:"));
    int httpResponseCode = https.PUT(json);
    
    // Display the result of the JSON upload 
    if(httpResponseCode>0)
    {   
      String response = https.getString();                       
      Serial.print(httpResponseCode); Serial.print(F(" - ")); Serial.println(response); 
    }
    else 
    { 
      Serial.printf("Error code: %d\n",httpResponseCode);
      Serial.printf("Error occurred while sending HTTP POST: %s\n", https.errorToString(httpResponseCode).c_str());  
    }

    https.end();
  }

}


/************************************************************************************
// Get SSDV records from the upload queue and post the SSDV data to the SondeHub server
************************************************************************************/
void postSSDVToServer()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    char packetBuf[256]; // Contains the hex encoded packet
    char base64_data[512];
    size_t base64_length;

    // Get a SSDV packet from the queue
    if( xQueueReceive( ssdv_Queue,
                       packetBuf,
                      ( TickType_t ) 1) == pdPASS )
    {
      HTTPClient https;   
      // packetBuf now contains a copy of the first item in the queue
      // Add the http headers
      https.begin("http://ssdv.habhub.org/api/v0/packets");  
      https.addHeader("Content-Type", "application/json");  
      https.addHeader("Accept", "application/json");
      https.addHeader("charsets", "utf-8");
    
      // code the packet into base64
      base64_encode(packetBuf, 256, &base64_length, base64_data);
      base64_data[base64_length] = '\0';

      DynamicJsonDocument doc(1024);
      // Add values in the document
      doc["type"] = "packet";
      doc["packet"] = base64_data;
      doc["encoding"] = "base64";
      doc["received"] = "2023-03-17";
      doc["receiver"] = CALLSIGN;
    
      String json;
      serializeJson(doc, json);
      Serial.println();
      Serial.print(F("JSON length SSDV: ")); Serial.println(json.length());
      
      int httpResponseCode = https.POST(json);
    
      // Print the results to the Serial console
      if(httpResponseCode <= 0)
      {
        Serial.printf("Error code: %d\n",httpResponseCode);
        Serial.printf("Error occurred while sending HTTP POST: %s\n", https.errorToString(httpResponseCode).c_str());  
      }
      // cleanup
      https.end();
    }
  }
}


/************************************************************************************
// Post the telemetry data to the SondeHub upload queue 
************************************************************************************/
void putTelemetryinQueue() 
{     
    DynamicJsonDocument doc(1024);
    // Add values in the document
    
    // Add the "dev" flag to the JSON if you only want to test the validity of the
    if (devflag) doc["dev"] = "true";
    doc["software_name"] = "TBTracker-RX";
    doc["software_version"] = TBTRACKER_VERSION;
    doc["uploader_callsign"] = CALLSIGN;
    doc["time_received"] = Telemetry.time_received;
    doc["payload_callsign"] = Telemetry.payload_callsign;
    doc["datetime"] = Telemetry.datetime;
    doc["lat"] =  Telemetry.lat;
    doc["lon"] = Telemetry.lon;
    doc["alt"] = Telemetry.alt;
    doc["frequency"] = Telemetry.frequency;
    doc["rssi"] = Telemetry.rssi;
    doc["snr"] = Telemetry.snr;
    doc["modulation"] = LoRaSettings.ModeString;
    doc["raw"] = Telemetry.raw;
    doc["uploader_antenna"] = ANTENNA_USED;
    doc["uploader_radio"] = RADIO_USED;

    if (UPLOAD_YOUR_POSITION)
    {
      // Add the uploader position to the JSON
      JsonArray uploader_position = doc.createNestedArray("uploader_position");
      uploader_position.add(Telemetry.uploader_position[0]);
      uploader_position.add(Telemetry.uploader_position[1]);
      uploader_position.add(Telemetry.uploader_position[2]);
    }

    // Add non standard data from the payload to the JSON
    if (Telemetry.extraFields)
    {
        // We need to check for 6, 8, 9, A, B, R, S
        if (Telemetry.lastField.indexOf("6") >= 0) doc["sats"] = Telemetry.sats;
        if (Telemetry.lastField.indexOf("8") >= 0) doc["heading"] = Telemetry.heading;
        if (Telemetry.lastField.indexOf("9") >= 0) doc["batt"] = Telemetry.batt;
        if ( (Telemetry.lastField.indexOf("A") >= 0) || (Telemetry.lastField.indexOf("B") >= 0) ) doc["temp"] = Telemetry.temp;
        if (Telemetry.lastField.indexOf("R") >= 0) doc["pressure"] = Telemetry.pressure;
        if (Telemetry.lastField.indexOf("S") >= 0) doc["ext_humidity"] = Telemetry.humidity;
#if defined(PAYLOAD_COMMENT)    
       doc["comment"] = Telemetry.comment;
#endif
    }
     
    String json;
    serializeJson(doc, json);
    json = "[" + json + "]";
   
    // JSON is ready here
    // Put it in the Telemetry queue
    char jbuf[1024];
    json.toCharArray(jbuf,json.length()+1);
    // Add the packet to the queue. do not wait if thge queue is full
    if (telemetry_Queue != NULL)
    {
       if (xQueueSend(telemetry_Queue, jbuf, 0) == pdPASS)
       { 
         Telemetry.uploadResult = "Telemetry packet added to upload queue.";
       }
       else
       {
         Telemetry.uploadResult = "Could not upload telemetry. Queue is full.";
       }
    }
}


/************************************************************************************
// Retrieve a record from the telemetry queue and upload to Sondehub
************************************************************************************/
void postTelemetryToServer() 
{
 
  if (WiFi.status() == WL_CONNECTED)
  {     
    String json;

    // Get a record from the telemetry queue
    char jbuf[1024];
    // Get a SSDV packet from the queue
    if( xQueueReceive( telemetry_Queue,
                       jbuf,
                      ( TickType_t ) 1) == pdPASS )
    {
      HTTPClient https;   
     
      https.begin(JSON_URL);  
      https.addHeader("Content-Type", "application/json");         
      https.addHeader("accept", "text/plain");
      
      json = jbuf;
      Serial.println();
      Serial.print(F("JSON length: ")); Serial.println(json.length());

      int httpResponseCode = https.PUT(json);
    
      // Print the results to the Serial console
      if(httpResponseCode>0)
      {
       
        String response = https.getString();                       
        Serial.print("\nUpload result: ");
        Serial.print(httpResponseCode); Serial.print(" - ");
        Serial.println(response); 
        // Telemetry.uploadResult =  response;
      }
      else 
      {   
        Serial.printf("Error code: %d\n",httpResponseCode);
        Serial.printf("Error occurred while sending HTTP POST: %s\n", https.errorToString(httpResponseCode).c_str());  
        // Telemetry.uploadResult = https.errorToString(httpResponseCode);
      }
      https.end();
    }
  }
}





