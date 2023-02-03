/************************************************************************************
* Experimental functions to decode LoRa-APRS 
************************************************************************************/

void parseAPRSPacket(String message)
{
  String gps_Lat;
  String gps_Long;
  String gps_Alt;

  // Get the source of the APRS packet
  int pos_Src = message.indexOf('>');
  if (pos_Src >= 0)
  {
    Telemetry.payload_callsign = message.substring(0, pos_Src);
    Serial.print(F("APRS source:\t")); Serial.println(Telemetry.payload_callsign);
  }

  // Get the location of the APRS payload
  int pos_Loc = message.indexOf(':');
  if (pos_Loc > 0)
  {
    // Get the latitude
    gps_Lat = message.substring(pos_Loc+2,pos_Loc+10);
    // Convert the latitude to decimal
    Telemetry.lat = gps_Lat.substring(0,2).toFloat()+(gps_Lat.substring(2,7).toFloat() / 60);
    if (gps_Lat[7]== 'S')
    {
      Telemetry.lat = Telemetry.lat*-1;
    }
    Serial.print("APRS latitude:\t"); Serial.println(Telemetry.lat,5);

    // Get the longitude
    gps_Long = message.substring(pos_Loc+11,pos_Loc+20);
    Telemetry.lon = gps_Long.substring(0,3).toFloat()+(gps_Long.substring(3,8).toFloat() / 60);
    Serial.print("APRS longitude:\t"); Serial.println(Telemetry.lon,5);
    if (gps_Long[8] == 'W')
    {
      Telemetry.lon = Telemetry.lon*-1;
    }
  }
  
  // Get the altitude of the APRS payload
  int pos_Alt = message.indexOf("/A=");
  if (pos_Alt > 0)
  {
    // altitude is in feet
    gps_Alt = message.substring(pos_Alt+3,pos_Alt+9);
    // Convert to meters
    Telemetry.alt = gps_Alt.toFloat() / 3.2808;
    Serial.print(F("APRS altitude:\t")); Serial.print(Telemetry.alt,0); Serial.println(F(" meter"));
  }
  else
  {
    Telemetry.alt=0;
  }

  // Determine your location
  setUploaderPosition();
  
  // Determine the distance and bearing of the payload
  setDistanceAndBearing();

  // Create a visual end-of-packet on the Serial console
  closePacket();
  
  // Update the SSD1306 display
#if defined(USE_SSD1306)
   displayUpdate(); 
#endif       
}
