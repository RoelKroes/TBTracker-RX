/************************************************************************************
* GPS related stuff
************************************************************************************/

#if defined(USE_GPS)

#include <TinyGPS++.h>                       

// Global object which contains GPS data
TinyGPSPlus gps;                            

/************************************************************************************
* Get data from the GPS
************************************************************************************/
static void smartDelay(unsigned long ms)                
{
  unsigned long start = millis();
  do
  {
    while (Serial2.available() && !receivedFlag)
      gps.encode(Serial2.read());
  } while ((millis() - start < ms) && !receivedFlag);

  // Set gps_valid flag, used to display GPS status on OLED display
  if (gps.satellites.value() > 3) 
    gps_valid = true;
  else
    gps_valid = false;

#if defined(GPS_DEBUG)
  Serial.println(gps.charsProcessed() + (String)" GPS chars read, " + gps.passedChecksum() + " valid GPS sentances, " + gps.satellites.value() + " GPS sats.");
#endif
}

/************************************************************************************
* Get data from the GPS
************************************************************************************/
static void processGPSData()
{
  // Location
  if (gps.location.isValid())
  { 
    Telemetry.uploader_position[0] = gps.location.lat();
    Telemetry.uploader_position[1] = gps.location.lng();
  }
  else
  {
    // Invalid GPS position, use data from settings.h 
    Telemetry.uploader_position[0] = UPL_LAT;
    Telemetry.uploader_position[1] = UPL_LON;
  }

 // Altitude
 if (gps.altitude.isValid())
    Telemetry.uploader_position[2] = gps.altitude.meters();
 else
   // Invalid altitude, use data from settings.h
   Telemetry.uploader_position[2] = UPL_ALT;   
}

#endif


/************************************************************************************
* Compute great-circle distance in km, using haversine formula
* 
* The haversine formula 'remains particularly well-conditioned for numerical 
* computation even at small distances'
* 
* It was published by R W Sinnott in Sky and Telescope, 1984, though known about 
* for much longer by navigators. (For the curious, c is the angular distance in 
* radians, and a is the square of half the chord length between the points).
* 
* We don't adjust for altitude. Maybe in a future version. 
************************************************************************************/
float GPSDistance(float lat1, float lon1, float lat2, float lon2)
{
  float ToRad = PI / 180.0;
  float R = 6371;   // radius earth in Km
  
  float dLat = (lat2-lat1) * ToRad;
  float dLon = (lon2-lon1) * ToRad; 
  
  float a = sin(dLat/2) * sin(dLat/2) +
        cos(lat1 * ToRad) * cos(lat2 * ToRad) * 
        sin(dLon/2) * sin(dLon/2); 
        
  float c = 2 * atan2(sqrt(a), sqrt(1-a)); 
  
  float d = R * c;
  return d;
}

/************************************************************************************
* Compute bearing from current location to the location of the payload
* Can be used for chasing a payload.
* 
* lat = current latitude
* lon = current longitude
* lat2 = payload latitude
* lon2 = payload longitude
************************************************************************************/
float GPSBearing(float lat,float lon,float lat2,float lon2){

    float teta1 = radians(lat);
    float teta2 = radians(lat2);
    // float delta1 = radians(lat2-lat); not used
    float delta2 = radians(lon2-lon);
    float y = sin(delta2) * cos(teta2);
    float x = cos(teta1)*sin(teta2) - sin(teta1)*cos(teta2)*cos(delta2);
    float brng = atan2(y,x);
    brng = degrees(brng);// radians to degrees
    brng = ( ((int)brng + 360) % 360 ); 
    return brng;
  }


/************************************************************************************
 * Convert the bearing in degrees to a bearing in compass wind directions
 ************************************************************************************/
String degToCompass(int num)
{
  int val=int((num/22.5)+.5);
  const char *arr[] = {"N","NNE","NE","ENE","E","ESE", "SE", "SSE","S","SSW","SW","WSW","W","WNW","NW","NNW"};
  return String(arr[(val % 16)]);
}
