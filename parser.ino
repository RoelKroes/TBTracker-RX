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
     toSerialConsole(F("Meta data detected:\t"));
     toSerialConsole(Telemetry.lastField);
     toSerialConsole(F("\n"));
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
         toSerialConsole(F("\nPayload callsign:\t")); toSerialConsole(Telemetry.payload_callsign);
         toSerialConsole(F("\n"));
       }

       if (counter == 1)  // frame
       {
         Telemetry.frame = atol(token);
         toSerialConsole(F("Payload frame:\t\t")); toSerialConsole(Telemetry.frame);
         toSerialConsole(F("\n"));
       }

       if (counter == 2) // Time
       {
        strftime(Telemetry.datetime,sizeof(Telemetry.datetime),"%Y-%m-%dT",&timeinfo);
        strcat(Telemetry.datetime,token); strcat(Telemetry.datetime,".000000Z");
        toSerialConsole(F("Payload time:\t\t")); toSerialConsole(Telemetry.datetime);
        toSerialConsole(F("\n"));
       }

       if (counter == 3) // Latitude
       {
         Telemetry.lat = atof(token); 
         toSerialConsole(F("Latitude:\t\t")); toSerialConsole(Telemetry.lat,5);
         toSerialConsole(F("\n"));
       }
       
       if (counter == 4) // Longitude
       {
         Telemetry.lon = atof(token); 
         toSerialConsole(F("Longitude:\t\t")); toSerialConsole(Telemetry.lon,5);
         toSerialConsole(F("\n"));
       }

       if (counter == 5) // Altitude
       {
         Telemetry.alt = atof(token); 
         toSerialConsole(F("Altitude:\t\t")); toSerialConsole(Telemetry.alt,0);
         toSerialConsole(F("\n"));
       }

       // All fields below here are considered metaData are and only sent when defined in the payloaddata
      if ((Telemetry.extraFields) && (counter > 5) && (counter < Telemetry.lastField.length()))
      {

        switch (Telemetry.lastField[counter])
        {
          case '6': Telemetry.sats = atoi(token);
                    toSerialConsole(F("Satellites:\t\t"));
                    toSerialConsole(Telemetry.sats);
                    toSerialConsole(F("\n"));
                    break;
          case '8': Telemetry.heading = atof(token);
                    toSerialConsole(F("Heading:\t\t"));
                    toSerialConsole(Telemetry.heading,0);
                    toSerialConsole(F("\n"));
                    break;
          case '9': Telemetry.batt = atof(token);
                    toSerialConsole(F("Battery voltage:\t"));
                    toSerialConsole(Telemetry.batt,2);
                    toSerialConsole(F("\n"));
                    break;
          case 'A': 
          case 'B': Telemetry.temp = atoi(token);
                    toSerialConsole(F("Temperature:\t\t"));
                    toSerialConsole(Telemetry.temp);
                    toSerialConsole(F("\n"));
                    break;
          case 'R': Telemetry.pressure = atof(token);
                    toSerialConsole(F("Pressure (hPa):\t\t"));
                    toSerialConsole(Telemetry.pressure,1);
                    toSerialConsole(F("\n"));
                    break;                    
         case 'S': Telemetry.humidity = atof(token);
                    toSerialConsole(F("Humidity (%):\t\t"));
                    toSerialConsole(Telemetry.humidity,0);
                    toSerialConsole(F("\n"));
                    break;                    
          default:  toSerialConsole(F("Extra field: "));
                    toSerialConsole(token);
                    toSerialConsole(F(" "));
                    toSerialConsole(Telemetry.lastField[counter]); 
                    toSerialConsole(F("\n"));
                    break;
        }
      }
      token=strtok(NULL, delimiter);
      counter++;
    }

#if defined(PAYLOAD_COMMENT)    
    Telemetry.comment = PAYLOAD_COMMENT;
    toSerialConsole(F("Comment:\t\t"));
    toSerialConsole(Telemetry.comment);
    toSerialConsole(F("\n"));
#endif

    // 3. Parse the GPS data or use the position from the Settings file
    toSerialConsole(F("\n"));
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
  toSerialConsole("----------------------------\n\n\n");  
}

/************************************************************************************
* Determine your position
************************************************************************************/
void setUploaderPosition()
{
#if defined(USE_GPS)    
    processGPSData();

    toSerialConsole(F("Your position from GPS:\n"));
    toSerialConsole(F("Your GPS latitude:\t")); toSerialConsole(Telemetry.uploader_position[0],5); toSerialConsole(F("\n"));
    toSerialConsole(F("Your GPS longitude:\t")); toSerialConsole(Telemetry.uploader_position[1],5); toSerialConsole(F("\n"));
    toSerialConsole(F("Your GPS Altitude:\t")); toSerialConsole(Telemetry.uploader_position[2],0); toSerialConsole(F("\n"));
#else
    Telemetry.uploader_position[0] = UPL_LAT;
    Telemetry.uploader_position[1] = UPL_LON;
    Telemetry.uploader_position[2] = UPL_ALT;
    toSerialConsole(F("Your position from settings:\n"));
    toSerialConsole(F("Your latitude:\t")); toSerialConsole(Telemetry.uploader_position[0],5); toSerialConsole(F("\n"));
    toSerialConsole(F("Your longitude:\t")); toSerialConsole(Telemetry.uploader_position[1],5); toSerialConsole(F("\n"));
    toSerialConsole(F("Your Altitude:\t")); toSerialConsole(Telemetry.uploader_position[2],0); toSerialConsole(F("\n"));
#endif
}

/************************************************************************************
* Determine the distance and bearing from your position to the tracker
************************************************************************************/
void setDistanceAndBearing()
{
   // Show the distance between the receiver and the balloon
   Telemetry.distance = GPSDistance(Telemetry.uploader_position[0],Telemetry.uploader_position[1],Telemetry.lat,Telemetry.lon);
   toSerialConsole(F("Distance (km):\t\t")); toSerialConsole(Telemetry.distance,2); toSerialConsole(F("\n"));

   // Show the bearing between the receiver and the balloon
   Telemetry.bearing = GPSBearing(Telemetry.uploader_position[0],Telemetry.uploader_position[1],Telemetry.lat,Telemetry.lon);

   // Get the bearing in compass direction
   Telemetry.compass = degToCompass(Telemetry.bearing);
   toSerialConsole(F("Bearing (degr.):\t")); toSerialConsole(Telemetry.bearing,0); toSerialConsole(F(" - travel ")); toSerialConsole(Telemetry.compass); toSerialConsole(F(" to chase\n"));
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
    
   toSerialConsole(F("Telemetry:\t\t")); toSerialConsole(dataStr); toSerialConsole(F("\n"));
   toSerialConsole(F("CRC received:\t\t")); 
   for (int n=0; n<=3; n++) 
   { 
      toSerialConsole(crcStr[n]);   
   }
   toSerialConsole(F("\nCRC calculated:\t\t")); toSerialConsole(crcCalculated);

   if ((crcCalculated[0] == crcStr[0]) && (crcCalculated[1] == crcStr[1]) && (crcCalculated[2] == crcStr[2]) && (crcCalculated[3] == crcStr[3]) )
   {
     toSerialConsole(F(" << CRC OK\n"));
     return true;
   }
   else
   {
     toSerialConsole(F(" << CRC NOT CORRECT!\n"));
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
