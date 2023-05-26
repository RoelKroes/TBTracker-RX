/************************************************************************************
* Telemetry parser related stuff.
************************************************************************************/

/************************************************************************************
 *   Get the last field from the RAW packet and determine if it contains metadata
 *************************************************************************************/
void getMetafromRaw(char *Buf)
{
   char Str[PACKETLEN];
   char *token; 
   int counter = 0;
   const char *delimiter = ",";
   String lastField;

   // Create a copy of the original string received from the radio
   memcpy(Str,Buf,PACKETLEN) ;

   // Get the CRC, which is the string after the asterisk
   char *dataStr = strtok(Str,"*");
   char *crcStr = strtok(NULL,"*"); 
    
   // Remove the prefix of $ characters
   while (dataStr[0] == '$')
     dataStr = &dataStr[1];

   // dataStr now contains all fields in a string separated by the delimiter  
   // Parse all the fields and remember the last one
    token = strtok(dataStr, delimiter);
    while (token != NULL)
    { 
      if (token != NULL)
      {
        lastField = token;
      }
      token = strtok(NULL, delimiter);
    }

   if (lastField.startsWith("012345"))
   {
      // There is metadata
      Telemetry.lastField = lastField;
      Telemetry.extraFields = true;
      Telemetry.lastField.toUpperCase();
   }
   else
   {
      Telemetry.extraFields = false;
   }

   if (Telemetry.extraFields)
   {
     Serial.print("Meta data detected:\t");
     Serial.println(Telemetry.lastField);
   }
}


/***********************************************************************************
* SONDEHUB EXTRA FIELDS SETTINGS
*  
* For displaying extra fields at sondehub, we need to define which fields are
* in the telemetry after the lat, lon, alt fields
* This can be done by adding a specific string after the last telemetry field
* This is supported by the various receivers made by Dave Akerman, and we just copy it.
* See: https://www.daveakerman.com/?page_id=2410
* 
* 0  PayloadID
* 1 Counter
* 2 Time
* 3 Latitude
* 4 Longitude
* 5 Altitude
* 6 Satellites
* 7 Speed
* 8 Heading
* 9 Battery Voltage
* A InternalTemperature
* B ExternalTemperature
* C PredictedLatitude
* D PredictedLongitude
* E CutdownStatus
* F LastPacketSNR
* G LastPacketRSSI
* H ReceivedCommandCount
* I-N ExtraFields
* O MaximumAltitude
* P Battery Current
* Q External Temperature 2
* R Pressure
* S Humidity
* T CDA
* U Predicted Landing Speed
* V Time Till Landing
* W Last Command Received
* 

* Parse the RAW data received by the radio
************************************************************************************/
void parseRawData(char *Buf)
{
    char Str[PACKETLEN];
    char *token; 
    int counter = 0;
    const char *delimiter = ",";

   // Create a copy of the original string received from the radio
   memcpy(Str,Buf,PACKETLEN) ;
        
    // Get the CRC, which is the string after the asterisk
    char *dataStr = strtok(Str,"*");
    Telemetry.raw = dataStr;
    char *crcStr = strtok(NULL,"*");
    
    // Remove the prefix of $ characters
    while (dataStr[0] == '$')
     dataStr = &dataStr[1];


    // 1. Check the internal checksum
    if (!CheckCRC(dataStr,crcStr))
       return;   // Error

    // 2. parse the fields
    // The software assumes that some data files are in the following fixed order:
    // 0. payload_callsign
    // 1. frame number
    // 2. Time
    // 3. Latitude
    // 4. Longitude
    // 5. Altitude
    
        
    // Parse all the fields
    token = strtok(dataStr, delimiter);
    while (token != NULL)
    {
       if (counter == 0)  // payload_callsign
       {
         Telemetry.payload_callsign = token;
         Serial.println(); Serial.print(F("Payload callsign:\t")); Serial.println(Telemetry.payload_callsign);
       }

       if (counter == 1)  // frame
       {
         Telemetry.frame = atol(token);
         Serial.print(F("Payload frame:\t\t")); Serial.println(Telemetry.frame);
       }

       if (counter == 2) // Time
       {
        strftime(Telemetry.datetime,sizeof(Telemetry.datetime),"%Y-%m-%dT",&timeinfo);
        strcat(Telemetry.datetime,token); strcat(Telemetry.datetime,".000000Z");
        Serial.print(F("Payload time:\t\t")); Serial.println(Telemetry.datetime);
       }

       if (counter == 3) // Latitude
       {
         Telemetry.lat = atof(token); 
         Serial.print(F("Latitude:\t\t")); Serial.println(Telemetry.lat,5);
       }
       
       if (counter == 4) // Longitude
       {
         Telemetry.lon = atof(token); 
         Serial.print(F("Longitude:\t\t")); Serial.println(Telemetry.lon,5);
       }

       if (counter == 5) // Altitude
       {
         Telemetry.alt = atof(token); 
         Serial.print(F("Altitude:\t\t")); Serial.println(Telemetry.alt,0);
       }

       // All fields below here are considered metaData are and only sent when defined in the payloaddata
      if ((Telemetry.extraFields) && (counter > 5) && (counter < Telemetry.lastField.length()))
      {

        switch (Telemetry.lastField[counter])
        {
          case '6': Telemetry.sats = atoi(token);
                    Serial.print(F("Satellites:\t\t"));
                    Serial.println(Telemetry.sats);
                    break;
          case '8': Telemetry.heading = atof(token);
                    Serial.print(F("Heading:\t\t"));
                    Serial.println(Telemetry.heading,0);
                    break;
          case '9': Telemetry.batt = atof(token);
                    Serial.print(F("Battery voltage:\t"));
                    Serial.println(Telemetry.batt,2);
                    break;
          case 'A': 
          case 'B': Telemetry.temp = atoi(token);
                    Serial.print(F("Temperature:\t\t"));
                    Serial.println(Telemetry.temp);
                    break;
          case 'R': Telemetry.pressure = atof(token);
                    Serial.print(F("Pressure (hPa):\t\t"));
                    Serial.println(Telemetry.pressure,1);
                    break;                    
         case 'S': Telemetry.humidity = atof(token);
                    Serial.print(F("Humidity (%):\t\t"));
                    Serial.println(Telemetry.humidity,0);
                    break;                    
          default:  Serial.print(F("Extra field: "));
                    Serial.print(token);
                    Serial.print(" ");
                    Serial.println(Telemetry.lastField[counter]);           
                    break;
        }
      }
      token=strtok(NULL, delimiter);
      counter++;
    }

#if defined(PAYLOAD_COMMENT)    
    Telemetry.comment = PAYLOAD_COMMENT;
    Serial.print(F("Comment:\t\t"));
    Serial.println(Telemetry.comment);
#endif

    // 3. Parse the GPS data or use the position from the Settings file
    Serial.println();
    setUploaderPosition();
    setDistanceAndBearing();

    // 4. Update the OLED display 
#if defined(USE_SSD1306)
    // flash the screen
    // displayFlash();  // Moved to receiveLoRa so we do it for every packet type
    //displayUpdate();  // Replaced with timed update trigered by "oledUpdateNeeded"
    oledUpdateNeeded = true;
#endif       

   // 5. Flash the flash pin if defined
#if defined(FLASH_PIN)
   // flashPin();  // Moved to receiveLoRa so we do it for every packet type
#endif  

   // 6. Upload to the Sondehub upload queue
   if (Telemetry.uploadSondehub) putTelemetryinQueue();    

   // 7. Close the packet with some dashes
   closePacket();
}


/************************************************************************************
* Create a visual end-of-packet on the Serial console
************************************************************************************/
void closePacket()
{
  Serial.println(F("----------------------------"));  
  Serial.println();
  Serial.println();
}

/************************************************************************************
* Determine your position
************************************************************************************/
void setUploaderPosition()
{
#if defined(USE_GPS)    
    processGPSData();

    Serial.println(F("Your position from GPS:"));
    Serial.print(F("Your GPS latitude:\t")); Serial.println(Telemetry.uploader_position[0],5);
    Serial.print(F("Your GPS longitude:\t")); Serial.println(Telemetry.uploader_position[1],5);
    Serial.print(F("Your GPS Altitude:\t")); Serial.println(Telemetry.uploader_position[2],0);
#else
    Telemetry.uploader_position[0] = UPL_LAT;
    Telemetry.uploader_position[1] = UPL_LON;
    Telemetry.uploader_position[2] = UPL_ALT;
    Serial.println(F("Your position from settings:"));
    Serial.print(F("Your latitude:\t")); Serial.println(Telemetry.uploader_position[0],5);
    Serial.print(F("Your longitude:\t")); Serial.println(Telemetry.uploader_position[1],5);
    Serial.print(F("Your Altitude:\t")); Serial.println(Telemetry.uploader_position[2],0);
#endif
}

/************************************************************************************
* Determine the distance and bearing from your position to the tracker
************************************************************************************/
void setDistanceAndBearing()
{
   // Show the distance between the receiver and the balloon
   Telemetry.distance = GPSDistance(Telemetry.uploader_position[0],Telemetry.uploader_position[1],Telemetry.lat,Telemetry.lon);
   Serial.print(F("Distance (km):\t\t")); Serial.println(Telemetry.distance,2);

   // Show the bearing between the receiver and the balloon
   Telemetry.bearing = GPSBearing(Telemetry.uploader_position[0],Telemetry.uploader_position[1],Telemetry.lat,Telemetry.lon);

   // Get the bearing in compass direction
   Telemetry.compass = degToCompass(Telemetry.bearing);
   Serial.print(F("Bearing (degr.):\t")); Serial.print(Telemetry.bearing,0); Serial.print(" - travel "); Serial.print(Telemetry.compass); Serial.println(" to chase");
}

/************************************************************************************
* Check the CRC from the received data
************************************************************************************/
bool CheckCRC(char *dataStr, char *crcStr)
{
   unsigned int CRC;
   int i, j, Count;
   char crcCalculated[6];

   // Calculate the CRC from the received rawData
   CRC = 0xffff;           // Seed
   for (i=0; dataStr[i] != '\0'; i++)
   {
     CRC ^= (((unsigned int)dataStr[i]) << 8);
      for (j=0; j<8; j++)
      {
          if (CRC & 0x8000)
              CRC = (CRC << 1) ^ 0x1021;
          else
              CRC <<= 1;
      }
   }

   Count = 0;
   crcCalculated[Count++] = Hex((CRC >> 12) & 15);
   crcCalculated[Count++] = Hex((CRC >> 8) & 15);
   crcCalculated[Count++] = Hex((CRC >> 4) & 15);
   crcCalculated[Count++] = Hex(CRC & 15);
   crcCalculated[Count++] = '\0';
    
   Serial.print("Telemetry:\t\t"); Serial.println(dataStr);
   Serial.print("CRC received:\t\t"); Serial.println(crcStr);
   Serial.print("CRC calculated:\t\t"); Serial.print(crcCalculated);

   if ((crcCalculated[0] == crcStr[0]) && (crcCalculated[1] == crcStr[1]) && (crcCalculated[2] == crcStr[2]) && (crcCalculated[3] == crcStr[3]) )
   {
     Serial.println(" << CRC OK");
     return true;
   }
   else
   {
      Serial.println(F(" << CRC NOT CORRECT!"));
     return false;
   }
}


/************************************************************************************
* Lookup table for HEX values, used in the CRC calculation
************************************************************************************/
char Hex(char Character)
{
  char HexTable[] = "0123456789ABCDEF";
  
  return HexTable[Character];
}
